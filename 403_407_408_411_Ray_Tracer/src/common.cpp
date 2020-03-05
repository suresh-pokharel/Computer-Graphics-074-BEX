#include "common.h"

#define DEFAULT_ARENA_SIZE MB_TO_BYTES(10)
static void *xcalloc(size_t n, size_t size ){
  void *ptr = calloc(n, size );
  if ( !ptr ){
    perror("Malloc failed! ");
    return NULL;
  } else {
    return ptr;
  }
}

static void *xmalloc( size_t size ){
  void *ptr = malloc( size );
  if ( !ptr ){
    perror("Malloc failed! ");
    return NULL;
  } else {
    return ptr;
  }
}

bool internal_array_grow(void** base, uint32_t count, size_t size){
    BufferContext *hdr;
    size_t n_count = MAX( ARRAY_MIN_BUFFER_COUNT, ( array_capacity( *base ) + count ) * 2 );
    size_t n_size = n_count * size + sizeof( BufferContext ) ;
    if ( *base == NULL ){
        hdr = (BufferContext *)malloc( n_size );
        hdr->length = 0;
    } else {
        assert( array_capacity( *base ) >= array_length( *base ) );
        hdr = ( BufferContext *)realloc( (void *)array_buffer_hdr( *base ), n_size );
        if ( !hdr )return false;
    }

    hdr->capacity = n_count;
    *base = (char *)hdr + sizeof(BufferContext);
    return true;
}

bool internal_array_append(void** base,
                           void* data,
                           uint32_t count,
                           size_t size)
{
    if ( !array_fits( *base , count ) ){
      if ( !internal_array_grow( base, count, size ) ) return false;
    }

    memcpy( (char *)(*base) + array_length( *base ) * size,
            data,
            size * count );
    
    array_buffer_hdr( *base )->length += count;
    return true;
}


void *arena_alloc( Arena *arena, size_t size, size_t align ){
  uint len = array_length( arena->buff );
  // search through arenas to find a suitable block
  for ( uint i = 0; i < len; i++ ){
    if ( arena->buff_capacity[i] >= arena->buff_len[i] + size ){
      void *p = ( char *)arena->buff[i] + arena->buff_len[i];
      void *ptr = ALIGN_UP_PTR( p, align );
      ptrdiff diff = (char *)ptr - (char *)p;
      arena->buff_len[ i ] += ( size + ( uint )diff );
      return ptr;
    }
  }

  // if suitable block is not found
  if ( size > UINT_MAX/ 2 ){
    fprintf( stderr, "size exceeds the max. size that can be alloced at once" );
    return NULL;
  }
  size_t n_size = MAX( DEFAULT_ARENA_SIZE, 2 * size );
  void *x = xcalloc( 1, n_size );
  array_push( arena->buff, x );
  array_push( arena->buff_len, size );
  array_push( arena->buff_capacity, n_size );
  return x;
}

void arena_free( Arena *arena ){
  array_free( arena->buff_len );
  array_free( arena->buff_capacity );
  for ( uint i = 0; i < array_length( arena->buff ); i++ ){
    free( arena->buff[i] );
  }
  array_free( arena->buff );
}

Arena new_arena( void ){
  Arena a;
  a.buff = NULL;
  a.buff_len = 0;
  a.buff_capacity = 0;
  return a;
}

int read_text_file_to_buffer( const char *path, char **buffer ){
  int fd = open( path, O_RDONLY );
  if ( fd == -1 ){
    fprintf(stderr,"%s, ",path );
    perror("Unable to open file for reading! ");
    return -1;
  }

  struct stat statbuff;
  if ( fstat( fd, &statbuff ) == -1 ){
    perror("Unable to get file stats! fstat failed: ");
    goto cleanup1;
  }

  uint64_t size;
  size = statbuff.st_size;

  char *buff; 
  buff = ( char * )xmalloc( size + 1 );
  if ( !buff ){
    goto cleanup1;
  }

  if ( read( fd, buff, size ) == -1 ){
    perror( "Read from file failed! ");
    goto cleanup2;
  }

 *buffer = buff;
 buff[ size ] = 0;
 return 0;

cleanup2:
  free( buff );
cleanup1:
  close( fd );
  return -1;
}

#if 0
static void *xmalloc( size_t size ){
     void *x = malloc(size);
     if ( !x ){
//          errExit("Unable to allocate memory!\n");
          perror("Unable to allocate Memory for Strings! ");
     }
     assert( x != NULL );
     return x;
}
#endif


static void *xrealloc( void *buff,size_t size ){
     void *n = realloc( buff,size );
     if ( !n ){
          perror("Unable to allocate Memory for Strings! ");
     }
     assert( n != NULL );
     return n;
}
char* _init_string( char *str ){
     if ( str != NULL ){
          fprintf(stderr,"Not a null string pointer!\n");
          assert( 0 );
          return NULL;
     } else {
          strHdr *n = (strHdr *)xcalloc( 1,sizeof(strHdr ) + MIN_SIZE );
          assert ( n );
          n->len = 0;
          n->cap = MIN_SIZE;
          assert( n->cap >= n->len + 1 ); // the invariant for the whole string process
          return n->str;
     }
}

char *_init_string_with_size( char *str , size_t size ){
     if ( str != NULL ){
          fprintf(stderr,"Not a null string pointer!\n");
          assert( 0 );
          return NULL;
     } else {
          strHdr *n = (strHdr *)xcalloc( 1,sizeof(strHdr ) + size );
          assert ( n );
          n->len = 0;
          n->cap = size;
          assert( n->cap >= n->len + 1 ); // the invariant for the whole string process
          return n->str;
     }
}

char *string_grow( strHdr *x, size_t n){
//     char *y = NULL;
     assert( x );
     size_t alloc_size = MAX( 2*(x->cap) + 1, MAX( MIN_SIZE, n ) );
     assert( alloc_size >= ( x->cap ) * 2 );
     strHdr *ns =(strHdr *)xrealloc( x , alloc_size + offsetof( strHdr, str) );
     ns->cap = ( alloc_size );
     assert( ns->cap >= ns->len + 1 ); // the invariant for the whole string process
     return ns->str;
}

char *_string_append(char *x, const char *y){
     size_t len = strlen(y);
     size_t size = str_size(x);
     if ( str_size(x) + len > str_cap(x) ){
          x = string_grow( str_hdr(x), size + len );
     }

     assert( str_cap(x) >= size + len );
     memcpy( x + str_len(x) , y , len + 1 );
     str_hdr(x)->len += len;
     return x;
}

char *_string_print(char *x, const char *fmt, ... ){
     va_list args;
     va_start(args,fmt);
     size_t cap = str_cap(x);
     size_t len = vsnprintf(x,cap,fmt,args);
     va_start(args,fmt);
     if ( ( len + 1 ) * sizeof(char)  > cap ){
          x = string_grow( str_hdr(x) , len + 1);
          cap = str_cap(x);
          len = vsnprintf(x,cap,fmt,args);
          assert( len <= cap);
     }
     va_end(args);
     str_hdr(x)->len = len;
     return x;
}

char *_string_app_print( char *x , const char *fmt ,... ){
     va_list args;
     va_start(args,fmt);
     size_t size = str_size(x);
     size_t cap = str_cap(x);
     size_t len = vsnprintf( x + size - 1, cap - size , fmt, args );
     va_start(args,fmt);
     if ( ( len + 1 )*sizeof(char) > cap - size  ){
          x = string_grow( str_hdr(x), cap + size + len );
          cap = str_cap(x);
          len = vsnprintf( x + size - 1, cap - size, fmt, args );
          assert( len <= cap - size );
     }
     va_end(args);
     str_hdr(x)->len += len;
     return x;
}
#if 0
// function deprecated until furthuer notice
char *_stringn_app_print( char *x , size_t n ,  const char *fmt ,... ){
     va_list args;
     va_start(args,fmt);
     size_t size = str_size(x);
     size_t cap = str_cap(x);
     size_t len = vsnprintf( x + size - 1, ( n < cap - size )?( n ):(cap-size) , fmt, args );
     va_start(args,fmt);
     if ( ( len + 1 )*sizeof(char) > cap - size  ){
          x = string_grow( str_hdr(x), cap + size + len );
          cap = str_cap(x);
          len = vsnprintf( x + size - 1, ( ( n ) < cap - size )?( n ):(cap-size), fmt, args );
          assert( len <= cap - size );
     }
     va_end(args);
     str_hdr(x)->len += len;
     return x;
}
#endif

char *_strn_append( char *dest, size_t n, char *src ){
     if ( str_size( dest ) + n > str_cap( dest ) ){
          dest = string_grow( str_hdr( dest ),n * 2 );  
     } 
     char *x  = dest + str_len( dest );
     for ( size_t i = 0; i < n ; i++ ){
          *x++ = *src++;
     }
     str_hdr(dest)->len += n;
     *x = 0;
     return dest;
}

char *_string_app_char( char *x, char c ){ // append a single character at the end of string
     if ( str_size( x ) + 1 >= str_cap( x ) ){
          x = string_grow( str_hdr(x), str_cap(x) * 2 );
     }
     strHdr *s = str_hdr(x);
     assert( s->len <= s->cap );
     x[ s->len ] = c;
     s->len++;
     x[ s->len ] = 0;
     return x;
}

static void swap( char *x, char *y ){
     char temp = *x;
     *x = *y;
     *y = temp;
}


void _delete_char_pos(char *str, size_t pos ){
     size_t len = str_len( str );
     // len is the index of the null character in the string, len - 1 is the last proper character
     if ( ( len == 0 ) || ( pos > len - 1 ) ) {
          return;
     } else {
          // At the end swaps the deleted character and the '\0' character
          for ( char *s = str + pos; s != str + len; s++ ){
               swap ( s, s+1 );
          }
          ( str_hdr(str)->len )--;
     }
}

char *_insert_string( char *x, char *y, size_t pos ){
     /*
      * x is the destination string, y is the source string
      * pos is the index at which the string y is inserted
      */
     size_t srcLen = strlen(y); // the number of bytes the stirng occupies
     size_t destLen = str_len( x );
     assert( pos >= 0 && pos <= destLen );
     if ( srcLen + str_size(x) >= str_cap( x ) ){
          x = string_grow( str_hdr( x ) , srcLen* 2 );
     }
     for ( char *s = x + destLen; s != x + pos - 1 ; s-- ){
          *( s + srcLen  ) = *s;
     }
     for ( size_t i = 0; i < srcLen; i++ ){
          x[ pos + i ] =  y[i];
     }
     str_hdr(x)->len += srcLen;
     x[ str_len(x) ] = 0;
     return x;
}

char *_insert_char( char *str, char c, size_t pos ){
     if ( str_size( str ) + 1 >= str_cap( str ) ){
          str = string_grow( str_hdr( str ), str_cap(str) << 2 );
     }
     size_t len = str_len( str );
     assert( pos <= len );
     for ( char *s = str + len + 1 ; s != str + pos; s-- ){
          swap( s , s - 1 );
     }
     str[ pos ] = c;
     ( str_hdr( str )->len ) += 1;
     return str;
}

char *string_delete_range( char *str, const int start, const int end ){
     // delete string from index start to index end ( inclusive )
     // e.g. delete_range( "abcdefghijk", 2, 5 ) gives "abghijk"
     assert( start > 0 ); 
     assert( end > start );
     int len = str_len( str );
     if ( end >= len ){
          str[ start ] = 0;
          str_hdr( str )->len = start;
          return str; 
     }
     for ( int i = end + 1 , beg = start; i <= len ; i++, beg++ ){
          swap( str + i  , str + beg );
     }
     str_hdr( str )->len -= ( end - start + 1 ); 
     str[ str_len( str ) ] = 0;
     return str;
}
#if  0
void str_test(void){
     char *s1 = NULL;  
     str_init(s1);
     str_append(s1,"fuck");
     str_app_print( s1, "%s%d\n", "this",32 );
     str_print( s1, "%s\n","Working!");
     char *s2 = NULL;
     str_init( s2 );
     //strn_app_print( s2, 3, "%s","aaaa" );
     strn_append( s2,3,"aaaa"); 
     assert( strcmp( s2, "aaa" ) == 0 );
     str_app_char( s2,'f');
     str_app_char( s2,'u');
     str_app_char( s2,'c');
     str_app_char( s2,'k');
     assert( strcmp( s2,"aaafuck") == 0 );
     str_delete_char_pos( s2, 1 );
     assert( strcmp( s2,"aafuck" ) == 0 );
     str_delete_char_pos( s2, str_len(s2)-1 );
     assert( strcmp( s2, "aafuc" ) == 0 );
     str_insert_char( s2, 'b', 1 );
     assert( strcmp( s2, "abafuc" ) == 0 );
     char *s3 = NULL;
     str_init( s3 );
     str_delete_char_pos( s3, 0 );
     str_insert_char( s3, '3', 0 );
     str_insert_char( s3,'2',0);
     assert( strcmp( s3, "23" ) == 0 );
     assert( str_len( s3 ) == strlen( s3 ) );
     str_insert_str( s3, "45", 2 );
#define STR_EQ( x,y ) ( ( strcmp( x ,y ) == 0 ) )
     assert( strcmp( s3, "2345" ) == 0 );
     str_insert_str( s3, "fuck",1 );
     printf("%s\n",s3 );
     assert( STR_EQ( s3,"2fuck345" ) );
     assert( str_len( s3 ) == strlen( s3 ) );

     char *s4 = NULL;
     str_init( s4 );
     str_insert_str( s4,"fuck",0 );
     char *s5 = NULL;
     str_init_size( s5 , 1 );
     str_app_char( s5, 'a' );
     str_app_char( s5, 'b' );
     str_app_char( s5, 'c' );
     str_app_char( s5, 'd' );
     assert( STR_EQ( s5, "abcd" ) );
     assert( str_len( s5 ) == strlen( s5 ) );
     str_insert_str( s5,"efghijkl", 4 );
     assert( STR_EQ( s5, "abcdefghijkl" ) );
     assert( str_len( s5 ) == strlen( s5 ) );
     str_delete_range( s5, 2, 5 );
     assert( STR_EQ( s5, "abghijkl" ) );
     assert( str_len( s5 ) == strlen( s5 ) );
     str_delete_range( s5, 3, str_len( s5 ) );
     assert( STR_EQ( s5, "abg" ) );
     assert( str_len( s5 ) == strlen( s5 ) );
     //str_append(s1," this shit\n");
     //for ( int i = 0; i < 32; i++ ){
     //     str_append(s1,"!");
     //}
     //str_append(s1,"\n");
     //printf("%s",s1);
//     str_print(s1,"%s\n","fuck");
//     printf("%s",s1); 
     str_free(s1);
     str_free(s2);
     str_free(s3);
     str_free(s4);
     str_free(s5);
}

#endif


void print_error_exit(const char *format, ... ){
     va_list args;
     va_start(args,format);
     fprintf( stderr, "FATAL : ");
     vfprintf(stderr,format,args);
     va_end(args);
     assert( 0 );
}

function ObjLoader(src){
    this.str = src;
    this.index = 0;
    
    //First find the material library
    this.nextLine=()=>{
        let start = this.index;
        for(;this.str[this.index]!=='\n'&&
                this.index<this.length;this.index++);
                
        let ret_str = (this.str.substring(start,this.index)).trim();
        
        if(this.str[this.index]==='\n')
            this.index++; 
        
        return ret_str;
    }
}

export default ObjLoader;
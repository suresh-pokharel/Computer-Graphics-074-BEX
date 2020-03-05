import {Vec3} from '../math/math'

class PlyLoader{
    constructor(str){
        this.index = 0;
        this.str = str;
        this.length = str.length;
        this.description = [];
        if(this.nextLine()!=='ply'){
            console.error('File doesn\'t seems to be ply format');
            return;
        }

        //Check for Format
        let things  = this.nextLine().split(' ');
        if(things[0]!=='format'){
            console.error("File Format not found");
            return;
        }else if(things[1].indexOf('ascii')!==0){
           console.error("Only Ascii format allowed(^_^) Thanks!");
           return;
        }

        //Store Comments into description
        let nextLin = this.nextLine();
        while(true){
            if(nextLin.indexOf('comment')===0){
                this.description.push(nextLin.substring(8));
            }else
                break;
            nextLin = this.nextLine();
        }

        //Create element format 
        this.elements = [];
        things = nextLin.split(' ');
        while(true){
            if(things[0]==='element'){
                let count = parseInt(things[2]);
                if(things[1]!==''&&!Number.isNaN(count)){
                    let element = {type: things[1], count: count, property:[]};
                    let go = true;
                    while(go){
                        things = this.nextLine().split(' ');
                        if(things[0]==='property'){
                            if(things[1]==='list'){
                                element.property.push({type: things[1], first: things[2], second: things[3], name: things[4]});
                            }
                            else
                                element.property.push({type: things[1], name: things[2]})
                        }else{
                            go=false;
                        }
                    }
                    this.elements.push(element);
                }
            }else
                break;
        }

        if(things[0]!=='end_header'){
            console.error('End of Header not specified');
        }

        let vertices=[], indices=[];
        for(let i=0;i<this.elements.length;i++){
            let element = this.elements[i];
            if(element.type==='vertex'){
                for(let i=0;i<element.count;i++){
                    things = this.nextLine().split(' ');
                    for(let j=0;j<element.property.length;j++){
                        vertices.push(parseFloat(things[j]));
                    }    
                }
            }else if(element.type==='face'){
                if(element.property[0].type==='list'){
                    for(let i=0;i<element.count;i++){
                        things = this.nextLine().split(' ');
                        let count = parseInt(things[0]);

                        //This does triangulation
                        for(let j=1;j<count-1;j++){
                            indices.push(parseInt(things[1]));
                            indices.push(parseInt(things[j+1]));
                            indices.push(parseInt(things[j+2]));
                        }
                    }
                }else{
                    console.error("Can't read vertex_indices with more than one property");
                    return;
                }
            }else{
                console.error("Can't read element of type "+element.type);
                return;
            }
        }

        //Normalize the vertexcolors
        let stride = this.elements[0].property.length;
        let ind = this.getIndexOf('red', 0);
        if(ind!==-1){
            console.log('Color Index ' +ind.toString());
            console.log(`${vertices[ind]}, ${vertices[ind+1]}, ${vertices[ind+2]}, ${vertices[ind+3]}`)
            if(this.elements[0].property[ind].type==='uchar'){
                for(let i=ind;i<vertices.length;i+=stride){
                    vertices[i] /=255;
                    vertices[i+1] /=255;
                    vertices[i+2] /=255;
                    vertices[i+3] =1;
                }
            }
            console.log(`${vertices[ind]}, ${vertices[ind+1]}, ${vertices[ind+2]}, ${vertices[ind+3]}`)
        }

        //Here Calculate the normals
        let vertices_with_normal = vertices;
        ind = this.getIndexOf('nx',0);
        console.log('Normal Index: '+ind.toString());
        if(ind===-1){
            let vec1, vec2;
            let i1,i2,i3,i4,i5,i6,i7,i8,i9;
            let normals = Array(3*vertices.length/stride).fill(null);
            for(let i=0;i<indices.length;i+=3){

                let a = stride*indices[i];
                i1 = vertices[a+0];
                i2 = vertices[a+1];
                i3 = vertices[a+2];
                // console.log(a);

                a = stride*indices[i+1];
                i4 = vertices[a+0];
                i5 = vertices[a+1];
                i6 = vertices[a+2];
                // console.log(a);

                a = stride*indices[i+2];
                i7 = vertices[a+0];
                i8 = vertices[a+1];
                i9 = vertices[a+2];
                // console.log(a);
                
                // console.log({i1, i2, i3, i4, i5, i6, i7, i8, i9})

                vec1 = new Vec3(i4-i1, i5-i2, i6-i3);
                vec2 = new Vec3(i7-i4, i8-i5, i9-i6);
                // console.log(vec2);
                // console.log(vec1);
                vec2 = vec1.cross(vec2);
                
                
                for(let j=0;j<3;j++){
                    a = 3*indices[j+i];
                    if(normals[a]===null){
                        normals[a] = vec2.x;
                        normals[a+1] = vec2.y;
                        normals[a+2] = vec2.z;
                    }
                }
            }

            //Here Combine normals and the vertices Okay
            let length = vertices.length+normals.length;
            vertices_with_normal= Array(length);
            for(let i=0,j=0,l=0;i<length;){
                for(let k=0;k<stride;j++,k++,i++){
                    vertices_with_normal[i] = vertices[j];
                }
                for(let k=0;k<3;k++,l++,i++){
                    vertices_with_normal[i] = normals[l];
                }
            }
        }

        if(this.getIndexOf('red',0)!==-1)
            return {vertices:vertices_with_normal, indices:indices, textured:true};
        else
            return {vertices:vertices_with_normal, indices:indices, textured:false};
    }


    nextLine(){
        let start = this.index;
        for(;this.str[this.index]!=='\n'&&
                this.index<this.length;this.index++);
                
        let ret_str = (this.str.substring(start,this.index)).trim();
        
        if(this.str[this.index]==='\n')
            this.index++; 
        
        return ret_str;
    }

    getIndexOf(str,index){
        let property = this.elements[index].property;
        for(let i=0;i<property.length;i++){
            if(property[i].name===str)
                return i;
        }
        return -1;
    }
}

export default PlyLoader;
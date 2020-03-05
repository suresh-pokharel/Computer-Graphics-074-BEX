import {Mat4f,Mat3f,Vec3} from '../math/math'
import {initShader} from './compile'

//Will Implement translate, rotate, etc
class FilledShape3D{
    constructor(gl, setting){
        if(!FilledShape3D.initialized)
            FilledShape3D.init(gl);
        this.vArray = []
        this.centroid = new Vec3(0,0,0);
        if(setting.indices == null){
            this.no_indices = true;
            for(let i=0;i<setting.num_face;i++){
                this.vArray[i] = gl.createVertexArray();
                gl.bindVertexArray(this.vArray[i]);

                let vBuffer = gl.createBuffer();
                gl.bindBuffer(gl.ARRAY_BUFFER, vBuffer);
                gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(setting.coords[i]), gl.STATIC_DRAW);
                
                //VertexAttribute
                gl.vertexAttribPointer(FilledShape3D.text_cord_loc, 2, gl.FLOAT, true, 20, 12);
                gl.enableVertexAttribArray(FilledShape3D.text_cord_loc);
                
                gl.vertexAttribPointer(FilledShape3D.coordinate_loc, 3, gl.FLOAT, true, 20, 0);
                gl.enableVertexAttribArray(FilledShape3D.coordinate_loc);

                //Calculation of centroid:
                for(let j=0;j<setting.nvp;j++){
                    this.centroid.x +=  setting.coords[i][5*j + 0];
                    this.centroid.y +=  setting.coords[i][5*j + 1];
                    this.centroid.z +=  setting.coords[i][5*j + 2];
                }

                gl.bindVertexArray(null);
                gl.bindBuffer(gl.ARRAY_BUFFER, null);
            }
        }

        this.centroid.x /= setting.nvp*setting.num_face;
        this.centroid.y /= setting.nvp*setting.num_face;
        this.centroid.z /= setting.nvp*setting.num_face;
        
        console.log("Centroid: ("+this.centroid.x.toString()+", "+this.centroid.y.toString()
                        +", "+this.centroid.z.toString()+")");

        this.texture = []
        //Texture:
        for(let i=0;i<setting.num_face;i++){
            this.texture[i] = gl.createTexture();
            gl.bindTexture(gl.TEXTURE_2D, this.texture[i]);
            
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
            
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
            
            gl.texImage2D(gl.TEXTURE_2D, 
                            0, 
                            gl.RGBA, 
                            setting.texture.width[i],
                            setting.texture.height[i], 
                            0, 
                            gl.RGBA, 
                            gl.UNSIGNED_BYTE,
                            new Uint8Array(setting.texture.data[i])
                            );
            gl.bindTexture(gl.TEXTURE_2D, null);
        }
        
        //Model Matrix
        this.model_mat = new Mat4f();

        this.numVert = setting.nvp;
        this.num_face = setting.num_face;
    }
  
    render(gl){
        
        gl.useProgram(FilledShape3D.program);
        if(this.no_indices){
            gl.uniformMatrix4fv(FilledShape3D.model_loc, false, this.model_mat.content);
            gl.uniformMatrix4fv(FilledShape3D.proj_loc, false, gl.proj_mat.content); 
            
            //Originally opengl supposes column major, change to row major by transpose
            gl.uniformMatrix4fv(FilledShape3D.view_loc, true, gl.cam.mat.content);  
            for(let i=0;i<this.num_face;i++){
                gl.bindTexture(gl.TEXTURE_2D, this.texture[i]);
                gl.bindVertexArray(this.vArray[i]);
                gl.drawArrays(gl.TRIANGLE_FAN, 0, this.numVert);
            }
            gl.bindVertexArray(null);
        }
    }

    rotate(axis,angle){

    }

    translate(){

    }

    static init(gl){
        let program = FilledShape3D.program = initShader(gl, FilledShape3D.vertexShader, 
                                                FilledShape3D.fragmentShader);
        FilledShape3D.coordinate_loc = gl.getAttribLocation(program, 'coordinate');
        FilledShape3D.tex_cord_loc = gl.getAttribLocation(program, 'tCord');
        FilledShape3D.model_loc = gl.getUniformLocation(program, 'model');
        FilledShape3D.proj_loc = gl.getUniformLocation(program, 'proj');
        FilledShape3D.view_loc = gl.getUniformLocation(program,'view');
        FilledShape3D.initialized = true;
    }

}

FilledShape3D.vertexShader =
                            "attribute vec2 tCord;\n"+
                            "attribute vec3 coordinate;\n"+
                            "uniform mat4 model;\n"+
                            "uniform mat4 view;\n"+
                            "uniform mat4 proj;\n"+
                            "varying vec2 vTex;\n"+
                            "void main(){\n"+
                            "   vTex = tCord;\n"+
                            "   gl_Position = proj*(view*(model*(vec4(coordinate.xyz, 1.0))));\n"+
                            "}";

FilledShape3D.fragmentShader =  "precision mediump float;\n"+		
                                "varying vec2 vTex;\n"+
                                "uniform sampler2D texData;\n"+	
                                "void main(){\n"+
                                "    gl_FragColor = texture2D(texData, vTex);\n"+
                                "}";

FilledShape3D.initialized = false;

function get_colored_surface(gl,setting){
    get_colored_surface.tex_cords = [
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0,1.0,
    ]

    let setting_to_send = {nvp:4, num_face: 1,coords:[], texture:{ height:[1],width:[1], data:[setting.color]}};
    setting_to_send.coords[0] = [];

    for(let i=0,j=0;j<get_colored_surface.tex_cords.length;i+=3, j+=2){
        let a = 5*j/2;
        console.log(a);
        setting_to_send.coords[0][a + 0] = setting.coords[i];
        setting_to_send.coords[0][a + 1] = setting.coords[i+1];
        setting_to_send.coords[0][a + 2] = setting.coords[i+2];

        console.log(`(${setting.coords[i]},${setting.coords[i+1]},${setting.coords[i+2]})`)

        setting_to_send.coords[0][a + 3] = get_colored_surface.tex_cords[j+0];
        setting_to_send.coords[0][a + 4] = get_colored_surface.tex_cords[j+1]; 
    }
    return new FilledShape3D(gl,setting_to_send);
}

function get_textured_cube(gl, settings){
    get_textured_cube.indices = new Uint16Array([
        0, 1, 2, 3,
        0, 1, 5, 4,
        
        0, 3, 7, 4,
        1, 2, 6, 5,
        4, 5, 6,  7,
        
        2, 3, 7, 6
    ]);

    get_textured_cube.tex_cords = [
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0,1.0,
    ]

    let cord_set = [
        settings.coords[0], settings.coords[1], settings.coords[2],
        settings.coords[3], settings.coords[1], settings.coords[2],
        settings.coords[3], settings.coords[4], settings.coords[2],
        settings.coords[0], settings.coords[4], settings.coords[2],
        settings.coords[0], settings.coords[1], settings.coords[5],
        settings.coords[3], settings.coords[1], settings.coords[5],
        settings.coords[3], settings.coords[4], settings.coords[5],
        settings.coords[0], settings.coords[4], settings.coords[5],
    ]
    
    let setting_to_send = { coords:[], texture:settings.texture, num_face: 6, nvp: 4};
    for(let i=0;i<6;i++){
        setting_to_send.coords[i] = []
        
        for(let j=0;j<4;j++){
            let disp = 3*get_textured_cube.indices[i*4 + j];
            
            //Vertices
            setting_to_send.coords[i].push(cord_set[ disp + 0]);
            setting_to_send.coords[i].push(cord_set[ disp + 1]);
            setting_to_send.coords[i].push(cord_set[ disp + 2]);

            //Texture Coordinate
            setting_to_send.coords[i].push(get_textured_cube.tex_cords[2*j  + 0]);
            setting_to_send.coords[i].push(get_textured_cube.tex_cords[2*j  + 1]);

        }
    }
    return new FilledShape3D(gl,setting_to_send);
}

class FilledShape2D{
    constructor(gl, setting){
        if(!FilledShape2D.initialized)
            FilledShape2D.init(gl);
        
        this.vArray = gl.createVertexArray();
        gl.bindVertexArray(this.vArray); 
        
        this.vBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.vBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(setting.coords), gl.STATIC_DRAW);
        
        //VertexAttribute
        
        gl.vertexAttribPointer(FilledShape2D.text_cord_loc, 2, gl.FLOAT, true, 16, 8);
        gl.enableVertexAttribArray(FilledShape2D.text_cord_loc);
        
        gl.vertexAttribPointer(FilledShape2D.coordinate_loc, 2, gl.FLOAT, true, 16, 0);
        gl.enableVertexAttribArray(FilledShape2D.coordinate_loc);


        gl.bindVertexArray(null);
        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        //Texture:
        this.texture = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, this.texture);
        
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        
        gl.texImage2D(gl.TEXTURE_2D, 
                        0, 
                        gl.RGB, 
                        setting.texture.width,
                        setting.texture.height, 
                        0, 
                        gl.RGB, 
                        gl.UNSIGNED_BYTE,
                        new Uint8Array(setting.texture.data)
                        );
        gl.bindTexture(gl.TEXTURE_2D, null);
        
        //Model Matrix
        this.model_mat = new Mat3f();
        this.numVert = setting.coords.length/4;
        
    }
    
    render(gl){
        gl.useProgram(FilledShape2D.program);
        gl.bindTexture(gl.TEXTURE_2D, this.texture);
        gl.uniformMatrix3fv(FilledShape2D.model_loc, false, this.model_mat.content);  
        gl.bindVertexArray(this.vArray);
        gl.drawArrays(gl.TRIANGLE_FAN, 0, this.numVert);
    }

    static init(gl){
        let program = FilledShape2D.program = initShader(gl, FilledShape2D.vertexShader, 
                                                FilledShape2D.fragmentShader);
        FilledShape2D.coordinate_loc = gl.getAttribLocation(program, 'coordinate');
        FilledShape2D.tex_cord_loc = gl.getAttribLocation(program, 'tCord');
        FilledShape2D.model_loc = gl.getUniformLocation(program, 'model');
        FilledShape2D.initialized = true;
    }



}


FilledShape2D.vertexShader =
                            "attribute vec2 tCord;\n"+
                            "attribute vec2 coordinate;\n"+
                            "uniform mat3 model;\n"+
                            "uniform mat3 view;\n"+
                            "uniform mat3 proj;\n"+
                            "varying vec2 vTex;\n"+
                            "void main(){\n"+
                            "   vTex = tCord;\n"+
                            "   vec3 newCord = model*vec3(coordinate.x, coordinate.y, 1.0);\n"+
                            "   gl_Position = vec4(newCord.xy,0.0,1.0);\n"+
                            "}";

FilledShape2D.fragmentShader =  "precision mediump float;\n"+		
                                "varying vec2 vTex;\n"+
                                "uniform sampler2D texData;\n"+	
                                "void main(){\n"+
                                "    gl_FragColor = texture2D(texData, vTex);\n"+
                                "}";
FilledShape2D.initialized = false;

class Grid3D{
    constructor(gl,bound=4){
        if(!Grid3D.init)
            this.init(gl);
        let lines = [];
        for(let i=-40;i<0;i++){
            let color = i%5===0?0.4:0.8;
            this.addLineX(lines,color,i/10,bound);
            this.addLineY(lines,color,i/10,bound);  
        }

        this.addLineX(lines,0.0,0,bound+bound/4);
        this.addLineY(lines,0.0,0,bound+bound/4);
        for(let i=1;i<40;i++){
            let color = i%5===0?0.4:0.8;
            this.addLineX(lines,color,i/10,bound);
            this.addLineY(lines,color,i/10,bound);  
        }
        this.addLineZ(lines,0.0,bound+bound/4);

        this.vArray = gl.createVertexArray();
        gl.bindVertexArray(this.vArray);
        this.vBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.vBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(lines), gl.STATIC_DRAW);
        gl.vertexAttribPointer(Grid3D.color_loc, 1, gl.FLOAT, true, 16, 12);
        gl.enableVertexAttribArray(Grid3D.color_loc);
        gl.vertexAttribPointer(Grid3D.coord_loc, 3, gl.FLOAT, true, 16, 0);
        gl.enableVertexAttribArray(Grid3D.cord_loc);
        gl.bindBuffer(gl.ARRAY_BUFFER,null);
        gl.bindVertexArray(null);
        this.render(gl);
    }

    addLineY(arr,color,offset,y){
        arr.push(offset);
        arr.push(-y);
        arr.push(0);
        arr.push(color);
        arr.push(offset);
        arr.push(y);
        arr.push(0);
        arr.push(color);
    }

    addLineX(arr,color,offset,x){
        arr.push(-x);
        arr.push(offset);
        arr.push(0);
        arr.push(color);
        arr.push(x);
        arr.push(offset);
        arr.push(0);
        arr.push(color);
    }

    addLineZ(arr,color,z){
        arr.push(0);
        arr.push(0);
        arr.push(-z);
        arr.push(color);
        arr.push(0);
        arr.push(0);
        arr.push(z);
        arr.push(color);
    }

    init(gl){
        let program = Grid3D.program = initShader(gl,Grid3D.vertexShader,Grid3D.fragmentShader);
        gl.useProgram(program);
        Grid3D.coord_loc = gl.getAttribLocation(program, 'caordinate');
        Grid3D.color_loc = gl.getAttribLocation(program, 'fColor');
        Grid3D.model_loc = gl.getUniformLocation(program, 'model');
        Grid3D.proj_loc = gl.getUniformLocation(program, 'projection');
        Grid3D.view_loc = gl.getUniformLocation(program,'view'); 
    }
    render(gl){
        gl.useProgram(Grid3D.program);
        gl.uniformMatrix4fv(Grid3D.model_loc, true, Grid3D.model_mat.content);
        gl.uniformMatrix4fv(Grid3D.proj_loc, true, gl.proj_mat.content); 
        gl.uniformMatrix4fv(Grid3D.view_loc, true, gl.cam.mat.content); 

        // gl.bindBuffer(gl.ARRAY_BUFFER, this.vBuffer);
        // gl.enableVertexAttribArray(Grid3D.color_loc);
        // gl.enableVertexAttribArray(Grid3D.coord_loc);
        gl.bindVertexArray(this.vArray);
        gl.drawArrays(gl.LINES,0,322);
        // gl.disableVertexAttribArray(Grid3D.coord_loc);
        // gl.disableVertexAttribArray(Grid3D.color_loc);
        // gl.bindBuffer(gl.ARRAY_BUFFER,null);
        gl.bindVertexArray(null);
        gl.useProgram(null);
    }
}

Grid3D.model_mat = new  Mat4f();
Grid3D.init = false;


Grid3D.vertexShader=   `attribute vec3 caordinate;
                        attribute float fColor;
                        uniform mat4 model;
                        uniform mat4 view;
                        uniform mat4 projection;
                        varying float Fcolor;
                        void main(){
                            vec4 pos = model* vec4(caordinate,1.0);
                            pos = view*pos;
                            pos = projection*pos;
                            gl_Position = pos;//vec4(coordinate,1.0);
                            Fcolor = fColor;
                        }`;
Grid3D.fragmentShader = `precision mediump float;		
                        varying float Fcolor;		
                        void main(){
                            gl_FragColor = vec4(Fcolor,Fcolor,Fcolor,1.0);
                        }`;

class GLStuffs{}
GLStuffs.getVertexBufferFor = function(vertices,gl){
    let vBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
    if(gl.getError()!==gl.NO_ERROR)
        return null;

    return vBuffer;
}
GLStuffs.getIndexBufferFor = function(indices, gl){
    let iBuffer = gl.createBuffer();
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, iBuffer);
    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, null);
    if(gl.getError()!==gl.NO_ERROR)
        return null;
    return iBuffer;
}

class ColoredSurface3D{
    constructor(setting,gl){
        if(!ColoredSurface3D.isInit)
            this.init(gl);
        
        this.vArray = gl.createVertexArray();
        gl.bindVertexArray(this.vArray);
        
        this.vBuffer = GLStuffs.getVertexBufferFor( setting.vertices,gl);
        
        gl.vertexAttribPointer(ColoredSurface3D.coord_loc, 3, gl.FLOAT, true, 24, 0);
        gl.enableVertexAttribArray(ColoredSurface3D.coord_loc);

        gl.vertexAttribPointer(ColoredSurface3D.normal_loc, 3, gl.FLOAT, true, 24, 12);
        gl.enableVertexAttribArray(ColoredSurface3D.normal_loc);

        this.iBuffer = GLStuffs.getIndexBufferFor(setting.indices, gl);
        gl.bindVertexArray(null);
        this.size = setting.indices.length;
        // console.log([this.vBuffer, this.iBuffer, this.size]);
    }
    update_vertices(vertices,gl){
        gl.deleteBuffer(this.vBuffer);
        this.vBuffer = GLStuffs.getVertexBufferFor(ColoredSurface3D.cord_loc, vertices,gl);
    }
    update_indices(indices,gl){
        gl.deleteBuffer(this.iBuffer);
        this.iBuffer = GLStuffs.getIndexBufferFor(indices,gl);
        this.size = indices.length;
    }
    render(gl,color){
        gl.useProgram(ColoredSurface3D.program);

        gl.uniformMatrix4fv(ColoredSurface3D.model_loc, true, ColoredSurface3D.model_mat.content);
        gl.uniformMatrix4fv(ColoredSurface3D.proj_loc, true, gl.proj_mat.content);
        gl.uniformMatrix4fv(ColoredSurface3D.view_loc, true, gl.cam.mat.content);
        gl.uniform4fv(ColoredSurface3D.color_loc, color);
        // let l_p = gl.cam.mat.multiplyVec4(new Vec3(gl.light_pos[0],
        //     gl.light_pos[1],gl.light_pos[2]));
        gl.uniform3fv(ColoredSurface3D.light_loc,gl.light_pos);
        gl.uniform3fv(ColoredSurface3D.light_color_loc, gl.light_color);
        // console.log(gl.cam.position.toArray());
        gl.uniform3fv(ColoredSurface3D.cam_pos, gl.cam.position.toArray());

        gl.uniform3fv(ColoredSurface3D.ambience_loc, gl.ambience);
        gl.uniform3fv(ColoredSurface3D.shininess_loc, gl.shininess);
        gl.uniform3fv(ColoredSurface3D.diffusion_loc, gl.diffusion);

        gl.uniform1f(ColoredSurface3D.glossiness_loc, gl.glossiness);
        gl.uniform1f(ColoredSurface3D.s_refl_c_loc, gl.specular_coefficient);


        gl.bindVertexArray(this.vArray);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.iBuffer);
        
        gl.drawElements(gl.TRIANGLES, this.size, gl.UNSIGNED_SHORT, 0);
        gl.bindVertexArray(null);
        gl.useProgram(null);
    }
    init(gl){
        let program = ColoredSurface3D.program = initShader(gl,ColoredSurface3D.vertexShader,ColoredSurface3D.fragmentShader);
        gl.useProgram(program);
        ColoredSurface3D.coord_loc = gl.getAttribLocation(program, 'coordinate');
        ColoredSurface3D.normal_loc = gl.getAttribLocation(program, '_normal');
        ColoredSurface3D.model_loc = gl.getUniformLocation(program, 'model');
        ColoredSurface3D.color_loc = gl.getUniformLocation(program, 'color');
        ColoredSurface3D.proj_loc = gl.getUniformLocation(program, 'projection');
        ColoredSurface3D.view_loc = gl.getUniformLocation(program,'view');

        ColoredSurface3D.light_loc       = gl.getUniformLocation(program, 'light_pos');
        ColoredSurface3D.light_color_loc = gl.getUniformLocation(program, 'light_color');
        ColoredSurface3D.ambience_loc    = gl.getUniformLocation(program, 'ambience_c');
        ColoredSurface3D.diffusion_loc   = gl.getUniformLocation(program, 'diffusion_c');
        ColoredSurface3D.shininess_loc   = gl.getUniformLocation(program, 'shininess_c');
        ColoredSurface3D.cam_pos   = gl.getUniformLocation(program, 'cam_pos');

        ColoredSurface3D.glossiness_loc   = gl.getUniformLocation(program, 'glosiness');
        ColoredSurface3D.s_refl_c_loc     = gl.getUniformLocation(program, 's_refl_c');

        // console.log({
        //     program:program,
        //     coord_loc:ColoredSurface3D.coord_loc,
        //     normal_loc:ColoredSurface3D.normal_loc,
        //     color_loc:ColoredSurface3D.color_loc,
        //     model_loc:ColoredSurface3D.model_loc,
        //     proj_loc :ColoredSurface3D.proj_loc ,
        //     view_loc :ColoredSurface3D.view_loc ,
        //     light_loc      :ColoredSurface3D.light_loc      ,  
        //     light_color_loc:ColoredSurface3D.light_color_loc,
        //     ambience_loc   :ColoredSurface3D.ambience_loc   ,
        //     diffusion_loc  :ColoredSurface3D.diffusion_loc  ,
        //     shininess_loc  :ColoredSurface3D.shininess_loc  ,
        //     glossiness_loc :ColoredSurface3D.glossiness_loc ,
        //     s_refl_c_loc   :ColoredSurface3D.s_refl_c_loc   ,
        // })
    }
    deleteData(gl){
       gl.deleteBuffer(this.vBuffer);
       gl.deleteBuffer(this.iBuffer);
       gl.deleteVertexArray(this.vArray);
    }
}

ColoredSurface3D.isInit = false;
ColoredSurface3D.model_mat = new Mat4f();

ColoredSurface3D.vertexShader =`attribute vec3 _normal;
                                attribute vec3 coordinate;
                                uniform mat4 model;
                                uniform mat4 view;
                                uniform mat4 projection;
                                uniform vec4 color;
                                uniform vec3 cam_pos;
                                uniform vec3 light_pos;
                                uniform vec3 light_color;
                                uniform vec3 ambience_c, diffusion_c, shininess_c;//Per Channel
                                uniform float glosiness, s_refl_c;
                                varying vec4 vColor;
                                void main(){
                                    vec4 pos = view*(model* vec4(coordinate,1));

                                    vec3 L = normalize(light_pos-coordinate);//Vector from pos to Light
                                    vec3 E = normalize(cam_pos-coordinate);//Vector from pos to Eye
                                    // L = vec3(view*(model*vec4(L,1)));
                                    vec3 H = normalize(L+E);

                                    vec3 N = normalize(_normal);
                                    
                                    //Cos(theta) of angle btwn normal and vector to light
                                    float Kd = max(dot(L,N),0.0);

                                    float Ks = s_refl_c*pow(max(dot(N,H),0.0),glosiness);

                                    vec3 ambience = vec3(color)*ambience_c;
                                    vec3 diffusion = vec3(color)*diffusion_c;
                                    vec3 specular = Ks*light_color*shininess_c;
                                    vColor = vec4((ambience+diffusion+specular),color.w);
                                    
                                    pos = projection*pos;
                                    gl_Position = pos;//vec4(coordinate,1.0);
                                }`;

ColoredSurface3D.fragmentShader =  `precision mediump float;
                                    varying vec4 vColor;
                                    void main(){
                                        gl_FragColor = vColor;

                                    }`;

class TexturedSurface3D{
    constructor(setting,gl){
        if(!TexturedSurface3D.isInit)
            this.init(gl);
        
        this.vArray = gl.createVertexArray();
        gl.bindVertexArray(this.vArray);
        
        this.vBuffer = GLStuffs.getVertexBufferFor(setting.vertices,gl);
        
        gl.vertexAttribPointer(TexturedSurface3D.coord_loc, 3, gl.FLOAT, true, 40, 0);
        gl.enableVertexAttribArray(TexturedSurface3D.coord_loc);

        gl.vertexAttribPointer(TexturedSurface3D.normal_loc, 3, gl.FLOAT, true, 40, 12);
        gl.enableVertexAttribArray(TexturedSurface3D.normal_loc);

	    gl.vertexAttribPointer(TexturedSurface3D.color_loc, 4, gl.FLOAT, true, 40, 24);
        gl.enableVertexAttribArray(TexturedSurface3D.color_loc);

        this.iBuffer = GLStuffs.getIndexBufferFor(setting.indices, gl);
        gl.bindVertexArray(null);
        this.size = setting.indices.length;
        console.log([this.vBuffer, this.iBuffer, this.size]);
    }
    update_vertices(vertices,gl){
        gl.deleteBuffer(this.vBuffer);
        this.vBuffer = GLStuffs.getVertexBufferFor(TexturedSurface3D.cord_loc, vertices,gl);
    }
    update_indices(indices,gl){
        gl.deleteBuffer(this.iBuffer);
        this.iBuffer = GLStuffs.getIndexBufferFor(indices,gl);
        this.size = indices.length;
    }
    render(gl){
        gl.useProgram(TexturedSurface3D.program);

        gl.uniformMatrix4fv(TexturedSurface3D.model_loc, true, TexturedSurface3D.model_mat.content);
        gl.uniformMatrix4fv(TexturedSurface3D.proj_loc, true, gl.proj_mat.content);
        gl.uniformMatrix4fv(TexturedSurface3D.view_loc, true, gl.cam.mat.content);
        gl.uniform3fv(TexturedSurface3D.light_loc, gl.light_pos);
        gl.uniform3fv(TexturedSurface3D.light_color_loc, gl.light_color);
        gl.uniform3fv(TexturedSurface3D.cam_pos, gl.cam.position.toArray());


        gl.uniform3fv(TexturedSurface3D.ambience_loc, gl.ambience);
        gl.uniform3fv(TexturedSurface3D.shininess_loc, gl.shininess);
        gl.uniform3fv(TexturedSurface3D.diffusion_loc, gl.diffusion);

        gl.uniform1f(TexturedSurface3D.glossiness_loc, gl.glossiness);
        gl.uniform1f(TexturedSurface3D.s_refl_c_loc, gl.specular_coefficient);


        gl.bindVertexArray(this.vArray);
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.iBuffer);
        
        gl.drawElements(gl.TRIANGLES, this.size, gl.UNSIGNED_SHORT, 0);
        gl.bindVertexArray(null);
        gl.useProgram(null);
    }
    init(gl){
        let program = TexturedSurface3D.program = initShader(gl,TexturedSurface3D.vertexShader,TexturedSurface3D.fragmentShader);
        gl.useProgram(program);
        TexturedSurface3D.coord_loc = gl.getAttribLocation(program, 'coordinate');
        TexturedSurface3D.color_loc = gl.getAttribLocation(program, 'color');
        TexturedSurface3D.normal_loc = gl.getAttribLocation(program, '_normal');
        TexturedSurface3D.model_loc = gl.getUniformLocation(program, 'model');
        TexturedSurface3D.proj_loc = gl.getUniformLocation(program, 'projection');
        TexturedSurface3D.view_loc = gl.getUniformLocation(program,'view');
        TexturedSurface3D.cam_pos       = gl.getUniformLocation(program, 'cam_pos');

        TexturedSurface3D.light_loc       = gl.getUniformLocation(program, 'light_pos');
        TexturedSurface3D.light_color_loc = gl.getUniformLocation(program, 'light_color');
        TexturedSurface3D.ambience_loc    = gl.getUniformLocation(program, 'ambience_c');
        TexturedSurface3D.diffusion_loc   = gl.getUniformLocation(program, 'diffusion_c');
        TexturedSurface3D.shininess_loc   = gl.getUniformLocation(program, 'shininess_c');

        TexturedSurface3D.glossiness_loc   = gl.getUniformLocation(program, 'glosiness');
        TexturedSurface3D.s_refl_c_loc     = gl.getUniformLocation(program, 's_refl_c');

        console.log({
            program:program,
            coord_loc:TexturedSurface3D.coord_loc,
            normal_loc:TexturedSurface3D.normal_loc,
            color_loc:TexturedSurface3D.color_loc,
            model_loc:TexturedSurface3D.model_loc,
            proj_loc :TexturedSurface3D.proj_loc ,
            view_loc :TexturedSurface3D.view_loc ,
            light_loc      :TexturedSurface3D.light_loc      ,  
            light_color_loc:TexturedSurface3D.light_color_loc,
            ambience_loc   :TexturedSurface3D.ambience_loc   ,
            diffusion_loc  :TexturedSurface3D.diffusion_loc  ,
            shininess_loc  :TexturedSurface3D.shininess_loc  ,
            glossiness_loc :TexturedSurface3D.glossiness_loc ,
            s_refl_c_loc   :TexturedSurface3D.s_refl_c_loc   ,
        })
    }
    deleteData(gl){
       gl.deleteBuffer(this.vBuffer);
       gl.deleteBuffer(this.iBuffer);
       gl.deleteVertexArray(this.vArray);
    }
}

TexturedSurface3D.isInit = false;
TexturedSurface3D.model_mat = new Mat4f();

TexturedSurface3D.vertexShader =`attribute vec3 _normal;
                                attribute vec3 coordinate;
				                attribute vec4 color;
                                uniform mat4 model;
                                uniform mat4 view;
                                uniform mat4 projection;
                                uniform vec3 light_pos;
                                uniform vec3 light_color;
                                uniform vec3 cam_pos;
                                uniform vec3 ambience_c, diffusion_c, shininess_c;//Per Channel
                                uniform float glosiness, s_refl_c;
                                varying vec4 vColor;
                                void main(){
                                    vec4 pos = view*(model* vec4(coordinate,1));

                                    vec3 L = normalize(light_pos-coordinate);//Vector from pos to Light
                                    vec3 E = normalize(cam_pos-coordinate);//Vector from pos to Eye
                                    // L = vec3(view*(model*vec4(L,1)));
                                    vec3 H = normalize(L+E);

                                    vec3 N = normalize(_normal);
                                    
                                    //Cos(theta) of angle btwn normal and vector to light
                                    float Kd = max(dot(L,N),0.0);

                                    float Ks = s_refl_c*pow(max(dot(N,H),0.0),glosiness);

                                    vec3 ambience = vec3(color)*ambience_c;
                                    vec3 diffusion = vec3(color)*diffusion_c;
                                    vec3 specular = Ks*light_color*shininess_c;
                                    vColor = vec4((ambience+diffusion+specular),color.w);
                                    
                                    pos = projection*pos;
                                    gl_Position = pos;//vec4(coordinate,1.0);
                                }`;

TexturedSurface3D.fragmentShader =  `precision mediump float;
                                    varying vec4 vColor;
                                    void main(){
                                        gl_FragColor = vColor;

                                    }`;
export{
    Grid3D,
    FilledShape3D,
    get_textured_cube,
    get_colored_surface,
    ColoredSurface3D,
    TexturedSurface3D,
}

// ((1+0.5*cos(u))*cos(v),(1+0.5*cos(u))*sin(v),0.5*sin(u))
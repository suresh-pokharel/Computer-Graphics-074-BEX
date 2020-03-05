import React, { Component } from 'react';
import {get_textured_cube,Grid3D, TexturedSurface3D, ColoredSurface3D} from './primitive'
import {Mat4f,Cam, Vec3} from '../math/math'
import PlotSpaceSetting from './PlotSpaceSetting'
import Symbol from '../math/symbol1'

class PlotSpace extends Component {
    w_frac = 0.75;
    status = {
        x_center: 0,
        y_center: 0,
        z_center: 0,

        x_step: 0.2,
        y_step: 0.2,
        z_step: 0.2,
        alpha: 1,
        show_axis:true,
        is_ortho:false,
        aspect:1,
        orthographic:{
            half_len:10,
        },
        prespective:{
            near:0.1,
            far:100,
            aov:50,
        }
    }

    objects = [];

    constructor(props){
        super(props);
        this.canvas = React.createRef();
        this.gl = null;
        this.clicked = false;
        this.lastX = 0;
        this.lastY = 0;
        this.lastTime = 0;
        this.a_canvas = React.createRef();
        this.set = React.createRef();
    }

    onkeydown(e){

    }

    onMouseMove(e){
        
        if(this.clicked===true){
            if((new Date()).getTime()-this.lastTime<30)
                return;

            let dx = (e.pageX-this.lastX)/this.canvas.current.width*2*Math.PI;
            let dy = (e.pageY-this.lastY)/this.canvas.current.height*2*Math.PI;

            this.gl.cam.rotateX(dy);
            this.gl.cam.rotateZ(-dx);

            this.lastX = e.pageX;
            this.lastY = e.pageY;

            this.gl.clearDepth(1.0);

            //this.updateDimensions();
            this.lastX = e.pageX;
            this.lastY = e.pageY;
            this.lastTime = (new Date()).getTime();
            this.render3D();
        }
    }

    render3D=()=>{
        this.gl.clear(this.gl.COLOR_BUFFER_BIT|this.gl.DEPTH_BUFFER_BIT);
        if(this.status.show_axis)
            this.grid.render(this.gl);
        for(let i=0;i<this.props.inputs.length;i++){
            let input = this.props.inputs[i];
            if(!input.visible)
                continue;
            if(input.type==='equation'&&input.fp!==null){
                if(input.object3D===null||input.object3D===undefined){
                    let setting = Symbol.get3DMesh(input.fp, 
                        this.props.getValueArray(this.props.inputs),
                        this.status.orthographic.half_len);
                    if(setting === null||setting === undefined)
                        continue;
                    input.object3D = new ColoredSurface3D(setting, this.gl);
                }
                input.object3D.render(this.gl,this.getColor(input.color));
            }else if(input.type==='file'){
                if(input.object!==null){
                    if(input.object3D===null||input.object3D===undefined){
                        if(input.object.textured){
                            input.object3D = new TexturedSurface3D(input.object, this.gl);    
                            console.log("Textured");
                        }else
                            input.object3D = new ColoredSurface3D(input.object, this.gl);
                    }else
                        input.object3D.render(this.gl,this.getColor(input.color));
                }
                    
            }
        }
        // console.log(this.gl.getError());
        // for(let obj of this.objects)
        //     obj.render(this.gl,[1,0,0,1]);
    }

    getColor(str){
        let m = 0;
        let r = (parseInt(str.substr(1,2),16))/255;
        let g = parseInt(str.substr(3,2),16)/255;
        let b = parseInt(str.substr(5,2),16)/255;
        return [r,g,b,this.status.alpha];
    }
    
    onMouseDown(e){
        if(this.clicked===false){
            this.clicked = true;
            this.lastX = e.pageX;
            this.lastY = e.pageY;
            this.lastTime = (new Date()).getTime();
        }
    }
    
    onMouseUp(e){
        this.clicked = false;
    }

    onMouseOut(e){
        this.clicked = false;
    }

    onWheel(e){
        if(this.status.is_ortho){
            this.status.orthographic.half_len += 0.005*e.deltaY;
            if(this.status.orthographic.half_len>40)
                 this.status.orthographic.half_len = 40;  
            else if(this.status.orthographic.half_len<0.1)
                this.status.orthographic.half_len += 0.1;
        }else{
            this.status.prespective.aov += 0.01*e.deltaY;
            if(this.status.prespective.aov>89)
                this.status.prespective.aov = 89;
            else if(this.status.prespective.aov<30)
                this.status.prespective.aov = 30;
        }
        this.setMatrix();
        this.render3D();
    }

    setMatrix=()=>{
        if(this.status.is_ortho){
            this.gl.proj_mat = Mat4f.getOrthoSymmetric(this.status.orthographic.half_len, 
                    this.status.aspect);
        }else{
            let p = this.status.prespective;
            this.gl.proj_mat = Mat4f.getPresSymmetric(p.near, p.far, 
                    this.status.aspect, p.aov);
        }
    }
    
    setCamera=()=>{
        if(this.status.is_ortho){
            this.gl.cam = new Cam(new Vec3(0,0,0), new Vec3(0,0,1), new Vec3(1,0,0)); 
        }else{
            this.gl.cam = new Cam(new Vec3(0,0,0), new Vec3(0,0,10), new Vec3(1,0,0));
        }
    }

    updateDimensions(w_frac = null){
        if(this.canvas.current===null)
            return;
        if(w_frac!==null)
            this.w_frac = w_frac;
        
        this.gl.clear(this.gl.COLOR_BUFFER_BIT|this.gl.DEPTH_BUFFER_BIT);
        this.canvas.current.height =window.innerHeight;
        this.canvas.current.width = this.w_frac*window.innerWidth;

        this.gl.viewport(0,0,this.canvas.current.width,this.canvas.current.height);

        this.status.aspect = this.canvas.current.height/this.canvas.current.width;
        this.setMatrix();
        this.render3D();
    }

    componentDidMount(){
        this.canvas.current.height =window.innerHeight;
        this.canvas.current.width = 0.75*window.innerWidth;
        this.status.aspect = this.canvas.current.height/this.canvas.current.width;
        this.gl = this.canvas.current.getContext('webgl2');
        
        this.gl.light_pos = [10,10,10];
        this.gl.light_color = [1,1,1];
        this.gl.ambience = [0.5,0.5,0.5];
        this.gl.diffusion = [0.5,0.5,0.5];
        this.gl.shininess = [0.5,0.5,0.5];
        this.gl.glossiness = 0.4;
        this.gl.specular_coefficient = 1;

        this.gl.cam = new Cam(new Vec3(0,0,0), new Vec3(0,0,10), new Vec3(1,0,0));
        
        this.gl.clearColor(1,1,1,1);
        this.gl.enable(this.gl.DEPTH_TEST);
    
        this.gl.enable(this.gl.BLEND);
        this.gl.blendFunc(this.gl.SRC_ALPHA, this.gl.ONE_MINUS_SRC_ALPHA);

        this.gl.viewport(0,0,this.canvas.current.width,this.canvas.current.height);

        this.setMatrix();
        
        // let setting = {
        //                 coords: [-100,-100,0, 
        //                           100,-100,0,
        //                           100, 100,0,
        //                          -100, 100,0
        //                         ],
        //                 color:[200,200,200,255]
        //               }

        // this.objects.push(this.get_cube_with_random_color(-0.4,-0.4,-0.4, 0.4,0.4,0.4))

        this.grid = new Grid3D(this.gl);


        this.render3D();

        window.addEventListener("resize", this.updateDimensions.bind(this,null));
        window.addEventListener("keydown", this.onKeyDown.bind(this));
    }

    get_cube_with_random_color(l,b,n, r,t,f){
        let setting =  {
            coords: [l,b,n, r,t,f],
            texture:{
                width : [1, 1, 1, 1, 1, 1, ],
                height : [1, 1, 1, 1, 1, 1, ],
                data: [	
                        [255*Math.random(),255*Math.random(),255*Math.random(),150],
                        [255*Math.random(),255*Math.random(),255*Math.random(),150],
                        [255*Math.random(),255*Math.random(),255*Math.random(),150],
                        [255*Math.random(),255*Math.random(),255*Math.random(),150],
                        [255*Math.random(),255*Math.random(),255*Math.random(),150],
                        [255*Math.random(),255*Math.random(),255*Math.random(),150],
                      ]
            },
        } 
        return get_textured_cube(this.gl,setting);
    }

    componentWillUnmount() {
        window.removeEventListener("resize", this.updateDimensions.bind(this,null));
    }

    onKeyDown(e){
        if(e.key==='w'){
            this.gl.cam.dispTarget(+0.05);
        }else if(e.key==='s'){
            this.gl.cam.dispTarget(-0.05);
        }
        this.render3D();
    }

    render() {
        return (
            <div>
                <canvas ref={this.canvas}
                onMouseDown={this.onMouseDown.bind(this)}
                onMouseUp={this.onMouseUp.bind(this)}
                onMouseMove={this.onMouseMove.bind(this)}
                onMouseOut={this.onMouseOut.bind(this)}
                onWheel={this.onWheel.bind(this)}
                onKeyDown={this.onKeyDown.bind(this)}
                ></canvas>
                <PlotSpaceSetting parent={this}></PlotSpaceSetting>
            </div>
        );
    }
}

export default PlotSpace;
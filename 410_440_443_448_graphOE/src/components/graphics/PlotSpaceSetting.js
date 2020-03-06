import React, { Component } from 'react';
import Tab from './setting_comp/Tab';
import { Mat4f, Cam,Vec3 } from '../math/math';
import MainTab from './setting_comp/MainTab';

class PlotSpaceSetting extends Component {

    changePrespective=(type, value)=>{
        let p = this.state.prespective;
        p.sliders[type].val = Number(value);
        this.setState({prespective: p});
        p = p.sliders;

        this.props.parent.gl.proj_mat = Mat4f.getPres(p.left.val,p.bottom.val,p.near.val, p.right.val,p.top.val,p.far.val);
        this.props.parent.render3D();
    }
    changePrespectiveAOV=(type, value)=>{
        let p = this.state.prespectiveaov;
        p.sliders[type].val = Number(value);
        this.setState({prespectiveaov: p});
        p = p.sliders;

        this.props.parent.gl.proj_mat = Mat4f.getPresSymmetric(p.near.val,p.far.val,p.aspect.val,p.aov.val);
        this.props.parent.render3D();
    }
    changeOrtho=(type, value)=>{
        let p = this.state.orthographic;
        p.sliders[type].val = Number(value);
        this.setState({orthographic: p});
        p = p.sliders;

        this.props.parent.gl.proj_mat = Mat4f.getOrtho(p.left.val,p.bottom.val,p.near.val, p.right.val,p.top.val,p.far.val);
        this.props.parent.render3D();
    }
    changeCamera=(type,value)=>{
        let p = this.state.camera;
        p.sliders[type].val = Number(value);
        this.setState({camera: p});
        p = p.sliders;
        let target = new Vec3(p.targetx.val,p.targety.val,p.targetz.val);
        let position = new Vec3(p.positionx.val,p.positiony.val,p.positionz.val);
        let up = new Vec3(p.upx.val,p.upy.val,p.upz.val);
        this.props.parent.gl.cam = new Cam(target,position,up);
        this.props.parent.render3D();
    }
    changeCameraAngle=(type,value)=>{
        let p = this.state.cameraAngle;
        let angle = (value-p.sliders[type].val)*Math.PI/180.0;
        p.sliders[type].val = Number(value);
        this.setState({cameraAngle: p});
        
        
        
        switch(type){
            case 'pitch':
                this.props.parent.gl.cam.pitch(angle);
            break;
            case 'roll':
                this.props.parent.gl.cam.roll(angle);
            break;
            case 'yaw':
                this.props.parent.gl.cam.yaw(angle);
            break;
        }
        
        this.props.parent.render3D();
    }
    changeLightPos=(type,value)=>{
        value = parseFloat(value);
        let p = this.state.lightPos;
        p.sliders[type].val = value;
        this.setState({lightPos:p});
        if(type==='x'){
            this.props.parent.gl.light_pos[0] = value;
        }else if(type==='y'){
            this.props.parent.gl.light_pos[1] = value;
        }else if(type==='z'){
            this.props.parent.gl.light_pos[2] = value;
        }
        this.props.parent.render3D();
    }
    changeLightColor=(type,value)=>{
        let p = this.state.lightColor;
        p.sliders[type].val = value;
        this.setState({lightColor:p});
        if(type==='R'){
            this.props.parent.gl.light_color[0] = value;
        }else if(type==='G'){
            this.props.parent.gl.light_color[1] = value;
        }else if(type==='B'){
            this.props.parent.gl.light_color[2] = value;
        }
        this.props.parent.render3D();
    }
    changeDiffusion=(type,value)=>{
        let p = this.state.diffusion;
        p.sliders[type].val = value;
        this.setState({diffusion:p});
        if(type==='R'){
            this.props.parent.gl.diffusion[0] = value;
        }else if(type==='G'){
            this.props.parent.gl.diffusion[1] = value;
        }else if(type==='B'){
            this.props.parent.gl.diffusion[2] = value;
        }
        this.props.parent.render3D();
    }
    changeShininess=(type,value)=>{
        let p = this.state.shininess;
        p.sliders[type].val = value;
        this.setState({shininess:p});
        if(type==='R'){
            this.props.parent.gl.shininess[0] = value;
        }else if(type==='G'){
            this.props.parent.gl.shininess[1] = value;
        }else if(type==='B'){
            this.props.parent.gl.shininess[2] = value;
        }
        this.props.parent.render3D();
    }
    changeConstants=(type,value)=>{
        value = parseFloat(value);
        let p = this.state.Constants;
        p.sliders[type].val = value;
        this.setState({Constants:p});
        if(type==='Glossiness'){
            this.props.parent.gl.glossiness = value;
        }else if(type==='SpecularCoefficient'){
            this.props.parent.gl.specular_coefficient = value;
        }else if(type==='Alpha'){
            this.props.parent.status.alpha = value;
        }
        this.props.parent.render3D();
    }
    
    state = {
        is_main:true,
        is_visible:false,
        current: 'cameraAngle',
        prespective:{
            name:'Prespective lbn,rtf',
            funct: this.changePrespective,
            sliders:{
                'near':{'min': 0.1, 'max':10, 'val':0.1},
                'far':{'min': 1, 'max':100, 'val':10},
                'left':{'min': -100, 'max':100, 'val':10},
                'right':{'min': -100, 'max':100, 'val':10},
                'bottom':{'min': -100, 'max':100, 'val':10},
                'top':{'min': -100, 'max':100, 'val':10},
            }
        },
        orthographic:{
            name:'Orthographic lbn,rtf',
            funct: this.changeOrtho,
            sliders:{
                'near':{'min': -100, 'max':100, 'val':-10},
                'far':{'min': -100, 'max':100, 'val':10},
                'left':{'min': -100, 'max':100, 'val':-10},
                'right':{'min': -100, 'max':100, 'val':10},
                'bottom':{'min': -100, 'max':100, 'val':-10},
                'top':{'min': -100, 'max':100, 'val':10},
            } 
        },
        orthosymmetric:{
            name:"Ortho Symmetric",
            sliders:{
                aspect:{min:0.1,max:5,val:1},
                half_len:{min:1,max:200,val:10},
            },
        },
        prespectiveaov:{
            name:'Prespective AOV',
            funct:this.changePrespectiveAOV,
            sliders:{
                aov:{min:45, max:89, val:45},
                aspect:{min:0.1,max:4, val:1},
                near:{min:0.1, max:10,val:1},
                far:{min:1,max:100,val:10}
            }
        },
        camera:{
            name:'Camera Coords',
            funct:this.changeCamera,
            sliders:{
                targetx:{min:-10,max:10,val:0},
                targety:{min:-10,max:10,val:0},
                targetz:{min:-10,max:10,val:0},
                positionx:{min:-10,max:10,val:1},
                positiony:{min:-10,max:10,val:1},
                positionz:{min:-10,max:10,val:1},
                upx:{min:-10,max:10,val:1},
                upy:{min:-10,max:10,val:1},
                upz:{min:-10,max:10,val:1},

            }
        },
        cameraAngle:{
            name:'Camera Angle',
            funct:this.changeCameraAngle,
            sliders:{
                pitch:{min:-180,max:180,val:0},
                yaw:{min:-180,max:180,val:0},
                roll:{min:-180,max:180,val:0},
            }
        },
        lightPos:{
            name:'Light Position',
            funct:this.changeLightPos,
            sliders:{
                x:{min:-20,max:20,val:10},
                y:{min:-20,max:20,val:10},
                z:{min:-20,max:20,val:10},
            }
        },
        lightColor:{
            name:'Light Color',
            funct:this.changeLightColor,
            sliders:{
                R:{min:0,max:1,val:1},
                G:{min:0,max:1,val:1},
                B:{min:0,max:1,val:1},
            }
        },
        diffusion:{
            name:'Diffusion',
            funct:this.changeDiffusion,
            sliders:{
                R:{min:0,max:1,val:0.4},
                G:{min:0,max:1,val:0.4},
                B:{min:0,max:1,val:0.4},
            } 
        },
        shininess:{
            name:'Shininess',
            funct:this.changeShininess,
            sliders:{
                R:{min:0,max:1,val:0.5},
                G:{min:0,max:1,val:0.5},
                B:{min:0,max:1,val:0.5},
            } 
        },
        Constants:{
            name:'Constants',
            funct:this.changeConstants,
            sliders:{
                Glossiness:{min:0,max:100,val:20},
                SpecularCoefficient:{min:0,max:1,val:0.4},
                Alpha:{min:0,max:1,val:1},
            }
        },
        maintabs:[  'prespective',
                    'orthographic',
                    'prespectiveaov',
                    'camera',
                    'cameraAngle', 
                    'lightPos',
                    'lightColor',
                    'diffusion',
                    'shininess',
                    'Constants',
                ],
        dum:100,
    }

    toggleInput(){
        this.setState({is_visible: !this.state.is_visible});
    }
    goBackToMain(){
        this.setState({is_main:true});
    }
    toggleViewType(){
        this.props.parent.status.is_ortho = !this.props.parent.status.is_ortho;
        this.props.parent.setCamera();
        this.props.parent.setMatrix();
        this.props.parent.render3D();
        this.setState({dum: 1000});
    }
    changeTab = (which)=>{
        this.setState({current: which});
        this.setState({is_main: false});
    }
    toggleAxis(){
        this.props.parent.status.show_axis = !this.props.parent.status.show_axis;
        this.props.parent.render3D();
        this.setState({dum: 100});
    }
    render(){
        return(
            <div className='fixed-div-20'>
            {
                this.state.is_visible?
                <div>
                    <input type='button' className='toggleBtn' value='>>' onClick={this.toggleInput.bind(this)}/>
                    <input type='button' className='rounded-right-small-button'
                            onClick={this.goBackToMain.bind(this)} value="Main"/>
                    <input type='button' className='rounded-right-small-button'
                        onClick={this.toggleAxis.bind(this)} defaultValue={ this.props.parent.status.show_axis?'Hide Axis':'Show Axis'}/>
                    <input type='button' className='rounded-right-small-button'
                        onClick={this.toggleViewType.bind(this)} defaultValue={ this.props.parent.status.is_ortho?'Prespective':'Orthographic'}/>
                    {
                        this.state.is_main?
                        <MainTab content={this.state.maintabs} changeTab={this.changeTab}/>
                        :
                        <div>
                        <Tab setting={this.state[this.state.current]} funct={this.state[this.state.current].funct} />
                        </div>
                    }
                </div>
                :<input type='button' className='toggleBtn' value='<<' 
                    onClick={this.toggleInput.bind(this)} />
            }
            </div>
        )
    }
}

export default PlotSpaceSetting;
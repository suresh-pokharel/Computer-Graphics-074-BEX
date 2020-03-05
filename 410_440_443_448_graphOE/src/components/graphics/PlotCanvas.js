import React, { Component } from 'react';
import Symbol from '../math/symbol1'
import PorpTypes from 'prop-types'

const h_frac = 0.95;
var x_step_p = 20;
var y_step_p = 20;

class PlotCanvas extends Component {
    w_frac = 0.80;
    status = {
        //Will be in real Coordinate
        is2D: true,
        x_center: 0,
        y_center: 0,
        z_center: 0,
    
        x_step: 0.4,
        y_step: 0.4,
        num:4,
        multiple:0.1,
    }
    state={};

    translate_view_by(x,y){
        this.status.x_center += x*this.status.x_step/x_step_p;
        this.status.y_center += y*this.status.y_step/y_step_p;
    }

    modifyStep(isOut){
        switch(this.status.num){
        case 1:
            if(isOut){
                this.status.num = 2;
            }else{
                this.status.num = 8;
                this.status.multiple /=10;
            }
        break;
        case 2:
            this.status.num = isOut?4:1;
        break;
        case 4:
            this.status.num = isOut?5:2;
        break;
        case 5:
            this.status.num = isOut?8:4;
        break;
        case 8:
            if(isOut){
                this.status.num = 1;
                this.status.multiple *=10;
            }else
                this.status.num = 5;
        }
    }

    constructor(props){
        super(props);
        this.canvas = React.createRef();
        this.controls = React.createRef();

        this.fix = 2;
        this.clicked = false;
        this.lastX = 0;
        this.lastY = 0;
        
        this.lastTime = new Date().getTime();
    }

    onMouseMove(e){
        if(this.clicked===true&&this.status.is2D===true){
            if((new Date()).getTime()-this.lastTime<30)
                return;
            this.translate_view_by(e.pageX-this.lastX, -e.pageY+this.lastY);
            this.updateDimensions();
            this.lastX = e.pageX;
            this.lastY = e.pageY;
            this.lastTime = (new Date()).getTime();
        }
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


    getInfo2D(){
        let height = this.canvas.current.height;
        let width = this.canvas.current.width;

        let RTPY = y_step_p/this.status.y_step;
        let RTPX = x_step_p/this.status.x_step;

        let px_center=Math.ceil(width/2 + this.status.x_center*RTPX);
        let py_center= Math.ceil(height/2 - this.status.y_center*RTPY);
        
        let top = py_center/RTPY;
        let bottom = (py_center-height)/RTPY;
        let left = -px_center/RTPX;
        let right = (width-px_center)/RTPY;

        return {

            height: height,
            width:  width,
            px_center: px_center,
            py_center: py_center,
            top: top,
            bottom: bottom,
            left:left,
            right:right,
            RTPY:RTPY,
            RTPX:RTPX,

        }
    }

    draw2DGrid(){
        
        let ctx = this.canvas.current.getContext('2d');
        let ctxI = this.getInfo2D();

        ctx.translate(-0.5,-0.5);
        ctx.lineWidth = 1.5;

        ctx.beginPath();
        
        //Y-Axis
        ctx.moveTo(ctxI.px_center,0);
        ctx.lineTo(ctxI.px_center, ctxI.height);

        //X-Axis
        ctx.moveTo(0,ctxI.py_center);
        ctx.lineTo(ctxI.width,ctxI.py_center);
        
        ctx.stroke();

        //We should always draw from top to bottom and left to right

        //Lines Parallel to X-Axis
        let start_p = ctxI.py_center%y_step_p;
        let count = 5-((ctxI.py_center-start_p)/y_step_p)%5;
        let label_p;

        if(ctxI.px_center>ctxI.width)
            label_p = ctxI.width;
        else if(ctxI.px_center<0)
            label_p = 30;
        else
            label_p = ctxI.px_center;

        ctx.lineWidth=0.2;
        while(start_p<=ctxI.height){
            if(count%5===0){
                ctx.lineWidth=0.8;
            }

            ctx.beginPath(); 
            ctx.moveTo(0, start_p);
            ctx.lineTo(ctxI.width, start_p);
            ctx.stroke();
            
            if(count%5===0){
                let num = (ctxI.py_center-start_p)/ctxI.RTPY;
                if(Math.abs(num)<1000&&Math.abs(num)>0.7||num===0){
                    let textwidth = (ctx.measureText(num.toFixed(this.fix).toString())).width+3;
                    ctx.fillText(num.toFixed(this.fix).toString(), 
                        label_p-textwidth, start_p-8);
                }else{
                    let textwidth = (ctx.measureText(num.toExponential(this.fix).toString())).width+3;
                    ctx.fillText(num.toExponential(this.fix).toString(), 
                        label_p-textwidth, start_p-8);
                }

                ctx.lineWidth=0.2;
            }
            start_p += y_step_p;
            count++;
        }

        //Lines Parallel to Y-Axis
        start_p = ctxI.px_center%x_step_p;
        count = 5-((ctxI.px_center-start_p)/x_step_p)%5;

        if(ctxI.py_center>ctxI.height)
            label_p = ctxI.height-8;
        else if(ctxI.py_center<0)
            label_p = 8;
        else
            label_p = ctxI.py_center-8;
        
        ctx.lineWidth=0.2;
        while(start_p<=ctxI.width){
            if(count%5===0){
                ctx.lineWidth=0.8;
            }

            ctx.beginPath(); 
            ctx.moveTo(start_p,0);
            ctx.lineTo(start_p, ctxI.height);
            ctx.stroke();
            
            if(count%5===0){
                if(start_p!==ctxI.px_center){
                    let num = (start_p-ctxI.px_center)/ctxI.RTPX;
                    if(Math.abs(num)<1000&&Math.abs(num)>=0.07){
                        let textwidth = (ctx.measureText(num.toFixed(this.fix).toString())).width+3;
                        ctx.fillText(num.toFixed(this.fix).toString(), 
                            start_p-textwidth,  label_p);
                    }else{
                        let textwidth = (ctx.measureText(num.toExponential(this.fix).toString())).width+3;
                        ctx.fillText(num.toExponential(this.fix).toString(), 
                            start_p-textwidth,  label_p);
                    }
                }
                ctx.lineWidth=0.2;
            }
            start_p += y_step_p;
            count++;
        }
    

        

    }

    updateDimensions(w_frac = null){
        if(w_frac!=null)
            this.w_frac = w_frac;
        if(this.canvas.current===null)
            return;
        this.canvas.current.height = h_frac*window.innerHeight;
        this.canvas.current.width = this.w_frac*window.innerWidth;
        
        this.draw2DGrid();
        this.draw();
    }

    draw(){
        let ctx = this.canvas.current.getContext('2d');
        let inf = this.getInfo2D();
        ctx.lineWidth = 3;
        for(let i=0;i<this.props.inputs.length;i++){
            let input = this.props.inputs[i];
            if(input.visible&&input.type==='equation'){
                if(input.fp!==null){
                    ctx.strokeStyle = input.color;
                    Symbol.draw2D(ctx,input.fp,inf,
                            this.props.getValueArray(input.fp.parm));
                }
            }
        }
        
    }

    componentDidMount(){
        this.canvas.current.height = h_frac*window.innerHeight;
        this.canvas.current.width = this.w_frac*window.innerWidth;
        this.draw2DGrid();

        window.addEventListener("resize", this.updateDimensions.bind(this,null));
    }

    componentWillUnmount() {
        window.removeEventListener("resize", this.updateDimensions.bind(this,null));
    }

    zoom(isOut=false){
        if((new Date()).getTime()-this.lastTime<30)
            return;
        this.lastTime = (new Date()).getTime();
        this.modifyStep(isOut);
        this.status.x_step = this.status.num*this.status.multiple;
        this.status.y_step = this.status.num*this.status.multiple;
        this.updateDimensions();
    }

    onWheel(e){
        if(e.deltaY>0){
            this.zoom(true);
        }else{
            this.zoom(false);
        }
    }
    render() {
        return (
            <div>
            
            <div style={{position:'fixed', top:'6%', right:'2%'}}>
                <input type='button' defaultValue='-' className='rounded-right-button' onClick={this.zoom.bind(this,true,2)}/>
                <input type='button' defaultValue='+' className='rounded-right-button' onClick={this.zoom.bind(this,false,2)}/>
            </div>
            <canvas ref = {this.canvas}
                onMouseDown={this.onMouseDown.bind(this)}
                onMouseUp={this.onMouseUp.bind(this)}
                onMouseMove={this.onMouseMove.bind(this)}
                onMouseOut={this.onMouseOut.bind(this)}
                onWheel={this.onWheel.bind(this)}/>
            </div>
        );
    }
}

PlotCanvas.PorpTypes={
    inputs: PorpTypes.array.isRequired,
    getValueArray: PorpTypes.func.isRequired,
}

export default PlotCanvas;
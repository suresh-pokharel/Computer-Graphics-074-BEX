import React, { Component } from 'react';
import nextId from 'react-id-generator';
import FromTo from './FromTo';
import '../style.css'

class EquationInput extends Component {
    state = {
        par:false,
    }
    constructor(props){
        super(props);
        this.text_box = React.createRef();
        this.color_picker = React.createRef();
        this.visible = React.createRef();
    }

    addSlider(name){
        this.props.addSlider(name, this.props.input.id);
    }
    removeInput(){
        this.props.removeInput(this.props.input.id);
    }
    update =(visible=null)=>{
        this.props.updateEquation(this.props.input.id,this.text_box.current.value,
                this.color_picker.current.value,visible);
    }
    showParameter(){
        this.setState({par: !this.state.par});
    }
    parametricChange=(name,min,max)=>{
        this.props.parametricChange(this.props.input.id, name,min,max);
    }
    render() {
        let slider_buttons=null;
        if(this.props.input.sliders.length!==0){
            let my_pointer = this;
            slider_buttons = this.props.input.sliders.map((name)=>{
                return <input key = {nextId()} type='button' defaultValue = {name} 
                    onClick={my_pointer.addSlider.bind(my_pointer,name)} className='rounded-right-button' />
            });
        }
        let parametric_thing = null;
        if(this.props.input.fp!==null){
            if(this.props.input.fp.type==='parametric'){
                if(this.props.is2D){
                    if(this.props.input.fp.parm.indexOf('t')!==-1)
                    parametric_thing = (<FromTo name='t' max={this.props.input.fp.max} 
                            min={this.props.input.fp.min} passChange={this.parametricChange}/>)    
                }else{
                    parametric_thing = [];
                    if(this.props.input.fp.parm.indexOf('u')!==-1)
                        parametric_thing.push(<FromTo name='u' max={this.props.input.fp.u.max} 
                        min={this.props.input.fp.u.min} passChange={this.parametricChange}/>)
                    if(this.props.input.fp.parm.indexOf('v')!==-1)
                        parametric_thing.push(<FromTo name='v' max={this.props.input.fp.v.max} 
                        min={this.props.input.fp.v.min} passChange={this.parametricChange}/>)
                }
            }else
                this.state.par = false;
        }

        return (
            <div className='input-panel-div'>
                <input type='button' defaultValue='x' className='rounded-right-small-button'
                    onClick={this.removeInput.bind(this)}/>
                <input type='color' ref={this.color_picker} className='color-picker'
                    onChange={this.update} value={this.props.input.color}/>
                {parametric_thing!==null?
                <input type='button' defaultValue={this.state.par?'Hp':'P'} className='rounded-right-small-button'
                    onClick={this.showParameter.bind(this)}/>:null}
                <input type='button' defaultValue={this.props.input.visible?'H':'S'} className='rounded-right-small-button'
                    onClick={this.update.bind(this,!this.props.input.visible)}/>
                <input type='text'  ref={this.text_box} defaultValue = {this.props.input.value} 
                    className='equation-input' onChange={this.update}/>
                {
                    this.props.input.sliders.length!==0?
                    <p className='label-small-left'>Add Slider(s)</p>
                    :null
                }
                {
                    this.state.par?
                    parametric_thing:null
                }
                {
                    this.props.input.error!==''?
                    
                    <p>{this.props.input.error}</p>
                    
                    :slider_buttons
                }
            </div>
        );
    }
}

export default EquationInput;
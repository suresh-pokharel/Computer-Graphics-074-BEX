import React, { Component } from 'react';
import SimpleSlider from './SimpleSlider';

class Tab extends Component {

    constructor(props){
        super(props);
        this.near = React.createRef();
        this.far = React.createRef();
        this.top = React.createRef();
        this.bottom = React.createRef();
        this.right = React.createRef();
        this.left = React.createRef();
    }

    handleChange(type,target){
        this.props.funct(type,target.current.value);
    }

    getForLBN(){
        return (<div>
            <label htmlFor='near' className='label-medium-left'>
             Near {(Number(this.props.setting.near)).toFixed(1)}
            </label>
            <input ref={this.near} type='range' min='0.1' max='10' step='0.1' id='near' 
                onChange={this.handleChange.bind(this,'near',this.near)}
                className='full-slider' defaultValue={this.props.setting.near}/>
            
            <label htmlFor='far' className='label-medium-left'>
             Far {(Number(this.props.setting.far)).toFixed(1)}
            </label>
            <input ref={this.far} type='range' min='10' max='20' step='0.1' id='far' 
                onChange={this.handleChange.bind(this,'far',this.far)}
                className='full-slider'defaultValue={this.props.setting.far}/>

            <label htmlFor='left' className='label-medium-left'>
             Left {(Number(this.props.setting.left)).toFixed(1)}
            </label>
            <input ref={this.left} type='range' min='-20' max='20' step='0.1' id='left' 
                onChange={this.handleChange.bind(this,'left',this.left)}
                className='full-slider' defaultValue={this.props.setting.left}/>
            
            <label htmlFor='right' className='label-medium-left'>
             Right {(Number(this.props.setting.right)).toFixed(1)}
            </label>
            <input ref={this.right} type='range' min='-20' max='20' step='0.1' id='right' 
                onChange={this.handleChange.bind(this,'right',this.right)}
                className='full-slider' defaultValue={this.props.setting.right}/>
            
            <label htmlFor='top' className='label-medium-left'>
             Top {(Number(this.props.setting.top)).toFixed(1)}
            </label>
            <input ref={this.top} type='range' min='-20' max='20' step='0.1' id='top' 
                onChange={this.handleChange.bind(this,'top',this.top)}
                className='full-slider' defaultValue={this.props.setting.top}/>
            
            <label htmlFor='bottom' className='label-medium-left'>
             Bottom {(Number(this.props.setting.bottom)).toFixed(1)}
            </label>
            <input ref={this.bottom} type='range' min='-20' max='20' step='0.1' id='bottom' 
                onChange={this.handleChange.bind(this,'bottom',this.bottom)}
                className='full-slider' defaultValue={this.props.setting.bottom}/>
        </div>);
    }

    handleSlider =(setting, value)=>{
        this.props.funct(setting.name, value);
    }

    getThings(){
        let a = [];
        for(let key in this.props.setting.sliders){
            let dict = this.props.setting.sliders[key];
            let setting = {name:key}
            setting = Object.assign({}, setting, dict);
            a.push(<SimpleSlider setting={setting} key={key} passChange={this.handleSlider}/>);
        }
        return a;
    }

    render() {
        return (
            <div>
            <fieldset className='setting-fieldset'>
            <legend>{this.props.setting.name}</legend>
                {this.getThings()}
            </fieldset>
            </div>
        );
    }
}

export default Tab;
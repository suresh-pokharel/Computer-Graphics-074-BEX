import React, { Component } from 'react';
import nextId from 'react-id-generator';

class SimpleSlider extends Component {
    state = { value: 0}
    constructor(props){
        super(props);
        this.ref = React.createRef();
    }
    handleChange=()=>{
        this.props.passChange(this.props.setting, this.ref.current.value);
    }
    render() {
        return (
            <div>
            <label className='label-medium-left'>
            {`${this.props.setting.name} ${(Number(this.props.setting.val)).toFixed(1)}`}
            </label>
            <input ref={this.ref} type='range' 
            min= {this.props.setting.min}
            max= {this.props.setting.max}
            step = '0.1' 
            onChange={this.handleChange}
            className='full-slider' 
            defaultValue={this.props.setting.val}/>
            </div>
        );
    }
}

export default SimpleSlider;
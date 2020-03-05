import React, { Component } from 'react';
import '../style.css'

class FromTo extends Component {
    constructor(props){
        super(props);
        this.max = React.createRef();
        this.min = React.createRef();
    }
    
    passChange = ()=>{
        this.props.passChange(this.props.name,this.min.current.value, this.max.current.value);
    }

    render() {
        return (
            <div>
                <input type='text' ref={this.min} className='text-input-small'
                    defaultValue={this.props.min.toString()} onChange={this.passChange}/>
                <span style={{fontSize:'15px'}}>{`<<=${this.props.name}=<<`}</span>
                <input type='text' ref={this.max} className='text-input-small'
                    defaultValue={this.props.max.toString()} onChange={this.passChange}/>
            </div>
        );
    }
}

export default FromTo;
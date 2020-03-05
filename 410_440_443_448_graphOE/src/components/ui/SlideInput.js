import React, { Component } from 'react';
import '../style.css'

class SlideInput extends Component {
    
    state = {
        show_range: false,
        min: 0,
        max: 1,
        dum:0,
    }

    constructor(props){
        super(props);
        this.slider = React.createRef();
        this.min = React.createRef();
        this.max = React.createRef();
        this.animating = false;
        this.timer = null;
        this.isIncrease = true;
        this.value = 0;
        this.state.min = this.props.input.min;
        this.state.max = this.props.input.max;
    }
    
    handleMinMax(){
        let min = parseFloat(this.min.current.value);
        let max = parseFloat(this.max.current.value);
        
        //min>max
        if(min>max){
            min = 0; max = 10;
        }

        this.setState({min:min});
        this.setState({max:max});

        //Slider Values
        if(this.slider.current.value>max)
            this.slider.current.value = max;
        else if(this.slider.current.value<min)
            this.slider.current.value = min;

        

        this.props.changeSlider(this.props.input.id, this.slider.current.value, 
            min, max,
                this.props.input.name);
    }

    handleChange(){
        this.props.changeSlider(this.props.input.id, this.slider.current.value, 
            this.state.min, this.state.max,
                this.props.input.name);
    }

    tick=()=>{
        if(this.value>this.state.max)
            this.isIncrease = false;
        else if(this.value<this.state.min)
            this.isIncrease = true;
        
        if(this.isIncrease)
            this.value += 50*parseFloat(this.slider.current.step);
        else
            this.value -= 50*parseFloat(this.slider.current.step);
        
        this.props.changeSlider(this.props.input.id, this.value, 
            this.state.min, this.state.max,
                this.props.input.name);
    }

    removeInput(){
        this.props.removeInput(this.props.input.id);
    }

    toggle_minmax(){
        this.setState({show_range:!this.state.show_range});
    }

    startAnimation(){
       if(this.animating===false){
           this.animating = true;
           this.timer = window.setInterval(this.tick,40);
       }else{
           window.clearInterval(this.timer);
           this.animating = false;
       }
       this.setState({dum:1});
    }
    componentWillUnmount(){
        if(this.animating)
            window.clearInterval(this.timer);
    }
    
    render() {
        return (
            <div className='input-panel-div'>
                <input type='button' defaultValue='x' className='rounded-right-small-button'
                    onClick={this.removeInput.bind(this)}/>
                <label className='label-medium-left'>
                    {this.props.input.name}
                </label>
                <label className='label-medium-left'>
                    {parseFloat(this.props.input.value).toExponential(1)}
                </label>
                <input type='button' defaultValue = {this.state.show_range?'^':'_'} 
                    onClick = {this.toggle_minmax.bind(this)} 
                        className='rounded-right-button'/>
                <input type='button' defaultValue = {this.animating?'Stop':'Start'} 
                    onClick = {this.startAnimation.bind(this)} 
                        className='rounded-right-button'/>
                <br/>
                <br/>

                <label className='label-small-left'>
                    {this.props.input.min.toExponential(1)}
                </label>
                <input 
                    ref = {this.slider} 
                    type='range' 
                    min= {this.props.input.min} 
                    max= {this.props.input.max} 
                    step={(this.props.input.max-this.props.input.min)/1000} 
                    defaultValue = {this.props.input.value}
                    onChange={this.handleChange.bind(this)}
                    className='slider'/>
               
               <label className='label-small-right'>
                    {this.props.input.max.toExponential(1)}
                </label>
                {
                    this.state.show_range?
                    <div>
                        <input type='text' ref={this.min} className='text-input-small'
                            defaultValue={this.state.min.toString()} onChange={this.handleMinMax.bind(this)}/>
                        <span> to </span>
                        <input type='text' ref={this.max} className='text-input-small'
                            defaultValue={this.state.max.toString()} onChange={this.handleMinMax.bind(this)}/>
                    </div>:
                    null
                }

            </div>
        );
    }
}


export default SlideInput;

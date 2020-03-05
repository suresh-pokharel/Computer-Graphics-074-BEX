import React, { Component } from 'react';
import EquationInput from './EquationInput';
import SlideInput from './SlideInput';
import PlyInput from './PlyInput';


class InputPanel extends Component {
    state = {
        
    };

    constructor(props){
        super(props);
        this.mainDiv = React.createRef();
    }

    componentDidMount(){
        
    }

    render() {
        let props = this.props;
        return props.inputs.map(function(input){
            if(input.type==='equation'){
               return <EquationInput key={input.id} 
                        input={input} updateEquation={props.updateEquation}
                        addSlider={props.addSlider} removeInput={props.removeInput}
                        is2D={props.is2D} parametricChange = {props.parametricChange}/>
            }else if(input.type==='slider'){
                return <SlideInput key={input.id} input={input} 
                                changeSlider={props.changeSlider} removeInput={props.removeInput}/>
            }else if(!props.is2D){
                return <PlyInput key={input.id} input={input} 
                            updateFile={props.updateFile} removeInput={props.removeInput}/>
            }
        });
    }
}

export default InputPanel;
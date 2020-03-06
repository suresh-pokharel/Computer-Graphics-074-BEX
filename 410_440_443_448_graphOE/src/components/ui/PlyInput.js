import React, { Component } from 'react';
import PlyLoader from '../graphics/PlyLoader'
import '../style.css';

class PlyInput extends Component {
    state = {
        loaded: false,
        show_color:false,
    }
    
    constructor(props){
        super(props);
        this.input_ref = React.createRef();
        this.color_ref = React.createRef();
    }

    componentDidMount(){
        this.input_ref.current.onclick = this.toggleLoad; 
    }

    passColor(visible=null){
        this.props.updateFile(this.props.input.id, this.color_ref.current.value, null, null,visible);
    }

    loadFile = (e)=>{
        e.preventDefault();
        let input_file = this.input_ref.current.files[0];
        if(input_file===undefined||input_file===null)
            return;
        let reader = new FileReader();
        let data;
        let thisPointer = this;
        reader.loadend = function(){}
        reader.onload = function(e){
            data = e.target.result;
            let obj = new PlyLoader(data);
            if(obj===null||obj===undefined)
                return;
            thisPointer.props.updateFile(thisPointer.props.input.id, null, obj, input_file.name);
        }
        this.setState({loaded:false});
        this.setState({show_color:true});
        reader.readAsText(input_file);
    }

    toggleLoad=()=>{
        this.setState({loaded:true});
    }

    render() {
        return (
            <form onSubmit={this.loadFile}>
                <label className='custom-input-label'>
                    {!this.state.show_color?
                        <input ref={this.input_ref} type='file' className='input-file'/>:
                        <span>
                        <input type='button' defaultValue='x' className='rounded-right-small-button'
                        onClick={this.props.removeInput.bind(this.props.input.id)}/>
                        <input ref={this.color_ref} type='color' className='color-picker'
                            onChange={this.passColor.bind(this)}/>
                         
                        <input type='button' value={this.props.input.visible?'H':'S'} 
                            className='rounded-right-button' 
                                onClick={this.passColor.bind(this,!this.props.input.visible)}/>
                        </span>
                    }
                        {this.props.input.name}
                    {this.state.loaded&&!this.state.show_color?
                    <input type='submit' value='Draw' className='rounded-right-button'/>:null}
                </label>
            </form>
        );
    }
}

export default PlyInput;
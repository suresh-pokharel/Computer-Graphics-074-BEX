import React, { Component } from 'react';
import PlotCanvas from './components/graphics/PlotCanvas';
import NavBar from './components/ui/NavBar';
import InputPanel from './components/ui/InputPanel';
import nextId from 'react-id-generator'
import './components/style.css'
import Symbol from './components/math/symbol1'
import PlotSpace from './components/graphics/PlotSpace';


const canvas_width_with_input = 0.80;
const canvas_width_without_input = 0.9999;
const def_slider_min = 0;
const def_slider_max = 1;

class App extends Component{
  constructor(props){
    super(props);
    this.input_ref = React.createRef();
    this.canvas_ref = React.createRef();
    this.u_id = nextId();
    this.lastTime = new Date().getTime();
  }

  componentDidMount(){
    document.body.addEventListener('wheel', function(e) {
      e.preventDefault();  
    }, {passive: false});
  }

  getPositionOfInput = (input_id)=>{
    for(let pos=0;pos<this.state.inputs.length;){
        let input = this.state.inputs[pos];
        if(input.id === input_id)
            return pos;
        pos++;
    }
    return -1;
  }

  changeSlider = (input_id, new_value, new_min, new_max,name)=>{
    let pos = this.getPositionOfInput(input_id);
    let newSlider = {
      id: input_id,
      type:'slider',
      min: new_min,
      max: new_max,
      name: name,
      value: parseFloat(new_value),
    };

    this.state.inputs.splice(pos,1);
    this.state.inputs.splice(pos, 0, newSlider);
    this.setState({dum: 100});
    this.canvas_ref.current.updateDimensions();
  }

  state ={
    canvas:{
      h_frac: 0.95,
      w_frac: 0.75
    },
    input_visible: true,
    rightSectionStyle: {float:'right',width:'75%'},
    inputs:[],
    dum: 0,
    is2D:true,
  }

  added_slider = [];
  getValueArray=(parm)=>{
    let val = Array(parm.length);
    for(let j=0;j<this.state.inputs.length;j++){
      let input = this.state.inputs[j];
      let i = parm.indexOf(input.name);
      if(i>=0){
        val[i] = input.value;
      }
    }
    return val;
  }

  addSlider=(name, input_id)=>{
    let newSlider = {
        id: nextId(),
        type: 'slider',
        min: def_slider_min,
        max: def_slider_max,
        name: name,
        value: 1.0,
    }

    this.added_slider.push(name);

    let pos = this.getPositionOfInput(input_id);

    //This is done so that slider lies below the equation input_id is of equation
    this.state.inputs[pos].sliders.splice(this.state.inputs[pos].sliders.indexOf(name),1);

    //This statement is required to notify that state has been changed
    this.setState({dum: 1});

    this.state.inputs.splice(pos+1,0,newSlider);
    
    //We will also need to remove sliders form other equation inputs if there are any
    for(let i=0;i<this.state.inputs.length;i++){
      if(this.state.inputs[i].type==='equation'){
        let index = this.state.inputs[i].sliders.indexOf(name)
        if(index!==-1){
          this.state.inputs[i].sliders.splice(index,1);
        }
      }
    }

    this.canvas_ref.current.updateDimensions();
  }

  addEquation = ()=>{
    let newEquation = {
      id:  nextId(),
      type:'equation',
      value:'',
      sliders: [],
      error:'',
      color: "#000000".replace(/0/g,function(){return (~~(Math.random()*16)).toString(16);}),
      fp:null,
      visible:true,
    }
    if(!this.state.is2D)
      newEquation.object3D = null;
    this.state.inputs.splice(0,0,newEquation);
    this.setState({dum:100});
  }

  getAppropriateSliders(fp){
    let sliders = [];
    for(let i=0;i<fp.parm.length;i++){
      let parm = fp.parm[i];
      if(this.added_slider.indexOf(parm)===-1)
        sliders.push(parm);
    }
    if(fp.type==='parametric'){
      if(this.state.is2D)
        sliders = sliders.filter((val)=>val!=='t');
      else{
        sliders = sliders.filter((val)=>val!=='u'&&val!=='v');
      }
    }
    else if(fp.type==='implicit'||fp.type==='explicit'){
      if(this.state.is2D)
        sliders = sliders.filter((val)=>val!=='x'&&val!=='y');
      else
        sliders = sliders.filter((val)=>val!=='x'&&val!=='y'&&val!=='z');
    }
    return sliders;
  }

  updateEquation = (input_id,text,color,visible=true)=>{
    if((new Date().getTime())-this.lastTime <100)
      return;
    else{
      this.lastTime = (new Date()).getTime();
    }

    let pos = this.getPositionOfInput(input_id);
    let newEquation = this.state.inputs[pos];
    newEquation.color = color;
    newEquation.visible = visible;
    let fp = null;
    
    if(text!==''){
      if(text!==newEquation.value){
        newEquation.value = text;
        try{
          fp = Symbol.getJSFunction(text,!this.state.is2D);
        }catch(e){}
        if(fp!==null){
          newEquation.fp = fp;
          newEquation.sliders = this.getAppropriateSliders(fp);
          newEquation.sliderscopy = Array.from(newEquation.sliders); //Copy needed for good reasons
          newEquation.error = '';
          if(!this.state.is2D){
            if(newEquation.object3D!==null&&newEquation.object3D!==undefined)
              newEquation.object3D.deleteData(this.canvas_ref.current.gl);
            newEquation.object3D = null;
          }
        }else{
          newEquation.error = 'Error';
          newEquation.fp = null;
        }
      }
    }else{
      newEquation.error = '';
      newEquation.fp = null;
    }

    this.state.inputs.splice(pos+1,0,newEquation);
    this.state.inputs.splice(pos,1);
    this.setState({dum:100});
    this.canvas_ref.current.updateDimensions();
  }

  removeInput = (input_id)=>{
    if(this.state.inputs.length===0)
      return;
    let pos;
    pos = this.getPositionOfInput(input_id);
    if(pos===-1)
      pos=0;
    if(this.state.inputs[pos].type==='slider'){
      let n = this.state.inputs[pos].name;
      this.added_slider = this.added_slider.filter((val)=>val!==n);
      
      for(let i=0;i<this.state.inputs.length;i++){
        if(this.state.inputs[i].type==='equation'){
          let index = this.state.inputs[i].sliderscopy.indexOf(n)
          if(index!==-1){
            this.state.inputs[i].sliders.push(n);
          }
        }
      }
    }else if(this.state.is3D){
      if(this.state.inputs[pos].object3D!==null||this.state.inputs[pos].object3D!==undefined)
        this.state.inputs[pos].object3D.deleteData(this.canvas_ref.current.gl);
    }


    this.state.inputs.splice(pos,1);
    this.setState({dum:100});
    this.canvas_ref.current.updateDimensions();
  }

  parametricChange = (input_id, name, min,  max)=>{
    let pos = this.getPositionOfInput(input_id);
    let newEquation = this.state.inputs[pos];
    if(this.state.is2D){
      newEquation.fp.min = parseFloat(min);
      newEquation.fp.max = parseFloat(max);
    }else{
      newEquation.fp[name].min = parseFloat(min);
      newEquation.fp[name].max = parseFloat(max);
      newEquation.object3D.deleteData(this.canvas_ref.current.gl);
      newEquation.object3D = null;
    }
    console.log("Parametric Change");
    this.state.inputs.splice(pos+1,0,newEquation);
    this.state.inputs.splice(pos,1);
    this.setState({dum:100});
    this.canvas_ref.current.updateDimensions();
  }

  toggle_input(){
    if(this.state.input_visible){
      this.setState({input_visible: false})
      this.setState({rightSectionStyle: {float:'right',width:'100%'}});
      this.canvas_ref.current.updateDimensions(canvas_width_without_input);
    }else{
      this.setState({input_visible: true})
      this.setState({rightSectionStyle: {float:'right',width:'75%'}});
      this.canvas_ref.current.updateDimensions(canvas_width_with_input);
    }
  }

  toggleDim=()=>{
    this.setState({is2D:!this.state.is2D});
    this.toggle_input(); 
  }

  updateFile=(input_id, color=null,obj=null,name=null,visible=null)=>{
    let pos = this.getPositionOfInput(input_id);
    let file = this.state.inputs[pos];
    if(name!==null)
      file.name = name;
    if(color!==null)
      file.color = color;
    if(obj!==null)
      file.object = obj;
    if(visible!==null)
      file.visible = visible;

    this.state.inputs.splice(pos+1,0,file);
    this.state.inputs.splice(pos,1);
    this.setState({dum:100});
    this.canvas_ref.current.updateDimensions();
  }

  addFile=()=>{
    let file={
      type:'file',
      id:nextId(),
      name:'Choose a file',
      object:null,
      color:"#000000".replace(/0/g,function(){return (~~(Math.random()*16)).toString(16);}),
      drawn:false,
      visible:true,
    }
    this.state.inputs.unshift(file);
    this.setState({dum:100});
  }

  render() {
    return(
      <div>
        <NavBar toggleDim = {this.toggleDim}/>
        <section style={{float: 'left'}}>
          {this.state.input_visible?
          <div className='left-section'>
            <div>
              <input type='button'  onClick={this.toggle_input.bind(this)} 
                  className='toggleBtn' defaultValue='<<'/>
              <input type='button' onClick={this.addEquation} className='rounded-right-button'
                defaultValue='+'/>
              <input type='button' onClick={this.removeInput} className='rounded-right-button'
                defaultValue='-'/>
              {!this.state.is2D?
              <input type='button' onClick={this.addFile} className='rounded-right-button'
                defaultValue='F+'/>:null}
            </div>
            <div className='input-panel'>
              <InputPanel is2D={this.state.is2D} key={this.u_id} inputs={this.state.inputs} 
                addSlider={this.addSlider} changeSlider={this.changeSlider} 
                  removeInput={this.removeInput} updateEquation={this.updateEquation}
                  parametricChange={this.parametricChange} updateFile={this.updateFile}/>
            </div>
          </div>
          :null}
          <section style={this.state.rightSectionStyle}>
          {this.state.input_visible?null:
            <input type='button'  onClick={this.toggle_input.bind(this)} 
                className='toggleBtnFixed' defaultValue='>>'/>}
            {this.state.is2D?
            <PlotCanvas ref={this.canvas_ref} inputs={this.state.inputs} 
            getValueArray={this.getValueArray}/>
            :
            <PlotSpace ref={this.canvas_ref} inputs={this.state.inputs} 
              getValueArray={this.getValueArray}/>
            }
          </section>
        </section>
      </div>
    );
  }
}

export default App;

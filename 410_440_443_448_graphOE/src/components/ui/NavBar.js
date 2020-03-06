import React, { Component } from 'react';
import Logo from '../../logo.svg'

class NavBar extends Component {
    constructor(props){
        super(props);
        this.twoD3D = React.createRef();
        this.dimState = '3D';
    }
    toggletwoD3D = ()=>{
        this.dimState = this.dimState==='3D'?'2D':'3D'; 
        this.props.toggleDim();
    }
    render() {
        return (
        <header style={headerStyle}>
            <img style={logoStyle} src = {Logo} alt="graphOE"/>
            <input ref={this.twoD3D} type='button' defaultValue={this.dimState} className='rounded-right-button' 
                onClick={this.toggletwoD3D}></input>
        </header>
        );
    }
}

const headerStyle ={
    background:'#eee',
    color: '#000',
    textAlign: 'left',
    padding: '0px',
    margin: '0px',
    height:'10%',
    width:'100%',
    fontSize:'30px',
};

const logoStyle = {
    width:'100px',
    height:'30px',
    alignment:'left',
}

export default NavBar;
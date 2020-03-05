import React, { Component } from 'react';
import nextId from 'react-id-generator';

class MainTab extends Component {
    changeTab(val){
        this.props.changeTab(val);
    }
    render(){
        return this.props.content.map((val)=>{
           return (
                <div key={nextId()}>
                    <input type='button'  value={val} className="rounded-center-button" 
                        onClick = {this.changeTab.bind(this,val)}/>
                </div>
           )
        });
    };
}

export default MainTab;
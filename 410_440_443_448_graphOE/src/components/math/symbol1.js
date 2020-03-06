import { Vec3 } from "./math";

class Symbol{
    
}

Symbol.insertMultiply= function(expression){

}

Symbol.draw2D = function(canvas, f, inf, val, threshold=0.000001){
    if(f.type==='explicit'){
        Symbol.draw2DExplicit(canvas,f,inf,val);
    }else if(f.type==='parametric'){
        Symbol.draw2DParametric(canvas,f,inf,val);
    }else if(f.type==='implicit'){
        Symbol.draw2DUsingTracing(canvas,f,inf,val);
    }
}

Symbol.draw2DExplicit = function(canvas, f, inf, val){
    if(f.expliciton==='y'){
        let x_index = f.parm.indexOf('x');

        if(x_index===-1){
            let const_y = inf.py_center-f.f(val)*inf.RTPY;
            canvas.beginPath();
            canvas.moveTo(0,const_y);
            canvas.lineTo(inf.width,const_y);
            canvas.stroke();
            return;
        }

        let step = (inf.right-inf.left)/2000;

        val[x_index] = inf.left;
        let lx = inf.px_center + val[x_index]*inf.RTPX;
        let ly = inf.py_center-f.f(val)*inf.RTPY;

        for(let i=0;i<2000;i++){
            val[x_index] = inf.left + i*step;
            // console.log(`(${lx},${ly})`);
            canvas.beginPath();
            canvas.moveTo(lx,ly);
            lx = inf.px_center+val[x_index]*inf.RTPX
            ly = inf.py_center-f.f(val)*inf.RTPY;
            canvas.lineTo(lx,ly);
            canvas.stroke();   
        }
    }else{
        let y_index = f.parm.indexOf('y');
        
        if(y_index===-1){
            let const_x = inf.px_center + f.f(val)*inf.RTPX;
            canvas.beginPath();
            canvas.moveTo(const_x,0);
            canvas.lineTo(const_x,inf.width);
            canvas.stroke();
            return;
        }
        
        let step = (inf.top-inf.bottom)/2000;

        val[y_index] = inf.bottom;
        let lx = inf.px_center + f.f(val)*inf.RTPX;
        let ly = inf.py_center-val[y_index]*inf.RTPY;

        for(let i=0;i<2000;i++){
            val[y_index] = inf.bottom + i*step;
            // console.log(`(${lx},${ly})`);
            canvas.beginPath();
            canvas.moveTo(lx,ly);
            lx = inf.px_center + f.f(val)*inf.RTPX;
            ly = inf.py_center-val[y_index]*inf.RTPY;
            canvas.lineTo(lx,ly); 
            canvas.stroke();   
        }
    }

}

Symbol.draw2DParametric = function(canvas, f, inf, val){
    //Get the values
    let val_x = Array.from(f.fx.parm.length);
    let val_y = Array.from(f.fy.parm.length);
    for(let j=0;j<f.parm.length;j++){
        let i = f.fx.parm.indexOf(f.parm[j]);
        if(i!==-1)
            val_x[i] = val[j];
        i = f.fy.parm.indexOf(f.parm[j]);
        if(i!==-1)
            val_y[i] = val[j];
    }

    let t_index_x = f.fx.parm.indexOf('t');
    let t_index_y = f.fy.parm.indexOf('t');

    //Plot only one point
    if(t_index_x===-1&&t_index_y===-1){
        let x = inf.px_center + f.fx.f(val_x)*inf.RTPX;
        let y = inf.py_center-f.fy.f(val_y)*inf.RTPY;
        canvas.beginPath();
        canvas.arc(x,y,2,0,2*Math.PI,false);
        canvas.fillStyle = canvas.strokeStyle;
        canvas.fill();
        canvas.stroke();
    }else if(t_index_x===-1&&t_index_y!==-1){
        let x = inf.px_center + f.fx.f(val_x)*inf.RTPX;
        val_y[t_index_y] = f.min;
        let y = inf.py_center-f.fy.f(val_y)*inf.RTPY;
        let step = (f.max-f.min)/100;
        for(let i=1;i<=100;i++){
            val_y[t_index_y] = f.min + i*step;
            canvas.beginPath();
            canvas.moveTo(x,y);
            y = inf.py_center-f.fy.f(val_y)*inf.RTPY;
            canvas.lineTo(x,y);
            canvas.stroke();
        }
    }else if(t_index_x!==-1&&t_index_y===-1){
        let y = inf.py_center-f.fy.f(val_y)*inf.RTPY;
        val_x[t_index_x] = f.min;
        let x = inf.px_center + f.fx.f(val_x)*inf.RTPX;
        let step = (f.max-f.min)/100;
        for(let i=1;i<=100;i++){
            val_x[t_index_x] = f.min + i*step;
            canvas.beginPath();
            canvas.moveTo(x,y);
            x = inf.px_center + f.fx.f(val_x)*inf.RTPX;
            canvas.lineTo(x,y);
            canvas.stroke();
        }
    }else{
        val_y[t_index_y] = f.min;
        let y = inf.py_center-f.fy.f(val_y)*inf.RTPY;
        val_x[t_index_x] = f.min;
        let x = inf.px_center + f.fx.f(val_x)*inf.RTPX;
        let step = (f.max-f.min)/100;
        for(let i=1;i<=100;i++){
            val_y[t_index_y]=val_x[t_index_x]=f.min + i*step;
            canvas.beginPath();
            canvas.moveTo(x,y);
            x = inf.px_center + f.fx.f(val_x)*inf.RTPX;
            y = inf.py_center-f.fy.f(val_y)*inf.RTPY;
            canvas.lineTo(x,y);
            canvas.stroke();
        } 
    }

}

//Does all the job of identifying type and giving appropriate function(s)
Symbol.getJSFunction = function(expression,is3D=false){
    if(expression.length===0)
        return null;

    expression = expression.toLowerCase();
    expression = expression.replace(/\s/ig,'');

    console.log(expression);

    //Check for Parametric
    if(expression[0]==='('){
        if(expression[expression.length-1]!==')')
            return null;
        expression = expression.substr(1,expression.length-2);
        let splitedExpressions = expression.split(',');
        let funct = {type:'parametric'};
        try{
            funct.parm = [];
            if(is3D){
                if(splitedExpressions.length!==3)
                    return null;
                let fzp = Symbol.getFunction(splitedExpressions[2]);
                funct.fz = fzp;
                funct.parm = fzp.parm.concat(funct.parm);
            }else if(splitedExpressions.length!==2){
                return null;
            }
            let fxp = Symbol.getFunction(splitedExpressions[0]);
            let fyp = Symbol.getFunction(splitedExpressions[1]);
            funct.fx = fxp;
            funct.fy = fyp;
            funct.parm = fxp.parm.concat(funct.parm);
            funct.parm = fyp.parm.concat(funct.parm);
            funct.parm = Array.from(new Set(funct.parm));
            if(is3D){
                funct.u = {min:0, max:1};
                funct.v = {min:0, max:1};
            }else{
                funct.min = 0;
                funct.max = 1;
            }
            return funct;
        }
        catch(e){
            console.log("Error at parsing");
            return null;
        }
    }
    
    let expl_checker;
    if(is3D)
        expl_checker = Symbol.isExplicitOn(expression,['x','y','z']);
    else
        expl_checker = Symbol.isExplicitOn(expression,['y','x']);

    //Check for Explicit
    if(expl_checker!==null){
        let fp = null;
        try{
            fp = Symbol.getFunction(expl_checker.expression);
        }catch(e){  
            console.log("Error");
            return null;
        }

        return {type:'explicit', expliciton:expl_checker.expliciton, f:fp.f, parm:fp.parm}
    }
    else{
        let a = expression.search('=');
        if(a===-1)
            return null;
        expression = expression.substr(0,a) + '-(' + expression.substr(a+1,expression.length-1)+')';
        console.log(expression);
        let fp = null;
        try{
            fp = Symbol.getFunction(expression);
        }catch(e){  console.log("Error");return null}
        return {type:'implicit', f: fp.f,parm:fp.parm};
    }
}

Symbol.isExplicitOn = function(expression, variables){
    for(let i=0;i<variables.length;i++){
        let what = variables[i];
        let match =  (expression.match(new RegExp(what,'g')));
        
        if(match===null||match.length>1)
            continue;

        if(expression[expression.length-1]===what&&expression[expression.length-2]==='='){
            expression = expression.substring(0,expression.length-2);
            return {expression:expression, expliciton: what};
        }
        if(expression[0]===what&&expression[1]==='='){
            expression = expression.substring(2,expression.length);
            return {expression:expression, expliciton: what};
        }
    }
    return null;
}

Symbol.getFunction = function (expression){
    let fn = [];
    let parm = [];
    let prev_alpha = false;
    let char ='';
    let func_str ='';

    for(let i=0;i<expression.length;i++){
        char = expression[i];
        if(prev_alpha===true){
            func_str += char;
            if(expression[i+1]<'a'||expression[i+1]>'z'||i+1>=expression.length){
                fn.push("Math."+func_str);
                prev_alpha = false;
            }
        }else if(char>='a'&&char<='z'){
            if(expression[i+1]<'a'||expression[i+1]>'z'||i+1>=expression.length){
                parm.push(char);
                fn.push(char);
            }else{
                prev_alpha = true;
                func_str = char;
            }
        }else
            fn.push(char);
    }
    func_str = '(____val)=>{return ';

    parm = Array.from(new Set(parm));
    parm.sort();
    
    for(let i=0;i<parm.length;i++){
        fn = fn.map((s)=>{
            if(s===parm[i])
                return '____val[' + i.toString() +']';
            return s;
        })
    }

    for(let i=0;i<fn.length;i++){
        func_str += fn[i];
    }
 
    func_str += ';}';

    let f;
    try{
        f =  eval(func_str);
        return {f: f, parm:parm};
    }catch(e){
        return null;
    }

}

Symbol.draw2DUsingSecant = function (canvas, fp, inf, val){
    
    let x_ind,y_ind;
    let step = (inf.right-inf.left)/100;
    let threshold = step/1000;

    x_ind = fp.parm.length-2;
    y_ind = fp.parm.length-1;

    val[x_ind] = inf.left;
    inf.height  /=2;
    inf.width /=2;


    let a=-1,b=1,c,fa,fb;
    let lx,ly;

    while(Math.abs(a-b)>threshold){
        val[y_ind] = a;
        fa = fp.f(val);
        val[y_ind] = b;
        fb = fp.f(val);
        c = (a*fb-b*fa)/(fb-fa);
        a = b; b = c;
        if(Number.isNaN(c))
            a=b=1;
    }

    lx = inf.px_center+val[x_ind]*inf.RTPX;
    ly = inf.py_center-c*inf.RTPY;
    val[x_ind] += step;

    while(val[x_ind]<inf.right){
        a=val[x_ind];b=a+2;
        while(Math.abs(a-b)>threshold){
            val[y_ind] = a;
            fa = fp.f(val);
            val[y_ind] = b;
            fb = fp.f(val);
            c = (a*fb-b*fa)/(fb-fa);
            a = b; b = c;
            if(Number.isNaN(c))
                a=b=1;
        }
        
        if(!Number.isNaN(c)){
            canvas.beginPath();
            canvas.moveTo(lx,ly);
            lx = inf.px_center+val[x_ind]*inf.RTPX
            ly = inf.py_center-c*inf.RTPY;
            canvas.lineTo(lx,ly);
            canvas.stroke();
        }
        val[x_ind] += step;
    }
}

Symbol.draw2DUsingAdaptiveTracing = function(canvas, fp, inf, val, threshold = 0.000001){
    
}

Symbol.draw2DUsingTracing = function(canvas, fp, inf, val, threshold = 0.00001){
    const del = (inf.right-inf.left)/(10e5);
    
    let x_ind = fp.parm.indexOf('x');
    let y_ind = fp.parm.indexOf('y');

    if(x_ind===-1||y_ind===-1)
        return;

    let step = (inf.right-inf.left)/100;
    if(step>0.1)
        step = 0.1;
    let q_step = (inf.right-inf.left)/5;

    let l = inf.left+q_step, b = inf.bottom+q_step;
    for(;l<inf.right;l+=q_step){
         b = inf.bottom;
         for(;b<inf.top;b+=q_step){
            let a = Symbol.pointAlgorithm(l,b,fp,val,del,x_ind,y_ind,threshold);
            if(a==null){
                continue;
            }
            let m = Symbol.trace(step, a.x,a.y,canvas,fp,inf,val,del,x_ind,y_ind,threshold);
            // console.log(a);
            if(m===true)
                return;
            else{
                // console.log("Found");
                //console.log(a);
                m = Symbol.trace(step, a.x,a.y,canvas,fp,inf,val,del,x_ind,y_ind,threshold,true);
            }
        }
    }
}

Symbol.trace = function(step, sx, sy, canvas, fp, inf, v, del,x_ind,y_ind,threshold,reverse=false){
    let F=0, Fx=0, Fy=0, Fac=0;
    let psx=inf.px_center+sx*inf.RTPX;
    let psy=inf.py_center-sy*inf.RTPY;
    
    let val = Array.from(v);
    let osx=sx, osy=sx;

    let slope;
    let dx=0, dy=0;
    let vx,vy,mag,angle_of_rotation = 0;

    //Assumes sx, sy already lies in the curve
    //First Step:
    
    val[x_ind] = sx + del;
    val[y_ind] = sy;
    Fx = (fp.f(val)-F)/del;

    if(Number.isNaN(Fx))
        return null;

    val[x_ind] = sx;
    val[y_ind] = sy + del;
    Fy = (fp.f(val)-F)/del;
    
    slope = -Fx/Fy;
    mag = Math.sqrt(1+slope*slope);
    
    vx = 1/mag;
    vy = slope/mag;

    

    // console.log(`ovx: ${ovx}, ovy: ${ovy}`);

    let count = 1000;
    canvas.fillRect(psx,psy,5,5);

    while(count>0){
        count--;
        if(reverse===true){
            sx -= step*vx;
            sy -= step*vy;
        }else{
            sx += step*vx;
            sy += step*vy;
        }
        let v_count = 100;
        //Point Algorithm
        while(v_count>0){
            v_count--;
            val[x_ind] = sx;
            val[y_ind] = sy;
            F = fp.f(val);

            if(Number.isNaN(F))
                return null;
            
            val[x_ind] = sx + del;
            Fx = (fp.f(val)-F)/del;

            val[x_ind] = sx;
            val[y_ind] = sy + del;
            Fy = (fp.f(val)-F)/del;

            Fac = F/(Fx*Fx+Fy*Fy);
            
            // console.log(`${sx.toFixed(3)}\t${sy.toFixed(3)}\t${F.toFixed(3)}\t${Fx.toFixed(3)}\t${Fy.toFixed(3)}\t${dx.toFixed(3)}\t${dy.toFixed(3)}\t`)

            dx = Fac*Fx;
            dy = Fac*Fy;

            sx -= dx;
            sy -= dy;

            if(Math.sqrt(dx*dx+dy*dy)<threshold)
                break;
        }
        
        // console.log({sx,sy});

        canvas.beginPath();
        canvas.moveTo(psx,psy);
        psx=inf.px_center+sx*inf.RTPX;
        psy=inf.py_center-sy*inf.RTPY;
        canvas.lineTo(psx,psy);
        canvas.stroke();
        
        slope = -Fx/Fy;

        mag = Math.sqrt(1+slope*slope);

        angle_of_rotation += Math.acos((vx+slope*vy)/mag);

        if(vx+slope*vy>0){
            vx = 1/mag;
            vy = slope/mag;
        }else{
            vx = -1/mag;
            vy = -slope/mag;
            //console.log({vx,vy});
        }

        //Check for initial
        if(angle_of_rotation>180){
            // console.log("Angle of rotation: "+angle_of_rotation.toString());
            dx = osx - sx;
            dy = osy - sy;
            //console.log(Math.sqrt(dx*dx + dy*dy));
            if(Math.sqrt(dx*dx + dy*dy)<5*step){
               // console.log('Returned due to start');
                return true;
            }
            angle_of_rotation -= 180;
        }

        //Check for the boundaries crucial for curves like hyperbola
        if(sx<inf.left||sx>inf.right){
            // console.log(`Returned due to ${sx<inf.left?"sx<left":"sx>right"}`);
            return false;
        }

        if(sy<inf.bottom||sy>inf.top){
            // console.log(`Returned due to ${sy<inf.bottom?"sy<bottom":"sy>top"}`);
            return false;
        }
    }
}

Symbol.pointAlgorithm = function(sx, sy, fp, val, del, x_ind, y_ind, threshold=0.00001){
    let F=0, Fx=0, Fy=0, Fac=0;
    let dx=0, dy=0;
    
    let count = 100;
    while(count>0){
        count--;
        val[x_ind] = sx;
        val[y_ind] = sy;
        F = fp.f(val);

        if(Number.isNaN(F))
            return null;
        
        val[x_ind] = sx + del;
        Fx = (fp.f(val)-F)/del;

        val[x_ind] = sx;
        val[y_ind] = sy + del;
        Fy = (fp.f(val)-F)/del;

        Fac = F/(Fx*Fx+Fy*Fy);
        
        // console.log(`${sx.toFixed(3)}\t${sy.toFixed(3)}\t${F.toFixed(3)}\t${Fx.toFixed(3)}\t${Fy.toFixed(3)}\t${dx.toFixed(3)}\t${dy.toFixed(3)}\t`)

        dx = Fac*Fx;
        dy = Fac*Fy;

        sx -= dx;
        sy -= dy;

        if(Math.sqrt(dx*dx+dy*dy)<threshold)
            break;
    }
    return {x:sx,y:sy};
   
}

Symbol.get3DMesh = function(fp, val, size){
    if(fp.type==='explicit'){
        return Symbol.get3DMeshForExplicit(fp, val, size);
    }else if(fp.type==='parametric'){
        return Symbol.get3DMeshForParametric(fp,val);
    }
}

Symbol.get3DMeshForExplicit = function(fp, val, size, count=50,del=10e-7){
    let step = size/count;
    let v3=0,v1,v1del;
    
    let v1_ind=0,v2_ind=0;

    let ret_arr = [];
    if(fp.expliciton==='z'){
        v1_ind = fp.parm.indexOf('x');
        v2_ind = fp.parm.indexOf('y');
        for(let i=-count;i<=count;i++){
            v1 = i*step; v1del = v1 + del;
            for(let j=-count;j<count;j++){
                val[v1_ind] = v1;
                val[v2_ind] = j*step;
                v3 = fp.f(val);
                ret_arr.push(val[v1_ind]);
                ret_arr.push(val[v2_ind]);
                ret_arr.push(v3);
                
                //df/dx
                val[v1_ind] = v1del;  
                ret_arr.push((fp.f(val) - v3)/del);

                //df/dy
                val[v1_ind] = v1;
                val[v2_ind] += del;
                ret_arr.push((fp.f(val)-v3)/del);

                //df/dz = -1
                ret_arr.push(-1);
            }
        }
    }else if(fp.expliciton==='y'){
        v1_ind = fp.parm.indexOf('x');
        v2_ind = fp.parm.indexOf('z');
        for(let i=-count;i<=count;i++){
            v1 = i*step; v1del = v1 + del;
            for(let j=-count;j<count;j++){
                val[v1_ind] = v1;
                val[v2_ind] = j*step;
                v3 = fp.f(val);
                ret_arr.push(val[v1_ind]);
                ret_arr.push(v3);
                ret_arr.push(val[v2_ind]);

                //df/dx
                val[v1_ind] = v1del;  
                ret_arr.push((fp.f(val) - v3)/del);

                //df/dy = -1
                ret_arr.push(-1);
                
                //df/dz
                val[v1_ind] = v1;
                val[v2_ind] += del;
                ret_arr.push((fp.f(val)-v3)/del);

            }
        }
    }else if(fp.expliciton==='x'){
        v1_ind = fp.parm.indexOf('y');
        v2_ind = fp.parm.indexOf('z');
        for(let i=-count;i<=count;i++){
            v1 = i*step; v1del = v1 + del;
            for(let j=-count;j<count;j++){
                val[v1_ind] = v1;
                val[v2_ind] = j*step;
                v3 = fp.f(val);
                ret_arr.push(v3);
                ret_arr.push(val[v1_ind]);
                ret_arr.push(val[v2_ind]);

                //df/dx = -1
                ret_arr.push(-1);
                
                //df/dy
                val[v1_ind] = v1del;  
                ret_arr.push((fp.f(val) - v3)/del);

                
                //df/dz
                val[v1_ind] = v1;
                val[v2_ind] += del;
                ret_arr.push((fp.f(val)-v3)/del);
            }
        }
    }else
        return null;
    return {vertices:ret_arr,indices:this.getIndicesFor(count)};
}


Symbol.getIndicesFor = function(count){
    count = 2*count;
    let indices = [];
    for(let j=0;j<count-1;j++){
        let m = j*count;
        for(let i=0;i<count;i++){
            indices.push(m+i);
            indices.push(m+i+1);
            indices.push(m+i+1+count);

            indices.push(m+i+1);
            indices.push(m+i+1+count);
            indices.push(m+i+count+2);
        }
    }
    return indices;
}

Symbol.get3DMeshForParametric = function(fp,val,count=50,del=10e-8){
    // console.log(fp);
    //One Function for every thing
    let x_u_index = fp.fx.parm.indexOf('u');
    let x_v_index = fp.fx.parm.indexOf('v');

    let y_u_index = fp.fy.parm.indexOf('u');
    let y_v_index = fp.fy.parm.indexOf('v');

    let z_u_index = fp.fz.parm.indexOf('u');
    let z_v_index = fp.fz.parm.indexOf('v');

    let u_step = (fp.u.max-fp.u.min)/count;
    let v_step = (fp.v.max-fp.v.min)/count;
    console.log({
        x_u_index:x_u_index,
        x_v_index:x_v_index,
        y_u_index:y_u_index,
        y_v_index:y_v_index,
        z_u_index:z_u_index,
        z_v_index:z_v_index,
    });
    let u = 0, v=0,u_del, v_del, c_x, c_y, c_z;
    let dr_du, dr_dv;

    //Generate value array for all functions
    let val_x = [];
    let val_y = [];
    let val_z = [];

    for(let i=0;i<fp.parm.length;i++){
        let parm = fp.parm[i];
        val_x[fp.fx.parm.indexOf(parm)] = val[i];
        val_y[fp.fy.parm.indexOf(parm)] = val[i];
        val_z[fp.fz.parm.indexOf(parm)] = val[i];
    }

    let vertices = [];
    //Generate Mesh
    for(let i=0;i<count;i++){
        u = fp.u.min + i*u_step;
        u_del = u+del; 
        val_x[x_u_index] = u;
        val_y[y_u_index] = u;
        val_z[z_u_index] = u;
        for(let j=0;j<=count;j++){
            
            v = fp.v.min + j*v_step;
            v_del = v+del;

            val_x[x_v_index] = v;
            val_y[y_v_index] = v;
            val_z[z_v_index] = v;

            //One of the most inefficient way to find gradients
            c_x = fp.fx.f(val_x);
            c_y = fp.fy.f(val_y);
            c_z = fp.fz.f(val_z);

            //Push the points
            vertices.push(c_x);
            vertices.push(c_y);
            vertices.push(c_z);

            //Here do the normals
            //dr/du
            val_x[x_u_index] = u_del;
            val_y[y_u_index] = u_del;
            val_z[z_u_index] = u_del;
            dr_du = new Vec3((fp.fx.f(val_x)-c_x)/del,(fp.fy.f(val_y)-c_x)/del,(fp.fy.f(val_y)-c_x)/del);
            
            //dr/dv
            val_x[x_u_index] = u;
            val_y[y_u_index] = u;
            val_z[z_u_index] = u;

            val_x[x_v_index] = v_del;
            val_y[y_v_index] = v_del;
            val_z[z_v_index] = v_del;

            dr_dv = new Vec3((fp.fx.f(val_x)-c_x)/del,(fp.fy.f(val_y)-c_x)/del,(fp.fy.f(val_y)-c_x)/del);
            dr_du = dr_du.cross(dr_dv);

            //Push the normals
            vertices.push(dr_du.x);
            vertices.push(dr_du.y);
            vertices.push(dr_du.z);
        }
    }
    return {vertices:vertices, indices:Symbol.getIndicesFor(count/2)};
}

// let fpa = Symbol.getJSFunction('x*x + y*y = 1');
// console.log(fpa.f([-1.25,-1.25]))
// let infa = {
//     left:-1.25,
//     right:+1.25,
//     top:+1.25,
//     bottom:-1.25,
//     px_center:0,
//     py_center:0,
//     width:100,
//     height:100,
//     RTPX:1,
//     RTPY:1,
// }
// Symbol.draw2DUsingTracing(null,fpa,infa,[0,0]);
// pow(x*x-y*y,2)- pow(x*x+y*y,4)=0

export default Symbol;
//  let f = Symbol.getJSFunction('x=y*y+z*z',true);
//  console.log(f.fz.f([2,3]));
// // console.log(f.fp.f([1,-1.25]));
// // console.log(Symbol.draw2DExplicit(null,f,infa,[1,0]));
// // console.log(Symbol.getJSFunction('(u,v,u+v)',true));
// console.log(Symbol.get3DMesh(f,[0,0],1));
// (sin(u)*cos(v),sin(u)*sin(v),cos(u))
// (0.1*cos(u),0.1*sin(u),v)
// (v*cos(u),v*sin(u),1+v)
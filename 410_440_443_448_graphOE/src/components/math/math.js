export const Vec2 = function(x,y){
	this.x = x;
	this.y = y;
	this.negate = function(){
		this.x = -this.x;
		this.y = -this.y;
	}
	this.add = function(v){
		this.x += v.x;
		this.y += v.y;
	}
	this.magnitude= () => {
		return Math.sqrt(this.x*this.x + this.y*this.y );
	}
	this.normalize = () => {
		let mag = this.magnitude();
		this.x /= mag;
		this.y /= mag;
	}
	this.scaler_multiply = (val)=>{
		this.x *= val;
		this.y *= val;
	}
}

export const Vec3 = function(x=0,y=0,z=0){
	this.x = x;
	this.y = y;
	this.z = z;
	this.negate = function(){
		this.x = -this.x;
		this.y = -this.y;
		this.z = -this.z;
	}
	this.add = function(v){
		let ret = new Vec3();
		ret.x = this.x + v.x;
		ret.y = this.y + v.y;
		ret.z = this.z + v.z;
		return ret;
	}
	this.sub = function(v){
		let ret = new Vec3();
		ret.x = this.x - v.x;
		ret.y = this.y - v.y;
		ret.z = this.z - v.z;
		return ret;
	}
	this.magnitude= () => {
		return Math.sqrt(this.x*this.x + this.y*this.y + this.z*this.z);
	}
	this.normalize = () => {
		let mag = this.magnitude();
		this.x /= mag;
		this.y /= mag;
		this.z /= mag;
	}
	this.scaler_multiply = (val)=>{
		this.x *= val;
		this.y *= val;
		this.z *= val;
	}
	this.cross = (v)=>{
		let ret = new Vec3();
		ret.x = this.y*v.z - v.y*this.z;
		ret.y = v.x*this.z - this.x*v.z;
		ret.z = this.x*v.y - v.x*this.y;
		return ret;
	}
	this.toArray = ()=>{
		return [this.x, this.y, this.z];
	}
}

export const Mat3f = function(content=null){
	if(content===null)
		this.content = new Float32Array([1.0,0.0,0.0, 
										 0.0,1.0,0.0, 
										 0.0,0.0,1.0]);
	else
		this.content = new Float32Array(content);
	this.multiply = function(other){
		let temp;
		var res = Mat3f.getNull();		
		for(var i=0;i<3;i++){
			temp = 0.0;
			for(var j=0;j<3;j++){
				for(var k=0;k<3;k++)
					temp += this.content[i*3 + k] * other.content[k*3 + j];
			}
			res.content[i*3+j] = temp;	
		}
		return res;
	}
}

Mat3f.getTranslate = function(tx, ty){
	var mat3 = Mat3f();	
	mat3.content[2] = tx;
	mat3.content[5] = ty;
	return mat3;
}

Mat3f.getRotate = function(angle){
	var cos_val = Math.cos(angle);
	var sin_val = Math.sin(angle);
	var mat3 = Mat3f();
	mat3.content[0] = cos_val; mat3.content[1] = -sin_val;
	mat3.content[3] = sin_val; mat3.content[4] = cos_val;
}

Mat3f.getNull=function(){
	return Mat3f([0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,]);
}

Mat3f.getShear = function(shx, shy){
	var mat3 = Mat3f();
	mat3.content[0] = shx;
	mat3.content[4] = shy;
	return mat3;
}

export const Mat4f = function(content=null){
	if(content===null)
		this.content = new Float32Array([1.0,0.0,0.0,0.0, 
										 0.0,1.0,0.0,0.0, 
										 0.0,0.0,1.0,0.0,
										 0.0,0.0,0.0,1.0]);
	else
		this.content = new Float32Array(content);
	this.multiply = function(other){
		let res = Mat4f.getNull();	
		for(let i=0;i<4;i++){
			for(let j=0;j<4;j++){
				let temp = 0.0;
				let p = i*4;
				for(var k=0;k<4;k++)
					temp += this.content[p + k] * other.content[k*4 + j];
				res.content[p+j] = temp;
			}
		}
		return res;
	}
	this.multiplyVec4 = function(vec){
		let res = Array(4);
		let arr = [vec.x,vec.y,vec.z,1];
		for(let i=0;i<4;i++){
			let temp = 0.0;
			let p = i*4;
			for(var k=0;k<4;k++)
				temp += this.content[p + k] * arr[k];
			res[i] = temp;
		}
		return new Vec3(res[0],res[1],res[2]);
	}

	this.printToCosole = ()=>{
		for(let i=0;i<4;i++){
			for(let j=0;j<4;j++){
				process.stdout.write(this.content[4*i+j].toFixed(3).toString()+', ');
			}
			process.stdout.write('\n');
		}
	}
}

Mat4f.getNull=function(){
	return new Mat4f([
		0.0,0.0,0.0,0.0, 
		0.0,0.0,0.0,0.0, 
		0.0,0.0,0.0,0.0,
		0.0,0.0,0.0,0.0]);
}

Mat4f.getScale=function(sx,sy,sz){
	return new Mat4f
	([
		sx,0.0,0.0,0.0, 
		0.0,sy,0.0,0.0, 
		0.0,0.0,sz,0.0,
		0.0,0.0,0.0,1.0
	]);
}

Mat4f.getTranslate = function(v){
	return new Mat4f
	([
		1.0,0.0,0.0,v.x, 
		0.0,1.0,0.0,v.y, 
		0.0,0.0,1.0,v.z,
		0.0,0.0,0.0,1.0
	]);
}

Mat4f.getRotate = function(angle, axis){
	angle /= 2;
	let a = Math.cos(angle);

	axis.normalize();
	axis.scaler_multiply(Math.sin(angle));

	let q = new Quaternion(a,axis.x,axis.y,axis.z);
	
	return q.get_mat();
}

Mat4f.getRotateX = function(angle){
	let cos = Math.cos(angle);
	let sin = Math.sin(angle);
	return new Mat4f([
		1.0,0.0, 0.0,0.0, 
		0.0,cos,-sin,0.0, 
		0.0,sin, cos,0.0,
		0.0,0.0, 0.0,1.0
	]);
}

Mat4f.getRotateY = function(angle){
	let cos = Math.cos(angle);
	let sin = Math.sin(angle);
	return new Mat4f([
		cos,0.0,-sin,0.0, 
		0.0,1.0, 0.0,0.0, 
		sin,0.0, cos,0.0,
		0.0,0.0, 0.0,1.0
	]);
}

Mat4f.getRotateZ = function(angle){
	let cos = Math.cos(angle);
	let sin = Math.sin(angle);
	return new Mat4f([
		cos,-sin,0.0,0.0, 
		sin, cos,0.0,0.0, 
		0.0, 0.0,1.0,0.0,
		0.0, 0.0,0.0,1.0
	]);
}
//width/height = aspect
Mat4f.getPresSymmetric = function(n,f, aspect, aov){
	aov *= Math.PI/180;
	let r = n*Math.tan(aov);
	let t = r*aspect;
	return Mat4f.getPres(-r,-t,n,r,t,f);
}

Mat4f.getOrtho = function(l, b, n, r, t , f){
	return new Mat4f([
		2/(r-l),       0,        0, -(r+l)/(r-l),
			  0, 2/(t-b),        0, -(t+b)/(t-b),
			  0,       0, -2/(f-n), -(f+n)/(f-n),
			  0,       0,        0,            1,
	]);
}

Mat4f.getOrthoSymmetric = function(h,aspect=1){
	let m = h*aspect;
	return Mat4f.getOrtho(-h,-m,-h, h,m,h);
}

Mat4f.getPres = function(l,b,n,r,t,f){
	return new Mat4f([
		2*n/(r-l),         0,  (r+l)/(r-l),            0,
				0, 2*n/(t-b),  (t+b)/(t-b),            0,
				0,         0, -(f+n)/(f-n), -2*f*n/(f-n),
				0,         0,          -1,             0,
	]);
}

export const Cam = class{
	constructor(target, position, up){

		this.target = target;
		this.f = position.sub(target);
		this.f.normalize();// Direction of Camera
		
		this.s = this.f.cross(up);//S-Axis of Camera Plane
		this.s.normalize();

		this.position = position;

		this.v = this.f.cross(this.s);//V-Axis of Camera Plane
		this.createMatrix();
	}

	createMatrix(){
		this.mat = new Mat4f([
			this.s.x,this.s.y,this.s.z,-this.position.x,
			this.v.x,this.v.y,this.v.z,-this.position.y,
			this.f.x,this.f.y,this.f.z,-this.position.z,
			0.0,0.0,0.0, 1.0
		]);
	}

	yaw = (angle)=>{
		//Rotate about V-axis
		let v = new Vec3(this.mat.content[4],this.mat.content[5],this.mat.content[6]);
		let rot_mat = Mat4f.getRotate(angle, v);
		this.mat = rot_mat.multiply(this.mat);
		//console.log(`${this.mat.content[8]},${this.mat.content[9]},${this.mat.content[10]}`)
	}
	pitch = (angle)=>{
		//Rotate about S-axis
		let v = new Vec3(this.mat.content[0],this.mat.content[1],this.mat.content[2]);
		let rot_mat = Mat4f.getRotate(angle, v);
		// console.log(rot_mat);
		this.mat = rot_mat.multiply(this.mat);
	}
	roll = (angle)=>{
		//Rotate about Normal to plane
		let v = new Vec3(this.mat.content[8],this.mat.content[9],this.mat.content[10]);
		let rot_mat = Mat4f.getRotate(angle, v);
		// console.log(rot_mat);
		this.mat = rot_mat.multiply(this.mat);
	}
	rotateX = (angle)=>{
		// let axis_of_rot = this.f.cross(this.v);
		// let rot_mat = Mat4f.getRotate(angle,axis_of_rot);
		// this.f = rot_mat.multiplyVec4(this.f);
		// this.s = rot_mat.multiplyVec4(this.s);
		// this.v = rot_mat.multiplyVec4(this.v);
		// this.position = rot_mat.multiplyVec4(this.position);
		// this.createMatrix()
		// console.log(this.f);
		let new_vec = new Vec3(this.s.x,this.s.y,this.s.z);
		let rot_mat = Mat4f.getRotate(angle,new_vec);
		this.position = rot_mat.multiplyVec4(this.position);
		this.mat = this.mat.multiply(rot_mat);
	}
	rotateZ = (angle)=>{
		let rot_mat = Mat4f.getRotateZ(angle);
		let rot_mats = Mat4f.getRotateZ(-angle);
		this.s = rot_mats.multiplyVec4(this.s);
		this.position = rot_mat.multiplyVec4(this.position);
		this.mat = this.mat.multiply(rot_mat);
	}
	rotateY = (angle)=>{
		let rot_mat = Mat4f.getRotateY(angle);
		this.mat = rot_mat.multiply(this.mat);
	}
	zoom = (fac=1)=>{
		let t_m = Mat4f.getScale(fac,fac,fac);
		this.mat = this.mat.multiply(t_m);
	}
	dispTarget = (val)=>{
		let f = new Vec3(this.mat.content[8],this.mat.content[9],this.mat.content[10]);
		f.scaler_multiply(val);
		// this.position = new Vec3(this.mat.content[3], this.mat.content[7], this.mat.content[11]);
		this.mat.content[3] += f.x;
		this.mat.content[7] += f.y;
		this.mat.content[11] += f.z;
	}
}

export const Quaternion = function(a, x,y,z){
	
	this.a = a;
	this.x = x;
	this.y = y;
	this.z = z;
	
	this.get_mat = ()=> {
		let qx = this.x, qy = this.y, qz = this.z, qw = this.a;
		let qx2 = qx*qx, qy2 = qy*qy, qz2 = qz*qz;
		return new Mat4f([
			1 - 2*qy2 - 2*qz2,	2*qx*qy - 2*qz*qw,	2*qx*qz + 2*qy*qw,0,
			2*qx*qy + 2*qz*qw,	1 - 2*qx2 - 2*qz2,	2*qy*qz - 2*qx*qw,0,
			2*qx*qz - 2*qy*qw,	2*qy*qz + 2*qx*qw,	1 - 2*qx2 - 2*qy2,0,
							0,					0,					0,1
		]);
	}

	this.mag = ()=>{
		return Math.sqrt(this.a*this.a+this.x*this.x+this.y*this.y+this.z*this.z);
	}

	this.normalize = () =>{
		let m = this.mag();
		this.a /=m;
		this.x /=m;
		this.y /=m;
		this.z /=m;
	}
}

class Complex{
	constructor(x=0,y=0){
		this.x = x;
		this.y = y;
	}
	add(n){
		return new Complex(n.x+this.x, n.y+this.y);
	}
	sub(n){
		return new Complex(this.x-n.x,this.y-n.y);
	}
	mul(n){
		return new Complex(this.x*n.x-this.y*n.y, this.x*n.y+this.y*n.x);
	}
	angle(){
		return Math.atan(this.y/this.x);
	}
	mag(){
		return Math.sqrt(this.x*this.x+this.y*this.y);
	}
	div(n){
		let mag = n.x*n.x + n.y*n.y;
		return new Complex((this.x*n.x+this.y*n.y)/mag, (this.y*n.x-this.x*n.y)/mag);
	}
	log(){
		return new Complex(Math.log(this.mag()),this.angle());
	}
	pow(n){
		let w = (this.log());
		if(Number.isNaN(w.y)){
			return new Complex(0,0);
		}
		w = w.mul(n);
		let r_n = Math.exp(w.x);
		return new Complex(r_n*Math.cos(w.y),r_n*Math.sin(w.y));
	}
}

// (Mat4f.getRotate(0.1,new Vec3(1,1,1))).printToCosole();
// module.exports = Vec3;
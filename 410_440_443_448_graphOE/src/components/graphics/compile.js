function initShader(gl, vertexShader, fragmentShader){
	//VertexShader
	var vertexShdr = gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShdr, vertexShader);
	gl.compileShader(vertexShdr);

	var shdr_comp_res = gl.getShaderParameter(vertexShdr, gl.COMPILE_STATUS);
	if(!shdr_comp_res){
		console.error("VertexShader Compilation Failed:\n" + gl.getShaderInfoLog(vertexShdr));
		return;
	}

	//FragmentShader
	var fragShdr = gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragShdr, fragmentShader);
	gl.compileShader(fragShdr);

	shdr_comp_res = gl.getShaderParameter(fragShdr, gl.COMPILE_STATUS);
	if(!shdr_comp_res){
		console.error("FragmentShader Compilation Failed:\n" + gl.getShaderInfoLog(fragShdr));
		return;
	}

	//Program
	var program = gl.createProgram();

	gl.attachShader(program, vertexShdr);
	gl.attachShader(program, fragShdr);

	gl.linkProgram(program);

	if(!gl.getProgramParameter(program, gl.LINK_STATUS)){
		console.error("Program Linking Failed:\n" + gl.getProgramInfoLog(program));
	}
	return program;

}

function initShaderFromId(gl, vertexShader_Id, fragmentShader_Id){
	var vertexShader = document.getElementById(vertexShader_Id).text;
	var fragmentShader = document.getElementById(fragmentShader_Id).text;

	return initShader(gl,vertexShader, fragmentShader);
}

export{initShader};
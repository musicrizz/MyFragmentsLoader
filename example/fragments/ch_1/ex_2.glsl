#version 430 core

//use this file as template

//do not modify - binding_point 1 in the cpp code
layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
	int zoom;     //  4      20 
};

//do not modify the name and type of this variables
//------------------------------------
//you can change the number of texture_img according to the images in the texture folder 
uniform sampler2D texture_img[5];

in vec2 fs_uv_coord;
//------------------------------------
out vec4 color;

float plot(in vec2 st) {    
    return 1.0 - smoothstep(0.0, 0.02, abs(st.y - st.x));
}

void main() {
	vec2 st = gl_FragCoord.xy/viewport;

    vec3 tmp = vec3(st.x);

    // Plot a line
    float pct = plot(st);
    
          //tolgo la linea
    tmp = (1.0-pct) * tmp ;
    	 //aggiungo la linea colorata di verde
    tmp = tmp + pct * vec3(0.0,1.0,0.0);

	color = vec4(tmp,1.0);
	
}
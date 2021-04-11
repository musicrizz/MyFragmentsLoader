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

vec3 colorA = vec3(0.149,0.141,0.912);
vec3 colorB = vec3(1.000,0.833,0.224);

void main(void)    {
	vec3 tmp = vec3(0.0);

	 float pct = abs(sin(time));

	    // Mix uses pct (a value from 0-1) to
	    // mix the two colors
	    tmp = mix(colorA, colorB, pct);

	    color = vec4(tmp,1.0);
	    
	
}
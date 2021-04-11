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


void main(void)    {
		
	vec2 st = gl_FragCoord.xy/viewport ;//mappatura tra 0 e 1
	
	vec4 c1 = texture(texture_img[0], fs_uv_coord*zoom);
	vec4 c2 = texture(texture_img[1], fs_uv_coord);
	vec4 c3 = texture(texture_img[2], fs_uv_coord);
	
	vec4 tmp = mix(c1, c2, 0.4);
	
	color = vec4(mix(tmp, c3, 0.6).rgb, 1.0);
	
}
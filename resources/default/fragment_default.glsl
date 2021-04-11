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
	
	color = texture(texture_img[0], fs_uv_coord);
	
}
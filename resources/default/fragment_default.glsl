#version 430 core

//use this file as template

//do not modify - binding_point 1 in the cpp code
layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 Viewport;  //  8      0   
	vec2  Mouse;     //  8      8
	int   Zoom;      //  4      16  
	float Time;      //  4      20 
	int   TimeDelta; //  4      24
};
//do not modify the name and type of this variables
//------------------------------------
//you can change the number of texture_img according to the images in the texture folder 
uniform sampler2D texture_img[5];

in vec2 uv_coord;
//------------------------------------


out vec4 fragColor;


void main(void)    {
	
	fragColor = texture(texture_img[0], uv_coord);
	
}
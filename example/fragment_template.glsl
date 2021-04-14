#version 430 core

//use this file as template

//do not modify - binding_point 1 in the cpp code
layout (std140) uniform CommonUniform
{     		                
	ivec2 viewport; //viewpor dimension same as the window 
	vec2 mouse;     //value range [(0,0) , (1,1)] ==> vec2(mouse)/vec2(viewport); //This is done by cpu
	float time;     //time, in seconds from GLWF is initialized
	int zoom;       //add by ine by mouse Scroll. min value is 0
};

//do not modify the name and type of this variables
//------------------------------------
//you can change the number of texture_img according to the images in the texture folder 
uniform sampler2D texture_img[5];

in vec2 fs_uv_coord;
//------------------------------------


out vec4 color;


void main(void)    {

	vec2 st = gl_FragCoord.xy/viewport;//mapping [0 to 1]
		
	//Your Calculations
	
	
}
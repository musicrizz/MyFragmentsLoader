#version 430 core

//use this file as template

//do not modify - binding_point 1 in the cpp code
layout (std140) uniform CommonUniform
{     		                
	ivec2 Viewport;  //viewpor dimension same as the window. (in shadertoy is ) iResolution
	
	vec2  Mouse;     //value range [(0,0) , (1,1)] ==> vec2(mouse)/vec2(viewport.x, viewport.y-h); 
					 //This is done by cpu. (0,0) is lower-left corner
	
	int   Zoom;      //add by one by mouse Scroll. min value is 0
	
	float Time;      //time, in seconds from GLWF is initialized
	int   TimeDelta; //delta Time in milliseconds , caution min is 0
};

//do not modify the name and type of this variables
//------------------------------------
//you can change the number of texture_img according to the images in the texture folder 
uniform sampler2D texture_img[5];

in vec2 uv_coord;
//------------------------------------


out vec4 fragColor;


void main(void)    {

	vec2 st = gl_FragCoord.xy/Viewport;//mapping [0 to 1]
		
	//Your Calculations
	
	
}
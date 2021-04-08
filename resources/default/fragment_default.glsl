#version 430 core

layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
};

//do not modify - sampler at 0 is NULL
layout(location = 3) uniform sampler2D texture_img[5];

out vec4 color;


void main(void)    {
	color = vec4(1.0,0.0,0.0,1.0);
}
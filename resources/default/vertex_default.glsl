#version 430 core

#pragma optimize(off)
#pragma debug(on)

//do not modify - binding_point 2
layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
};

//do not modify
layout(location = 1) in vec4 position;

//do not modify
layout(location = 3) uniform sampler2D texture1;


void main(void)    {
	gl_Position = position ;
}

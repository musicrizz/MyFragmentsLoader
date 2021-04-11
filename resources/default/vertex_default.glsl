#version 430 core

#pragma optimize(off)
#pragma debug(on)

//use this file as template

//do not modify - binding_point 1 in the cpp code
layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
	int zoom;     //  4      20  
};
//--------------
//do not modify the name and type of this variables
layout(location = 2) in vec4 position;
layout(location = 3) in vec2 uv_coord;

out vec2 fs_uv_coord;
//--------------

void main(void)    {
	fs_uv_coord = uv_coord;
	gl_Position = position;
}

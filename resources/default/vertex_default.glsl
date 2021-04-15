#version 430 core

#pragma optimize(on)
#pragma debug(off)

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
//--------------
//do not modify the name and type of this variables
layout(location = 2) in vec4 position;
layout(location = 3) in vec2 _uv_coord;

out vec2 uv_coord;
//--------------

void main(void)    {
	uv_coord = _uv_coord;
	gl_Position = position;
}

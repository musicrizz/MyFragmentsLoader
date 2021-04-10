#version 430 core


layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
};

//do not modify -
uniform sampler2D texture_img[5];

in vec2 fs_uv_coord;

out vec4 color;


void main(void)    {
	
	color = texture(texture_img[0], fs_uv_coord);
	
}
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
	int   TimeDelta; //delta Time in milliseconds
};

//do not modify the name and type of this variables
//------------------------------------
//you can change the number of texture_img according to the images in the texture folder 
uniform sampler2D texture_img[5];

in vec2 uv_coord;
//------------------------------------

#define PI 3.14159265359

out vec4 fragColor;

void main() {
	//vec2 st = gl_FragCoord.xy / viewport;
	vec2 r = Viewport;
	float t = Time;

	vec3 c;
	float l, z = t;
	for (int i = 0; i < 3; i++) {
		vec2 uv, p = gl_FragCoord.xy / r;
		uv = p;
		p -= 0.5;
		p.x *= r.x / r.y;
		z += .07;
		l = length(p);
		uv += p / l * (sin(z) + 1.) * abs(sin(l * 9. - z * 2.));
		c[i] = .01 / length(abs(mod(uv, 1.) - .5));
	}
	
	fragColor = vec4(c / l, t);

}

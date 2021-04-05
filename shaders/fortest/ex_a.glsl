#version 330 core

layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
};

out vec4 color;

vec3 colorA = vec3(0.149,0.141,0.912);
vec3 colorB = vec3(1.000,0.833,0.224);

void main(void)    {
	vec3 tmp = vec3(0.0);

	 float pct = abs(sin(time));

	    // Mix uses pct (a value from 0-1) to
	    // mix the two colors
	    tmp = mix(colorA, colorB, pct);

	    color = vec4(tmp,1.0);
	    
	   wwerwerwerwerwre 
	
}
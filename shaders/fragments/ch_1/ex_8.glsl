#version 330 core

layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
};

out vec4 color;

#define PI 3.14159265359

float plot(in vec2 st, in float pct){
  return  smoothstep( pct-0.02, pct, st.y) 
          - smoothstep( pct, pct+0.02, st.y);
}

vec3 colorA = vec3(0.149,0.141,0.912);
vec3 colorB = vec3(1.000,0.833,0.224);

void main() {
	vec2 st = gl_FragCoord.xy/viewport;

	vec3 tmp = vec3(0.0);

	vec3 pct = vec3(st.x);

	//pct.r = smoothstep(0.0,1.0, st.x);
	//pct.g = sin(st.x*PI);
	//pct.b = pow(st.x,0.5);

	tmp = mix(colorA, colorB, pct);

	// Plot transition lines for each channel
	tmp = mix(tmp,vec3(1.0,0.0,0.0),plot(st,pct.r));
	tmp = mix(tmp,vec3(0.0,1.0,0.0),plot(st,pct.g));
	tmp = mix(tmp,vec3(0.0,0.0,1.0),plot(st,pct.b));

	color = vec4(tmp,1.0);
}

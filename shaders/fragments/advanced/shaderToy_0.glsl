#version 330 core

// http://www.pouet.net/prod.php?which=57245
// If you intend to reuse this shader, please add credits to 'Danilo Guanabara'

layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
};

#define PI 3.14159265359

out vec4 color_out;

void main() {
	//vec2 st = gl_FragCoord.xy / viewport;
	vec2 r = viewport;
	float t = time;

	vec3 c;
	float l, z = t;
	for (int i = 0; i < 3; i++) {
		vec2 uv, p = gl_FragCoord.xy / r;
		uv = p;
		p -= .5;
		p.x *= r.x / r.y;
		z += .07;
		l = length(p);
		uv += p / l * (sin(z) + 1.) * abs(sin(l * 9. - z * 2.));
		c[i] = .01 / length(abs(mod(uv, 1.) - .5));
	}
	color_out = vec4(c / l, t);

}

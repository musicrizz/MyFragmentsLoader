#version 430 core

//use this file as template

//do not modify - binding_point 1 in the cpp code
layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
	int zoom;     //  4      20 
};

//do not modify the name and type of this variables
//------------------------------------
//you can change the number of texture_img according to the images in the texture folder 
uniform sampler2D texture_img[5];

in vec2 fs_uv_coord;
//------------------------------------



out vec4 color;

#define PI 3.14159265359

float plot(in vec2 st, in float pct){
  return  smoothstep( pct-0.02, pct, st.y) 
          - smoothstep( pct, pct+0.02, st.y);
}

void main() {
	vec2 st = gl_FragCoord.xy/viewport;

    // Smooth interpolation between 0.1 and 0.9
    //float y = smoothstep(0.1, 0.9,st.x);
	float y = smoothstep(0.2, 0.5, st.x) - smoothstep(0.5,0.8,st.x);

    vec3 tmp = vec3(y);

    float pct = plot(st,y);
       
    tmp = (1.0-pct) * tmp + pct * vec3(0.0,1.0,0.0);

    color = vec4(tmp,1.0);
}
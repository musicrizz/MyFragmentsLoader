#version 330 core

layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
};

out vec4 color;

float plot(in vec2 st, in float pct){
  return  smoothstep( pct-0.02, pct, st.y) -
          smoothstep( pct, pct+0.02, st.y);
}

void main() {
	vec2 st = gl_FragCoord.xy/viewport;

    // Step will return 0.0 unless the value is over 0.5,
    // in that case it will return 1.0
    float y = step(0.5,st.x);

    vec3 tmp = vec3(y);

    float pct = plot(st,y);
    
    tmp = (1.0-pct) * tmp + pct * vec3(0.0,1.0,0.0);

    color = vec4(tmp,1.0);
    
}

#version 330 core

layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
};

out vec4 color;

float plot(in vec2 st) {    
    return 1.0 - smoothstep(0.0, 0.02, abs(st.y - st.x));
}

void main() {
	vec2 st = gl_FragCoord.xy/viewport;

    vec3 tmp = vec3(st.x);

    // Plot a line
    float pct = plot(st);
    
          //tolgo la linea
    tmp = (1.0-pct) * tmp ;
    	 //aggiungo la linea colorata di verde
    tmp = tmp + pct * vec3(0.0,1.0,0.0);

	color = vec4(tmp,1.0);
	
}
#version 330 core

layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
};

out vec4 color;


void main(void)    {

	vec2 st = gl_FragCoord.xy/viewport;//mappatura tra 0 e 1
	//vec2 mm = vec2(mouse)/vec2(viewport); //trasormazione la facciamo in cpu



	//'muose pointer'
	if( (st.x > (mouse.x-0.01) && st.x < (mouse.x+0.01) )
		&&
		(st.y > (mouse.y-0.01) && st.y < (mouse.y+0.01) ) ) {
		color = vec4(1.0,0.0,0.0,1.0);
	}else{
		//color = vec4(0.0, 0.0, abs(cos(time)), 1.0);
		//color = vec4(smoothstep(0.0,1.0,st.x), 0.0, 0.0, 1.0);
		//color = vec4(smoothstep(0.0,1.0,st.y), 0.0, 0.0, 1.0);
		//color = vec4(smoothstep(0.3,0.7,st.x), 0.0, 0.0, 1.0);
		color = vec4(0.0, 0.0, smoothstep(0.0,0.8, length(st)), 1.0);
		//color = vec4(smoothstep(0.0,1.0,length(st - vec2(0.5))), 0.0, 0.0, 1.0);
	
		//simulate1(st);
	}
	
}

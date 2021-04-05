#version 330 core

layout (std140) uniform CommonUniform
{     		        //base  //Offset          
	ivec2 viewport; //  8      0   
	vec2 mouse;     //  8      8
	float time;     //  4      16  
};

out vec4 color;

#define PI 3.14159265359 
#define HALF_PI PI/2

float map0_1ToMin1_1(float x) {
	return (x - 0) * 2 - 1.0;
}
float mapMin1_1To0_1(float x) {
	return (x+1) * (0.5) ;
}

float simulateSin(in float x);
float simulateCos(in float x);
float funzione1(in float x);

float easeInElastic(float x);
float elasticInOut(float t);

float plot(in vec2 st, in float pct){
  return  smoothstep( pct-0.02, pct, st.y) 
          - smoothstep( pct, pct+0.02, st.y);
}

void main() {
	vec2 st = gl_FragCoord.xy/viewport;

	int t = int(time);
	
	float y = elasticInOut(st.x);

    vec3 tmp = vec3(y);

    float pct = plot(st,y);
       
    tmp = (1.0-pct) * tmp + pct * vec3(0.0,1.0,0.0);

    color = vec4(tmp,1.0);
//	float f = mapToMinus1To1(st.x);
//	if(f < 0)  {
//		color = vec4(0.0, 0.0, 1.0, 1.0);
//	}else{
//		color = vec4(f, 0.0, 0.0, 1.0);
//	}
	
}

float simulateSin(in float x)  {
	return sin(PI * 2 * (x+time) ) * 0.25 + 0.5; 
	//return sin(  PI * abs(smoothstep(-1.0, 1.0, x)) ) * 0.5 + 0.25 ;
}

float simulateCos(in float x)  {
	return cos(PI * x) * 0.5 + 0.5; 
}

float funzione1(in float x)   {
	float k = map0_1ToMin1_1(x);
	//float z = ( 1.0 - pow(abs(k), 0.5) ); //OK
	//float z = ( pow(cos( PI * k / 2.0), 2.5)  ); //OK
	float z = ( pow(cos( PI * k / 2.0), 0.5)  ); //OK
	
	return (mapMin1_1To0_1(z)  -0.5 ) * 2;
}

float easeInElastic(float x)  {
	float c4 = (2 * PI) / 3;

return x == 0
  ? 0
  : x == 1
  ? 1                                                   // questa è unamia riduzione per vederla ,eglio
  : -pow(2, 10 * x - 10) * sin((x * 10 - 10.75) * c4) * 0.5 + 0.25;

}

float elasticInOut(float t) {
  return (t < 0.5
    ? 0.5 * sin(+13.0 * HALF_PI * 2.0 * t) * pow(2.0, 10.0 * (2.0 * t - 1.0))
    : 0.5 * sin(-13.0 * HALF_PI * ((2.0 * t - 1.0) + 1.0)) * pow(2.0, -10.0 * (2.0 * t - 1.0)) + 1.0) * 0.4 + 0.25;
}



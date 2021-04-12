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

out vec4 fragColor;

const int MaxIterations = 100;
const vec2 Focus = vec2(-.5, .5);
const float Zoom = 1.0;
 
vec3 color(int iteration, float sqLengthZ) {
    // If the point is within the mandlebrot set 
    // just color it black
    if(iteration == MaxIterations)
        return vec3(0.0);
 
    // Else we give it a smoothed color
   float ratio = (float(iteration) - log2(log2(sqLengthZ))) / float(MaxIterations);
 
    // Procedurally generated colors
    return mix(vec3(1.0, 0.0, 0.0), vec3(1.0, 0.5, 0.5), sqrt(ratio));
}

 
void main () {      
    // C is the aspect-ratio corrected UV coordinate.
    vec2 c = (-1.0 + 2.0 * gl_FragCoord.xy / viewport.xy) * vec2(viewport.x / viewport.y, 1.0);
 
   
    //vec2 c = (gl_FragCoord.xy/viewport);
 
    // Apply scaling, then offset to get a zoom effect
    c = (c * exp(-zoom)) + Focus;
	vec2 z = c;
 
    int iteration = 0;
 
    while(iteration < MaxIterations) {
        // Precompute for efficiency
   	float zr2 = z.x * z.x;
        float zi2 = z.y * z.y;
 
        // The larger the square length of Z,
        // the smoother the shading
        if(zr2 + zi2 > 32.0) break;
 
        // Complex multiplication, then addition
    	z = vec2(zr2 - zi2, 2.0 * z.x * z.y) + c;
        ++iteration;
    }
 
    // Generate the colors
    fragColor = vec4(color(iteration, dot(z,z)), 1.0);
 
    // Apply gamma correction
    fragColor.rgb = pow(fragColor.rgb, vec3(0.5));
}

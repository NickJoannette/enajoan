#version 330 core
layout (points) in;

layout (triangle_strip, max_vertices = 3) out;
uniform float time; 

in VS_OUT {
    vec3 normal;
} gs_in[];

out GS_OUT {
	vec3 color;

} gs_out;

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

const float MAGNITUDE = 1.0;
const vec3 darkBlue = vec3(0, 0.007, 0.321);
const vec3 lightBlue = vec3(0.126, 0.5817, 0.644);
out vec4 PM;
out vec3 fColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;



mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

/*
void generatePoint(vec3 darkBlue, vec3 lightBlue)
{
	vec4 v1 = gl_in[0].gl_Position;
	vec4 v2 =   gl_in[1].gl_Position;
	vec4 v3 =   gl_in[2].gl_Position;

	vec3 axis = cross(vec3(0,0,1),gs_in[0].normal);
	

	
	mat4 randRotation = rotationMatrix(axis,time);



	vec4 baryCenter = ((1.0/3.0) *(v1+v2+v3));
	
	vec4 shiftWhatever = vec4(cross(vec3(0,1,0),gs_in[0].normal),0);
	vec4 BC2 = baryCenter + shiftWhatever;
	


for (int i = 0; i < 5; i ++) {


		// LEFT BASE
		gl_Position = (baryCenter + vec4((0.5+i)*axis,0.0));
		// Multiply into world space or whatever
		gl_Position = projection * view * gl_Position;
	
	    gs_out.color = darkBlue;
		EmitVertex();	
			
		// RIGHT BASE
		gl_Position = (baryCenter + vec4((-0.5+i)*axis,0.0));
		// Multiply into world space or whatever
		gl_Position = projection * view * gl_Position;
			
	    gs_out.color = darkBlue;

  		EmitVertex();	
		
		// THE TIP OF THE TRIANGLE (EQUAL TO CENTER VERTEX PLUS NORMAL)
		gl_Position =   (baryCenter + vec4(axis,0)*i + vec4(gs_in[0].normal,0));
		
		
		// Multiply into world space or whatever
		gl_Position = projection * view * gl_Position;
			
	    gs_out.color = lightBlue;

		EmitVertex();	

    EndPrimitive();
	
	}

for (int i = 0; i < 5; i ++)  for (int j = 0 ; j < 5 ; j ++){


		// LEFT BASE
		gl_Position = (BC2 + shiftWhatever*j + vec4((0.5+i)*axis,0.0));
		// Multiply into world space or whatever
		gl_Position = projection * view * gl_Position;
	
	    	    gs_out.color = darkBlue;

		EmitVertex();	
			
		// RIGHT BASE
		gl_Position = (BC2 + shiftWhatever*j + vec4((-0.5+i)*axis,0.0));
		// Multiply into world space or whatever
		gl_Position = projection * view * gl_Position;
			
		    gs_out.color = darkBlue;

		
		EmitVertex();	
		
		// THE TIP OF THE TRIANGLE (EQUAL TO CENTER VERTEX PLUS NORMAL)
		gl_Position =   (BC2 + shiftWhatever*j + vec4(axis,0)*i + vec4(gs_in[0].normal,0));
		
		// Multiply into world space or whatever
		gl_Position = projection * view * gl_Position;
			
	    gs_out.color = lightBlue;

		EmitVertex();	

    EndPrimitive();
	
	}

}
*/
























// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}



// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }



// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}



// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }




















uniform vec3 playerPosition;
uniform vec3 playerDirection;


void generateTriangle() {

	vec4 v1 = gl_in[0].gl_Position;


    vec4 tipOfGrassBladePos = vec4(gs_in[0].normal * (1.60+ 4.0*random(v1.xyz)),0.0);
	float distanceToPlayer = length(playerPosition - (v1.xyz + tipOfGrassBladePos.xyz));
		
		
	float bendAngle = 0;
	if (distanceToPlayer < 22.0) {
		float strength = (22.0-distanceToPlayer)/22.0;
		bendAngle = (3.14159/2.0) * strength;
	}








	vec3 axis = cross(vec3(0,0,1),gs_in[0].normal);


	mat4 randRotation = rotationMatrix(gs_in[0].normal,rand(v1.xz));
	vec3 rotatedAxis = vec3(randRotation * vec4(axis,1.0));
	


	gl_Position = v1+vec4(-2.0*random(v1)*rotatedAxis,0);
	gl_Position = projection * view * gl_Position;
	gs_out.color = darkBlue;

	EmitVertex();	
	
	gl_Position = v1+vec4(2.0*random(v1.zxz)*rotatedAxis,0);
	gl_Position = projection * view * gl_Position;
	gs_out.color = darkBlue;

	EmitVertex();	
	


	// Set up the grass bend
	vec3 windBendAxis = cross(gs_in[0].normal,-playerDirection);
	mat4 windRandBend = rotationMatrix(windBendAxis,bendAngle);

	vec3 bendAxis = cross(gs_in[0].normal,axis);
	mat4 randBend = rotationMatrix(bendAxis,random(v1.xzy));
	
	
	vec3 rotatedNormal = vec3(windRandBend  * tipOfGrassBladePos);

	gl_Position = v1 + vec4(rotatedNormal,0);
	gl_Position = projection * view * gl_Position;	
	gs_out.color = lightBlue;

	EmitVertex();	
			
    EndPrimitive();

}

void main()
{
    //generatePoint(vec3(0, 0.007, 0.521), vec3(0.156, 0.717, 0.784)); // first vertex normal
	generateTriangle();
	
}
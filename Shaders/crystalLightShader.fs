#version 330 core


//////////////// INPUTS ///////////////////////
in vec3 vertPos;
in vec3 normal;
in vec3 FragPos;
in vec2 texCoord;
///////////////////////////////////////////////


/////////////// STRUCTS ///////////////////////
struct Material {
vec3 ambient;
vec3 diffuse;
vec3 specular;
float shininess;

};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	vec2 attenuation;
	float strength;
};


/////////////////////////////////////////////////



////////////////// UNIFORMS /////////////////////

uniform vec3 viewPos;

uniform bool animatedModel;
uniform float far;
uniform float near;

uniform Material baseMaterial;
uniform PointLight pointLight;  
uniform vec3 distantBackgroundFogColor;

uniform vec3 crystalColor;
///////////////////////////////////////// OUTPUTS
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
////////////////////////////////////////////////




float LinearizeDepth(float depth, float near, float far) 	{
	float z = depth * 2.0 - 1.0;
	return (2.0*near*far) / (far + near - z * (far - near));	
}

	
void main()
{   
	float depth = LinearizeDepth(gl_FragCoord.z, near, far)/far; 	
 
	vec3 norm = normalize(normal);
    Material baseMat = baseMaterial;
	PointLight thisLight = pointLight;


	vec3 lightingResult = (vec3(127) + thisLight.diffuse) /255.0;
		
	vec3 foggedResult =  mix(lightingResult*(1.0-depth), distantBackgroundFogColor, depth);	
	
	// Gamma correction
	float gamma = 2.2;
	vec3 gammaCorrectedResult = pow( foggedResult,vec3(1.0/gamma));

	FragColor = vec4(gammaCorrectedResult, 1.0);
		
		
	// check whether fragment output is higher than threshold, if so output as brightness color
   // float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    
	// LearnOpenGL doesn't show to do it like this. I'm not sure why it wasn't working otherwise. Now, if the color of a fragment's magnitude is > 1.0,
	// it will be blurred.
	float brightness = length(FragColor.rgb);
	if(brightness > 0.20)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
		
       // BrightColor = vec4(FragColor.rgb, 1.0);



};


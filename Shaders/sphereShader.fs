#version 330 core


//////////////// INPUTS ///////////////////////
in vec3 vertPos;
in vec3 normal;
in vec3 FragPos;
in vec2 texCoord;

#define NUM_CRYSTAL_LIGHTS 8
uniform float NUM_CRYSTALS_IN_CHUNK;
///////////////////////////////////////////////


/////////////// STRUCTS ///////////////////////
struct Material {
vec3 ambient;
vec3 diffuse;
vec3 specular;
float shininess;
};


uniform Material baseMaterial;

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	vec2 attenuation;
	float strength;
};

uniform PointLight crystalLights[NUM_CRYSTAL_LIGHTS];
/////////////////////////////////////////////////



////////////////// UNIFORMS /////////////////////
uniform sampler2D diffuseMap;

uniform vec3 viewPos;
uniform vec3 distantBackgroundFogColor;
uniform float far;
uniform float near;


/////////////////////////////////////////////////


///////////////////////////////////////// OUTPUTS
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;



float LinearizeDepth(float depth, float near, float far) 	{
	float z = depth * 2.0 - 1.0;
	return (2.0*near*far) / (far + near - z * (far - near));	
}




vec3 calculateCrystalLight(PointLight crystalLight, vec3 normal, vec3 fragPos, vec3 viewDir, Material baseMat)
{


	crystalLight.ambient = (vec3(127) + crystalLight.ambient)/255.0; 
	crystalLight.diffuse = (vec3(127) + crystalLight.diffuse)/255.0; 
	crystalLight.specular = (vec3(127) + crystalLight.specular)/255.0; 
	
////////////////////////////// Ambient Lighting /////////////////////////////////////////////


	
	vec3 ambientLight = crystalLight.ambient * baseMat.ambient;
	

	////////////////////////////// Diffuse Lighting /////////////////////////////////////////////
	vec3 lightDir = normalize(crystalLight.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuseLight = crystalLight.diffuse * diff * baseMat.diffuse;
	

	////////////////////////////// Specular Lighting /////////////////////////////////////////////
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), baseMat.shininess * 128.0);
    vec3 specularLight = crystalLight.specular * spec * baseMat.specular;

	// Calculate attenuation factor based on distance from the light to this fragment	
	float fragToLightDistance = length(crystalLight.position - fragPos);
	float attenuationFactor = (1.0)/(1.0 + crystalLight.attenuation.x*fragToLightDistance + crystalLight.attenuation.y*(fragToLightDistance * fragToLightDistance));

	ambientLight *= attenuationFactor;
	diffuseLight *= attenuationFactor;
	specularLight *= attenuationFactor;

	
    return (ambientLight + diffuseLight + specularLight);
}





void main()
{   
	float depth = LinearizeDepth(gl_FragCoord.z, near, far)/far; 	

	// CREATE COPIES OF UNIFORMS
    Material baseMat = baseMaterial;
	
    vec3 viewDir = normalize(viewPos - FragPos);

	
	vec3 lightingResult = vec3(0);
	for (int i = 0; i < NUM_CRYSTALS_IN_CHUNK; i++) {
	vec3 ad = calculateCrystalLight(crystalLights[i], normal, FragPos, viewDir,baseMat);
	lightingResult += ad;
	}

	lightingResult *= vec3(texture(diffuseMap, texCoord));
	lightingResult = vec3(1.0);
	vec3 foggedResult =  mix(lightingResult*(1.0-depth), distantBackgroundFogColor, depth);	

	 	// Gamma correction
	float gamma = 2.2;
	vec3 gammaCorrectedResult = pow(foggedResult,vec3(1.0/gamma));

	FragColor = vec4(gammaCorrectedResult, 0.3);
			
		

	
	float brightness = length(FragColor.rgb);
	if(brightness > 0.2)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
		
};


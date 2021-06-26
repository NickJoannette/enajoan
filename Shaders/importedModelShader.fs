#version 330 core

///////// LIGHTING SPECIFIC UNIFORMS //////////
uniform vec3 viewPos;
#define NUM_CRYSTAL_LIGHTS 8
uniform float NUM_CRYSTALS_IN_CHUNK;

//////////////// INPUTS ///////////////////////
in vec3 normal;
in vec2 texCoord;
in vec3 FragPos;

/////////////// STRUCTS ///////////////////////
struct Material {
vec3 ambient;
vec3 diffuse;
vec3 specular;
float shininess;
};

uniform Material baseMaterial;
uniform Material extraMaterial; uniform bool useExtraMaterial;

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	vec2 attenuation;
	float strength;
};

uniform PointLight crystalLights[NUM_CRYSTAL_LIGHTS];
uniform PointLight pointLight;

///////////////////////////////////////// OUTPUTS
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
///////////////////////////////////////// READING IN TEXTURES

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;





vec3 calculateCrystalLight(PointLight crystalLight, vec3 normal, vec3 fragPos, vec3 viewDir, Material baseMat, Material extraMaterial, bool useExtraMaterial)
{

	crystalLight.ambient = (vec3(127) + crystalLight.ambient)/255.0; 
	crystalLight.diffuse = (vec3(127) + crystalLight.diffuse)/255.0; 
	crystalLight.specular = (vec3(127) + crystalLight.specular)/255.0; 

	//////////////////////////////////// LIGHTING ///////////////////////////////////////////////////
	vec3 lightDir = normalize(crystalLight.position - FragPos);

		////////////////////////////// Ambient Lighting /////////////////////////////////////////////
	vec3 ambientLight = crystalLight.ambient * baseMat.ambient * (useExtraMaterial ? extraMaterial.ambient : vec3(1.0));
	

	////////////////////////////// Diffuse Lighting /////////////////////////////////////////////
	
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuseLight = crystalLight.diffuse * diff * baseMat.diffuse* (useExtraMaterial ? extraMaterial.diffuse : vec3(1.0));
	

	////////////////////////////// Specular Lighting /////////////////////////////////////////////
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), baseMat.shininess * (useExtraMaterial ? extraMaterial.shininess : 1.0) *	128.0);
    vec3 specularLight = crystalLight.specular * spec * baseMat.specular * (useExtraMaterial ? extraMaterial.specular : vec3(1.0));
	

	// Calculate attenuation factor based on distance from the light to this fragment	
	float fragToLightDistance = length(crystalLight.position - FragPos);
	float attenuationFactor = (1.0)/(1.0 + crystalLight.attenuation.x*fragToLightDistance + crystalLight.attenuation.y*(fragToLightDistance * fragToLightDistance));

	
	//ambientLight *= attenuationFactor;
	diffuseLight *= attenuationFactor;
	
	specularLight *= attenuationFactor;
	
    return (ambientLight + diffuseLight + specularLight);
}

void main()
{   
	// CREATE COPIES OF UNIFORMS
    Material baseMat = baseMaterial;
   	PointLight mainPointLight = pointLight;
	
	
    vec3 viewDir = normalize(viewPos - FragPos);

	
	vec3 lightingResult = vec3(0);
	for (int i = 0; i < NUM_CRYSTALS_IN_CHUNK; i++) {
		vec3 ad = calculateCrystalLight(crystalLights[i], normal, FragPos, viewDir,baseMat, extraMaterial, useExtraMaterial);
		lightingResult += ad;
	}
	
	// Gamma correction
	float gamma = 2.2;
	vec3 gammaCorrectedResult = pow( lightingResult,vec3(1.0/gamma));

    FragColor = vec4(gammaCorrectedResult, 1.0);
	
	
	
	// check whether fragment output is higher than threshold, if so output as brightness color
   // float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    
	// LearnOpenGL doesn't show to do it like this. I'm not sure why it wasn't working otherwise. Now, if the color of a fragment's magnitude is > 1.0,
	// it will be blurred.
	
	float brightness = length(FragColor.rgb);
	if(brightness > 0.2)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	
};


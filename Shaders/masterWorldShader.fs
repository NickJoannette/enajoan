#version 330 core


//////////////// INPUTS ///////////////////////
in vec3 vertPos;
in vec3 normal;
in vec3 FragPos;
in vec3 PMT;
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
uniform sampler2D texture_diffuse;
uniform samplerCube shadowMap;


uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 pointLightColor;
uniform bool useDiffuse; // Decide if we're just using a pre-defined material or a sampler2D texture.
uniform bool shadowingEnabled;
uniform bool texturingEnabled;
uniform vec2 attenuation;
uniform bool animatedModel;
uniform float far;
uniform float near;

uniform bool colorAsLight;
uniform Material baseMaterial;
uniform PointLight pointLight;  

uniform bool overrideColor;
uniform vec3 manualColor;
uniform bool isMarchingCubeTerrain;
/////////////////////////////////////////////////

///////////// WATER LIGHTING UNIFORMS ///////////

uniform vec3 distantBackgroundFogColor;
uniform bool isFadeableObject;
/////////////////////////////////////////////////


////////////////POINT SHADOWS////////////////////

// array of offset direction for sampling (From LearnOpenGL)
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);
/////////////////////////////////////////////////


float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - pointLight.position;

    float currentDepth = length(fragToLight);
   
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 35;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
  
        
    return shadow;
}


///////////////////////////////////////// OUTPUTS
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
////////////////////////////////////////////////






// ------- NOISE FUNCTIONS ------- //
//	Classic Perlin 2D Noise 
	//	by Stefan Gustavson
	//
	vec2 fade(vec2 t) { return t * t*t*(t*(t*6.0f - 15.0f) + 10.0f); }

	vec4 permute(vec4 x) { return mod((((x*34.0f) + 1.0f))*x, 289.0f); }
	vec4 taylorInvSqrt(vec4 r) { return 1.79284291400159f - 0.85373472095314f * r; }
	vec3 fade(vec3 t) { return t * t*t*(t*(t*6.0f - 15.0f) + 10.0f); }

	float cnoise(vec2 P){
	  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
	  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
	  Pi = mod(Pi, 289.0); // To avoid truncation effects in permutation
	  vec4 ix = Pi.xzxz;
	  vec4 iy = Pi.yyww;
	  vec4 fx = Pf.xzxz;
	  vec4 fy = Pf.yyww;
	  vec4 i = permute(permute(ix) + iy);
	  vec4 gx = 2.0 * fract(i * 0.0243902439) - 1.0; // 1/41 = 0.024...
	  vec4 gy = abs(gx) - 0.5;
	  vec4 tx = floor(gx + 0.5);
	  gx = gx - tx;
	  vec2 g00 = vec2(gx.x,gy.x);
	  vec2 g10 = vec2(gx.y,gy.y);
	  vec2 g01 = vec2(gx.z,gy.z);
	  vec2 g11 = vec2(gx.w,gy.w);
	  vec4 norm = 1.79284291400159 - 0.85373472095314 * 
		vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11));
	  g00 *= norm.x;
	  g01 *= norm.y;
	  g10 *= norm.z;
	  g11 *= norm.w;
	  float n00 = dot(g00, vec2(fx.x, fy.x));
	  float n10 = dot(g10, vec2(fx.y, fy.y));
	  float n01 = dot(g01, vec2(fx.z, fy.z));
	  float n11 = dot(g11, vec2(fx.w, fy.w));
	  vec2 fade_xy = fade(Pf.xy);
	  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
	  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
	  return 2.3 * n_xy;
	}
	
	

#define NUM_OCTAVES 5
float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }


float noise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	// Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the cpointLights in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}
float fbm(vec2 x) {
	float v = 0.0;
	float a = 0.5;
	vec2 shift = vec2(100);
	// Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(x);
		x = rot * x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

float LinearizeDepth(float depth, float near, float far) 	{
	float z = depth * 2.0 - 1.0;
	return (2.0*near*far) / (far + near - z * (far - near));	
}

uniform bool specialColor;
	
void main()
{   

	float depth = LinearizeDepth(gl_FragCoord.z, near, far)/far; 	

	vec3 norm = normalize(normal);
    Material baseMat = baseMaterial;
	PointLight mainPointLight = pointLight;

/*
	if (isMarchingCubeTerrain) {
		vec3 noiseFactor = normalize(vec3(noise(PMT.xy), noise(PMT.xz), cnoise(PMT.xz)));
		mainPointLight.ambient +=  noiseFactor * 0.34;
		mainPointLight.diffuse += noiseFactor * 0.65;
		mainPointLight.specular += noiseFactor * 0.35;
	}
	
*/
	////////////////////////////// Ambient Lighting /////////////////////////////////////////////
	vec3 ambientLight = mainPointLight.ambient * baseMat.ambient;
	
	if (isFadeableObject) {
		float adjustedAmbientR = ambientLight.r *  (1.0-depth);
		float adjustedAmbientG = clamp(depth,0,1) * distantBackgroundFogColor.g; 
		float adjustedAmbientB = clamp(depth,0,1) * distantBackgroundFogColor.b; 
		ambientLight = vec3(adjustedAmbientR, adjustedAmbientG, adjustedAmbientB);
	}
	
	////////////////////////////// Diffuse Lighting /////////////////////////////////////////////
	
	vec3 lightDir = normalize(mainPointLight.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuseLight = mainPointLight.diffuse * 
	( specialColor ? manualColor : vec3(1.0,1.0,1.0) * (diff * baseMat.diffuse));
	

	if (isFadeableObject) {
	float adjustedDiffuseR = diffuseLight.r * (1.0-depth);
	float adjustedDiffuseG = clamp(depth,0,1) * distantBackgroundFogColor.g; 
	float adjustedDiffuseB = clamp(depth,0,1) * distantBackgroundFogColor.b; 
	diffuseLight = vec3(adjustedDiffuseR,adjustedDiffuseG,adjustedDiffuseB);
	}
////////////////////////////// Specular Lighting /////////////////////////////////////////////
      vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), baseMat.shininess * 128.0);
    vec3 specularLight = mainPointLight.specular * spec * baseMat.specular;

	
	if (isFadeableObject) {
	float adjustedSpecularR = specularLight.r * (1.0-depth);
	float adjustedSpecularG = clamp(depth,0,1) * distantBackgroundFogColor.g; 
	float adjustedSpecularB = clamp(depth,0,1) * distantBackgroundFogColor.b; 
	specularLight = vec3(adjustedSpecularR,adjustedSpecularG,adjustedSpecularB);
	}
	
	
	// Calculate attenuation factor based on distance from the light to this fragment	
	float fragToLightDistance = length(mainPointLight.position - FragPos);
	float attenuationFactor = (1.0)/(1.0 + mainPointLight.attenuation.x*fragToLightDistance + mainPointLight.attenuation.y*(fragToLightDistance * fragToLightDistance));

	// Apply attenuation
//	ambientLight *= attenuationFactor;
	diffuseLight *= attenuationFactor;
	specularLight *= attenuationFactor;
	

	// Calculate point shadow depth strength
	float shadow = ShadowCalculation(FragPos);
	
	vec3 lightingResult = (ambientLight + (1.0)*
    ( diffuseLight + specularLight ));

	lightingResult = lightingResult;
	
	// Gamma correction
	float gamma = 2.2;
	vec3 gammaCorrectedResult = pow( lightingResult,vec3(1.0/gamma));

	//if (overrideColor) gammaCorrectedResult = manualColor;
	
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


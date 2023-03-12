#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;

in vec4 fragPosLightSpace;

// spotLight
in vec4 fragPos;

uniform sampler2D shadowMap;
uniform samplerCube skybox;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;

//texture
uniform sampler2D ambientTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//fog
uniform int fogEnable;

//point
uniform int bulbEn;

//transparenta
uniform int transparent;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

struct Light{
	vec3 position;  
  
    vec3 ambientP;
    vec3 diffuseP;
    vec3 specularP;
	
    float constant;
    float linear;
    float quadratic;
};

uniform Light bulbLight[3];

struct SpotLight {    
    vec3 position;
    vec3 direction;
    
    float cutOff;
    float outerCutOff;

    vec3 ambientS;
    vec3 diffuseS;
    vec3 specularS;
};

uniform SpotLight lanternLight;


void computeLightComponents()
{		
	//in eye coordinates, the viewer is situated at the origin
	vec3 cameraPosEye = vec3(0.0f);
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

//SpotLight
vec3 computeSpotLight()
{
	float theta = dot(normalize(lanternLight.position - fragPos.xyz), normalize(-lanternLight.direction));
	float epsilon = lanternLight.cutOff - lanternLight.outerCutOff;
	float intensity = clamp((theta - lanternLight.outerCutOff) / epsilon, 0.0f, 1.0f) / 10.0;

	vec3 ambientSpot = lanternLight.ambientS;
	vec3 diffuseSpot = lanternLight.diffuseS * intensity;
	vec3 specularSpot = lanternLight.specularS * intensity;

	return ambientSpot + diffuseSpot + specularSpot;
}

//point light
vec3 computeBulbLight(Light bulbLight)
{
	float distance    = length(bulbLight.position - fragPos.xyz);
	float attenuation = 1.0 / (bulbLight.constant + bulbLight.linear * distance + 
    		    bulbLight.quadratic * (distance * distance));
	
	vec3 ambientPoint  = bulbLight.ambientP*attenuation*texture(diffuseTexture, fTexCoords).rgb;
	vec3 diffusePoint  = bulbLight.diffuseP*attenuation*texture(diffuseTexture, fTexCoords).rgb;
	vec3 specularPoint = bulbLight.specularP*attenuation*texture(specularTexture, fTexCoords).rgb; 
	
	return ambientPoint + diffusePoint + specularPoint;
}

float computeShadow()
{
	// perform perspective divide
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	// Transform to [0,1] range
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	
	// Get closest depth value from light's perspective
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	
	// Get depth of current fragment from light's perspective
	float currentDepth = normalizedCoords.z;
	
	//float bias = max(0.05 * (1.0 - dot(fNormal, lightDir)), 0.005); 
	float bias=0.00045;
	float shadow=0.0f;
	
	vec2 texelSize=1.0/textureSize(shadowMap,0);

	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, normalizedCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	
	// Check whether current frag pos is in shadow
	//shadow = (currentDepth - bias) > closestDepth ? 1.0 : 0.0;
	
	if(normalizedCoords.z>1.0f)
		shadow = 0.0f;
	
	return shadow;
}

float computeFog()
{
 float fogDensity = 0.04f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	computeLightComponents();
	
	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(ambientTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	float shadow = computeShadow();
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
	
	if(bulbEn==1)
		color+=computeBulbLight(bulbLight[0]);
	
	color+=computeBulbLight(bulbLight[1]);
	color+=computeBulbLight(bulbLight[2]);
	
	color+=computeSpotLight();
	
	vec4 color4 = vec4(color.x, color.y, color.z, 0.4f);
	
    float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	
	vec4 fColor2=vec4(color,0.4f);
	
	if(fogEnable==1)
	{
		fColor2 = mix(fogColor, color4, fogFactor);
	}
	//else
	//{
	//	fColor = vec4(color, 1.0f);
	//}
	fColor=fColor2;
}
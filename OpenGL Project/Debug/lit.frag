#version 330 core

// TODO: Add material struct

// TODO: Add spotlight

struct LightInfo
{
	int Type;									//____| 0 >> POINT light |_____| 1 >> SPOT light |____| 2 >> directed light |____

	vec3 LightPosition_worldspace;				// Light Position in world space

	vec3 La;									// Ambient light intensity
	vec3 Ld;									// Diffuse light intensity
	vec3 Ls;									// Specular light intensity

	vec3 LightColor;
	float LightPower;

	vec3 SpotLightDirection_worldspace;			// Spotlight Position in world space
	float CutOff;								// For Spotlight
	float OuterCutOff;							// For Spotlight
};

#define LIGHT_COUNT 3

// Interpolated for each fragment
in vec2 TexCoord;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;

out vec4 gl_FragColor;

uniform LightInfo Lights[LIGHT_COUNT];
uniform sampler2D textureSampler;
uniform mat4 V;

vec3 CalcLight(LightInfo light);

void main(){

	vec3 color = vec3(0,0,0);

	for (int i = 0; i < LIGHT_COUNT; i++) {
		color += CalcLight(Lights[i]);
	}

	//vec3 fogColor = vec3(0.5,0.5,0.5);
	//float fogFactor = 10000 - length(Lights[2].LightPosition_worldspace - Position_worldspace);
	//fogFactor = fogFactor  / (10000 - 200);

	//vec3 finalColor = mix(fogColor, color, fogFactor);
	
	gl_FragColor = vec4(color, 1.0f);

}


vec3 CalcLight(LightInfo light) {

	vec3 LightColor = light.LightColor;																		// Light properties
	float LightPower = light.LightPower;

	vec3 LightPosition_cameraspace = ( V  * vec4(light.LightPosition_worldspace,1)).xyz;
	vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;					// Vector that goes from the vertex to the light, in camera space	


	// TODO: multiply by Material Ka, Kd, Ks here
	vec3 MaterialDiffuseColor = texture( textureSampler, TexCoord ).rgb;									// Material properties
	vec3 MaterialAmbientColor = vec3(0.15,0.15,0.15) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor =  vec3(0.3,0.3,0.3);

	float distance = length( light.LightPosition_worldspace - Position_worldspace );						// Distance to the light


	vec3 n = normalize( Normal_cameraspace );																// Normal of the computed fragment, in camera space

	vec3 l = normalize( LightDirection_cameraspace );														// Direction of the light (from the fragment to the light)

	float cosTheta = clamp( dot( n,l ), 0,1 );																// Cosine of the angle between the normal and the light direction, 
																											// clamped above 0
																												

	vec3 E = normalize(EyeDirection_cameraspace);															// Eye vector (towards the camera)

	vec3 R = reflect(-l,n);																					// Direction in which the triangle reflects the light

	float cosAlpha = clamp( dot( E,R ), 0,1 );																// Cosine of the angle between the Eye vector and the Reflect vector,
																											// clamped to 0
	if (light.Type == 0 /* point */ ) {
		// TODO: Multiply by La, Ld, Ls here
		// TODO: Remove LightColor, Light Power
		// TODO: Replace 8 by material shininess
		// TODO: constant + linear + quadratic attenuation
		return	MaterialAmbientColor +																			// Ambient : simulates indirect lighting		
				MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance)	+				// Diffuse : "color" of the object		
				MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,8) / (distance*distance);		// Specular : reflective highlight, like a mirror
	}

	if (light.Type == 1 /* spotlight */ ) {
		
		vec3 SpotLightDirection_cameraspace = (V * vec4(light.SpotLightDirection_worldspace, 0)).xyz;
		float theta = dot(l, normalize(-SpotLightDirection_cameraspace));
		float epsilon = light.CutOff - light.OuterCutOff;
		float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);

		return	MaterialAmbientColor +	
				MaterialDiffuseColor * intensity * LightColor * LightPower * cosTheta / (distance*distance)	+
				MaterialSpecularColor * intensity * LightColor * LightPower * pow(cosAlpha,8) / (distance*distance);
	}

	return vec3(0,0,0);
}
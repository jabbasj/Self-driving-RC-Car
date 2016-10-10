#version 330 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_TexCoords;

out vec2 TexCoord;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;


void main(){

	gl_Position =  MVP * vec4(in_Position, 1);		

	Position_worldspace = (M * vec4(in_Position,1)).xyz;									// Position of the vertex, in worldspace : M * position
	

	vec3 vertexPosition_cameraspace = ( V * M * vec4(in_Position,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;					// Vector that goes from the vertex to the camera (0,0,0), in camera space
	

	Normal_cameraspace = ( V * M * vec4(in_Normal,0)).xyz;									// Normal of the the vertex, in camera space
	
	TexCoord = in_TexCoords;
}


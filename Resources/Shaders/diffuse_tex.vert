#version 330 core

layout  (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout  (location = 2) in vec2 texCoord;
//layout (location = 3) in vec4 colors;

out vec2 TexCoord;
out vec3 fragPos;		// Position du fragment.
out vec3 normalSurf;	// Normal à la surface.

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 normalMatrix;  // Couteux, calculé 1 fois à l’extérieur du shader via glm.

void main()
{
	TexCoord = vec2(texCoord.x, texCoord.y);
	vec4 fragWorldSpace = model * vec4(position, 1.0);
	fragPos     = vec3(fragWorldSpace);		// Vertex dans l’espace world.
	normalSurf  = normalMatrix * normals;	// Normal dans l’espace world.
	gl_Position = proj * view * fragWorldSpace;
}


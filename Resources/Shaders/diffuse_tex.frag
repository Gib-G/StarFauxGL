#version 330 core

in vec2 TexCoord;	// Issu du vertex shader, et interpolé entre les 3 sommets.
in vec3 fragPos;	// Idem.
in vec3 normalSurf;	// Idem.

uniform sampler2D texture_diffuse;
uniform vec3      lightColor;
uniform vec3      lightPosition;

void main()
{
	vec3 N = normalize(normalSurf);
	vec3 L = normalize(lightPosition - fragPos);
	float diffuse = max(0,dot(N,L));
	
	gl_FragColor = texture(texture_diffuse, TexCoord)*vec4(diffuse * lightColor,1);
}

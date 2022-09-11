#version 330 core

uniform mat4 material;		// les 3 couleurs et paramètres sont "compactés" dans une matrice 4x4
uniform vec3 lightColor;

void main()
{
	vec3 ambientColor  = vec3(material[0][0],material[0][1],material[0][2]);
	vec3 diffuseColor  = vec3(material[0][1],material[1][1],material[1][2]);

	vec3 color = /*ambientColor +*/ diffuseColor;
	gl_FragColor = vec4(lightColor * color,1);
}

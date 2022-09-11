#version 330 core

in vec2 TexCoord;

uniform sampler2D texture_ambient;
uniform vec3      lightColor;

void main()
{
	gl_FragColor = texture(texture_ambient, TexCoord)*vec4(lightColor,1);
}

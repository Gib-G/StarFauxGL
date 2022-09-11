#version 330 core

in vec2 TexCoord;
in vec4 Colors;

//uniform sampler2D texture_ambient;
uniform  sampler2D texture_diffuse;
//uniform sampler2D texture_specular;
uniform float lum;

void main()
{
	vec4 color = texture(texture_diffuse, TexCoord);
	gl_FragColor = vec4(color.rgb * lum, color.a);
}

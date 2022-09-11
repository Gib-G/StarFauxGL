#version 330 core

in vec3 fragPos;	// Issu du vertex shader, et interpolé entre les 3 sommets.
in vec3 normalSurf;	// Idem.

uniform mat4  material;		// les 3 couleurs et paramètres sont "compactés" dans une matrice 4x4
uniform vec3  lightColor;
uniform vec3  lightPosition;
uniform vec3  cameraPosition;


//float getExponentialFogFactor(float c)
//{
//	float fogDensity = 0.00001;
//	return clamp(exp(-fogDensity*c*c), 0,1);  // borné dans [0,1]. 
//}

void main()
{
//	float d = distance(cameraPosition, fragPos);	// distance fragment-caméra.
//	float f = getExponentialFogFactor(d);			// Facteur d’atténuation.
//	vec3 fogColor = vec3(0.5,0.5,0.5);


	// Extraction des couleurs et paramètres
	vec3 ambientColor  = vec3(material[0][0],material[0][1],material[0][2]);
	vec3 diffuseColor  = vec3(material[0][1],material[1][1],material[1][2]);
	vec3 specularColor = vec3(material[0][2],material[2][1],material[2][2]);
	float shininess    = material[3][0];
	
	vec3 N = normalize(normalSurf);
	vec3 L = normalize(lightPosition - fragPos);
	vec3 R = reflect(-L,N);
	vec3 V = normalize(cameraPosition - fragPos);
	float diffuse   = max(0,dot(N,L));
	float specular  = pow(max(dot(V,R), 0.0), shininess);
	
	vec3 color = lightColor * (ambientColor + diffuse*diffuseColor + specular*specularColor);
	gl_FragColor = vec4(color,1);
//	gl_FragColor = vec4(f*color + (1-f)*fogColor,1);
}

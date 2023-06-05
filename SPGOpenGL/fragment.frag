#version 400

out vec4 fragColor;

in vec3 normal;
in vec3 pos;
in vec2 texCoord;

uniform vec3 lightPos1;
uniform vec3 lightPos2;
uniform vec3 viewPos;

uniform sampler2D texture_1; 
uniform sampler2D texture_2;

uniform int objectID;

//Model de iluminare Phong
vec3 lighting_src(vec3 objectColor, vec3 pos, vec3 normal, vec3 lightPos, vec3 viewPos,
				vec3 ambient, vec3 lightColor, vec3 specular, float specPower)
{
	vec3 L = normalize(lightPos - pos);
	vec3 V = normalize(viewPos - pos);
	vec3 N = normalize(normal);
	vec3 R = normalize(reflect(-L, N));
	
    float diffCoef = max(dot(L, N), 0.0);
    float specCoef = pow(max(dot(R, V), 0.0), specPower);

	vec3 ambientColor = ambient * lightColor;
	vec3 diffuseColor = diffCoef * lightColor * dot(L,N);
	vec3 specularColor = specCoef * specular * lightColor;
	vec3 col = ( ambientColor + diffuseColor + specularColor ) * objectColor; 

	return clamp(col, 0, 1);
}


void main() 
{
	if(objectID == 1)
	{
		vec3 objectColor = vec3(1.0, 1.0, 1.0);
		vec3 ambient = vec3(0.1);
		vec3 specular = vec3(0.8);
		float specPower = 32.0;
		vec3 diffuse = vec3(1.0, 0.0, 0.0);

		vec3 lightColor = vec3(1.0, 1.0, 1.0);
		vec3 color = lighting_src(objectColor, pos, normal, lightPos1, viewPos, ambient, lightColor, specular, specPower);
		
		vec3 lightColor2 = vec3(1.0, 1.0, 1.0);
		vec3 color2 = lighting_src(objectColor, pos, normal, lightPos2, viewPos, ambient, lightColor, specular, specPower);

		vec3 finalColor = color + color2;

		vec4 texColor = texture(texture_1, texCoord); 

		fragColor = vec4(finalColor, 1.0) * texColor;
	}
	else if (objectID == 2)
	{
		vec3 objectColor = vec3(1.0, 1.0, 1.0);
		vec3 ambient = vec3(0.3); 
		vec3 specular = vec3(1.0);
		float specPower = 32.0;
		vec3 diffuse = vec3(1.0, 0.0, 0.0);

		vec3 lightColor = vec3(1.0, 1.0, 1.0);
		vec3 color = lighting_src(objectColor, pos, normal, lightPos1, viewPos, ambient, lightColor, specular, specPower);

		vec3 finalColor = color;

		vec4 texColor = texture(texture_2, texCoord); 

		fragColor = vec4(finalColor, 1.0) * texColor;

	}
	else
	{
		return;
	}	
}

#version 400

//Obiectul 1

layout(location = 0) in vec3 vPos1;
layout(location = 1) in vec3 vNormal1;
layout(location = 2) in vec2 vTexCoord1; 

//Obiectul 2

layout(location = 3) in vec3 vPos2;     
layout(location = 4) in vec3 vNormal2;  
layout(location = 5) in vec2 vTexCoord2;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 normalMatrix;

uniform int objectID;

out vec3 normal;
out vec3 pos;
out vec2 texCoord;

void main() 
{
    if(objectID == 1)
    {
        vec3 transformedPos = vec3(-vPos1.x, vPos1.y, vPos1.z);
        gl_Position = modelViewProjectionMatrix * vec4(transformedPos, 1.0);
        normal = normalize(vec3(normalMatrix * vec4(vNormal1, 0.0)));
        pos = vPos1;
        texCoord = vTexCoord1;
    }
    else if (objectID == 2)
    {
        gl_Position = modelViewProjectionMatrix * vec4(vPos2, 1.0);
        normal = normalize(vec3(normalMatrix * vec4(vNormal2, 0.0)));
        pos = vPos2;
        texCoord = vTexCoord2;
    }
    else
    {
        return;
    }
}

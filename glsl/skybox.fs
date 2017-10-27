#version 440 core
out vec4 FColor;

in VSOutput
{
    vec3 texcoord;
} fsInput;

uniform samplerCube Skybox;

void main()
{    
    FColor = texture(Skybox, fsInput.texcoord);
}
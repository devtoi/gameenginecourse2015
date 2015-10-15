#new_shader vertex
#version 430

in vec4 PosL;
in vec4 NormalL;
in vec4 TexIn;
in vec4 TangentL;
uniform uint g_BufferOffset;
uniform mat4 g_ViewProj;
#define MAX_OBJECTS 1000
struct ShaderInput{
		mat4 World;
		vec4 Color;
};
layout(std430, binding = 0) buffer ShaderInputs{
    ShaderInput g_Input[MAX_OBJECTS];
};
void main(){
	uint id = g_BufferOffset + gl_InstanceID;
	vec4 PosW = g_Input[id].World * vec4(PosL.xyz,1);
	gl_Position = g_ViewProj * PosW;
}
#end_shader
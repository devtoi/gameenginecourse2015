#new_shader vertex
#version 430
in vec4 PosL;
in vec4 NormalL;
in vec4 TexIn;
in vec4 TangentL;

out vec3 Pos;

uniform mat4 g_ProjView;
uniform mat4 g_World;

void main(){
	vec4 PosV = g_ProjView * (g_World * vec4(PosL.xyz,1));
	gl_Position = PosV.xyww;
	Pos = PosL.xyz;
}
#end_shader

#new_shader fragment
#version 430

in vec3 Pos;
uniform samplerCube g_SkyTex;

layout(location = 0) out vec4 FragmentColor;
layout(location = 1) out vec2 FragmentNormal;
layout(location = 2) out vec2 FragmentRoughMetal;

void main(){
	FragmentColor = texture(g_SkyTex, Pos.xyz);
	FragmentColor.a = 0; //dont do light calculation
	FragmentRoughMetal = vec2(0,0);
	FragmentNormal = vec2(0,0);
}
#end_shader
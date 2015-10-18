#new_shader vertex
#version 430
layout (location = 0) in vec4 PosL;
layout (location = 1) in vec4 NormalL;
layout (location = 2) in vec4 TexIn;
layout (location = 3) in vec4 TangentL;

out struct {
	vec4 NormalW;
	vec4 TexCoord;
	vec4 TangentW;
	vec4 Color;
} v_Out;

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
	vec4 PosW;
	PosW = g_Input[id].World * vec4(PosL.xyz,1);
	v_Out.NormalW = g_Input[id].World * vec4(NormalL.xyz,0);
	v_Out.TangentW = g_Input[id].World * vec4(TangentL.xyz,0);

	v_Out.TexCoord = TexIn;
	v_Out.Color = g_Input[id].Color;
	gl_Position = g_ViewProj * PosW;
}
#end_shader

#new_shader fragment
#version 430
layout(early_fragment_tests) in;
in struct {
	vec4 NormalW;
	vec4 TexCoord;
	vec4 TangentW;
	vec4 Color;
} v_Out;

layout(binding=0) uniform 	sampler2D 	g_DiffuseTex;
layout(binding=1) uniform 	sampler2D 	g_NormalTex;
layout(binding=2) uniform 	sampler2D 	g_RoughnessTex;
layout(binding=3) uniform 	sampler2D 	g_MetallicTex;

layout(location = 0) out vec4 FragmentColor;
layout(location = 1) out vec3 FragmentNormal;
layout(location = 2) out vec2 FragmentRoughMetal;

#pragma optionNV(fastmath on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

vec2 EncodeNormal(vec3 n){
	float f = sqrt(8 * n.z + 8);
	return n.xy / f + 0.5f;
}

vec3 CalcBumpedNormal(vec3 Normal, vec3 Tangent, sampler2D normalMap, vec2 uv){
	vec3 normal = normalize(Normal);
	vec3 tangent = normalize(Tangent);
	tangent = normalize(tangent - dot(tangent,normal) * normal);
	vec3 bitangent = cross(tangent,normal);

	vec3 bump = texture(normalMap,uv).xyz;
	bump = (bump * 2.0) - 1.0; 

	vec3 newNormal;
	mat3 TBN = mat3(tangent,bitangent,normal);
	newNormal = TBN * bump;
	return normalize(newNormal);
}

void main(){
	
	float roughness = texture(g_RoughnessTex,v_Out.TexCoord.xy).r;
	float metallic =  texture(g_MetallicTex, v_Out.TexCoord.xy).r;
	const float minRoughness=1e-8;
	roughness = max(minRoughness, roughness);

	vec3 normal = CalcBumpedNormal(v_Out.NormalW.xyz, v_Out.TangentW.xyz, g_NormalTex, v_Out.TexCoord.xy);
	vec4 albedo = texture(g_DiffuseTex, v_Out.TexCoord.xy);
	if(albedo.a <= 0.01f){
		discard;
	}
	vec3 baseColor = pow(albedo.xyz, vec3(2.2)) * v_Out.Color.xyz; //raise to 2.2(gamma) to be in linear space

	FragmentNormal = normalize(normal);
	FragmentColor.xyz = baseColor;
	FragmentColor.a = 1; //do light calculation
	FragmentRoughMetal = vec2(roughness, metallic);
}
#end_shader

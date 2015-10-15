#new_shader vertex
#version 430
layout (location = 0) in vec4 PosL;
layout (location = 1) in vec4 NormalL;
layout (location = 2) in vec4 TexIn;
layout (location = 3) in vec4 TangentL;

out struct {
	vec4 PosW;
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
layout(std430, binding = 1) buffer TransparentShaderInputs{
    ShaderInput g_Input[MAX_OBJECTS];
};

void main(){
	uint id = g_BufferOffset + gl_InstanceID;
	v_Out.PosW = g_Input[id].World * vec4(PosL.xyz,1);
	v_Out.NormalW = g_Input[id].World * vec4(NormalL.xyz,0);
	v_Out.TangentW = g_Input[id].World * vec4(TangentL.xyz,0);

	v_Out.TexCoord = TexIn;
	v_Out.Color = g_Input[id].Color;
	gl_Position = g_ViewProj * v_Out.PosW;
}
#end_shader

#new_shader fragment
#version 430
layout(early_fragment_tests) in;
in struct {
	vec4 PosW;
	vec4 NormalW;
	vec4 TexCoord;
	vec4 TangentW;
	vec4 Color;
} v_Out;

layout(binding=0) uniform 	sampler2D 	g_DiffuseTex;
layout(binding=1) uniform 	sampler2D 	g_NormalTex;
layout(binding=2) uniform 	sampler2D 	g_RoughnessTex;
layout(binding=3) uniform 	sampler2D 	g_MetallicTex; 
layout(binding=4) uniform 	sampler2D 	g_DepthBuffer;

layout(binding = 6) uniform samplerCube g_SkyCubeTex;
layout(binding = 7) uniform samplerCube g_IrradianceCubeTex;

layout(location = 0) out vec4 acum;
layout(location = 1) out vec4 revealage;

uniform float g_ZNear;
uniform float g_ZFar;
uniform float g_Transparency;
uniform uint  g_NumDLights;
uniform vec3  g_CamPos;
//shared
#define MAX_LIGHTS_PER_TILE 256
#define MAX_POINTLIGHTS 1024
#define MAX_DIRLIGHTS 4
#define MAX_NUM_LIGHTS MAX_POINTLIGHTS + MAX_DIRLIGHTS

#include PBR.txt

layout(std430, binding = 3) readonly buffer LightBuffer
{
	Light lights[MAX_NUM_LIGHTS];
};


float d(float z){
	return ((g_ZNear * g_ZFar) / (z - g_ZFar)) / (g_ZNear - g_ZFar);
}

float w(float z, float alpha){
	return alpha * max(0.01, 3 * 1000 * pow((1.0 - d(z)), 3) );
}
void main(){

	if(gl_FragCoord.z > texelFetch(g_DepthBuffer, ivec2(gl_FragCoord.xy), 0).r){
		discard;
	}
	vec4 color = texture(g_DiffuseTex, v_Out.TexCoord.xy) * v_Out.Color;
	float roughness = texture(g_RoughnessTex, v_Out.TexCoord.xy).r;
	float metal = texture(g_MetallicTex, v_Out.TexCoord.xy).r;

	float alpha = color.a * g_Transparency;
	vec3 normal = normalize(v_Out.NormalW.xyz);
	vec4 lightColor = vec4(0.0);
	uint i;
	for(i = 0; i < g_NumDLights; ++i){
		Light d = lights[MAX_POINTLIGHTS + i];
		//just do diffuse half lambert shading
		float ndotl = saturate(dot(normal, normalize(d.Direction) * -1));
		ndotl = ndotl * 0.5 + 0.5;
		color.xyz *= ndotl;
		//lightColor += CalcDLight(d, v_Out.NormalW.xyz, v_Out.PosW.xyz, g_CamPos.xyz, color.xyz, roughness, metal );
	}
	acum = vec4(color.xyz, alpha) * w(gl_FragCoord.z, alpha);
	revealage = vec4(alpha);
}

#end_shader
#new_shader compute
#version 430
#define WORK_GROUP_SIZE 16
layout(local_size_x = WORK_GROUP_SIZE, local_size_y = WORK_GROUP_SIZE) in;
#pragma optionNV(fastmath on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

//GBuffers
layout(binding = 0) uniform sampler2D gAlbedoBuffer;
layout(binding = 1) uniform sampler2D gNormalBuffer;
layout(binding = 2) uniform sampler2D gRoughMetalBuffer;
layout(binding = 3) uniform sampler2D gDepthBuffer;

layout(rgba8, binding = 0) writeonly restrict uniform image2D output_img;
//uniforms
uniform vec2 gScreenSize;
uniform uint gTotalLightCount;
uniform uint numDLights;
uniform mat4 gProj;
uniform mat4 gView;
uniform mat4 gInvProjView;
uniform vec3 gCamPos;
uniform vec2 gScreenOffset;
uniform vec2 gViewportSize;
uniform mat4 gLightMatrix;

layout(binding = 4) uniform sampler2D	g_BRDFTex;
layout(binding = 5) uniform sampler2DShadow g_ShadowMap;
layout(binding = 6) uniform samplerCube g_SkyCubeTex;
layout(binding = 7) uniform samplerCube g_IrradianceCubeTex;


//shared
#define MAX_LIGHTS_PER_TILE 256
#define MAX_POINTLIGHTS 1024
#define MAX_DIRLIGHTS 4
#define MAX_NUM_LIGHTS MAX_POINTLIGHTS + MAX_DIRLIGHTS

shared int sMaxDepth;
shared int sMinDepth;
shared uint sPointLightCount;
shared uint sPointLightIndex[MAX_LIGHTS_PER_TILE];

#include PBR.txt

layout(std430, binding = 3) readonly buffer LightBuffer
{
	Light lights[MAX_NUM_LIGHTS];
};
vec3 DecodeNormal(vec4 enc){
    vec2 fenc = enc.xy*4-2;
    float f = dot(fenc,fenc);
    float g = sqrt(1-f/4);
    vec3 n;
    n.xy = fenc*g;
    n.z = 1-f/2;
    return n;
}
float ComputeShadow(vec3 posW){
	vec4 shadowCoord = (gLightMatrix * vec4(posW,1));
    vec3 ProjCoords = shadowCoord.xyz / shadowCoord.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;
    vec2 offset = 1.0 / vec2(4096);
    float factor = 0;
    for(int x = -1; x <= 1; x++){
    	for(int y = -1; y <= 1; y++){
    		vec2 Offsets = vec2(x * offset.x, y * offset.y);
    		vec3 UVC = vec3(UVCoords + Offsets, z + 0.0001);
    		factor += texture(g_ShadowMap, UVC);
    	}
    }

	return factor / 9.0f;
}
vec3 Reinhard(vec3 c){
	return c / (c + 1);
}
vec3 Uncharted2Tonemap(vec3 x)
{
    float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;

    return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float RGBLuma(vec3 rgb){
	return 0.2126 * rgb.r + 0.7152 * rgb.g + 0.0722 * rgb.b;
}

void main()
{
	if(gl_LocalInvocationIndex == 0)
	{
		sMaxDepth = int(0xFFFFFFFF);
		sMinDepth = 0;
		sPointLightCount = 0;
	}
	barrier(); //sync

	ivec2 screenPos = ivec2(gScreenOffset) + ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(screenPos) / vec2(gScreenSize - 1);
	
	float depth = texelFetch(gDepthBuffer, screenPos.xy, 0).r * 2.0f - 1.0f;
	float zNear = gProj[3][2] / (gProj[2][2] - 1.0f);
	float zFar 	= gProj[3][2] / (gProj[2][2] + 1.0f);
	float clipDelta = zFar - zNear;

	vec2 viewPortUV = vec2(gl_GlobalInvocationID.xy) / vec2(gViewportSize - 1);
	//Worldpos
	vec4 sPos = vec4(viewPortUV * 2 - 1, depth,1);
	vec4 projPos = gInvProjView * sPos;
	vec4 posW = vec4(projPos.xyz / projPos.w , projPos.w);

	float linearDepth = (1.0f / posW.w - zNear) / clipDelta;
	int unsignedDepth = floatBitsToInt(linearDepth);

	atomicMin(sMinDepth, unsignedDepth);
	atomicMax(sMaxDepth, unsignedDepth);

	barrier(); //sync

	float minDepthZ = intBitsToFloat( sMinDepth );
	float maxDepthZ = intBitsToFloat( sMaxDepth );

	minDepthZ = minDepthZ * clipDelta + zNear;
	maxDepthZ = maxDepthZ * clipDelta + zNear;

	vec2 tileScale = (gScreenSize / float(2 * WORK_GROUP_SIZE));
	vec2 tileBias = tileScale - vec2(gl_WorkGroupID.xy);

	vec4 col1 = vec4(-gProj[0][0] * tileScale.x , 0 					, tileBias.x , 0);
	vec4 col2 = vec4( 0 				, -gProj[1][1] * tileScale.y 	, tileBias.y , 0);
	vec4 col4 = vec4( 0 				, 0 					                 , -1.0f 	   , 0);

	vec4 frustrumPlanes[6];
	//left plane
	frustrumPlanes[0] = col4 + col1;
	//right plane
	frustrumPlanes[1] = col4 - col1;
	//top plane
	frustrumPlanes[2] = col4 - col2;
	//bottom plane
	frustrumPlanes[3] = col4 + col2;
	//far plane
	frustrumPlanes[4] = vec4(0.0f, 0.0f,  1.0f, maxDepthZ);
	//near plane
	frustrumPlanes[5] = vec4(0.0f, 0.0f, -1.0f, -minDepthZ);
	//normalize planes
	for(int i = 0; i < 4;i++)
	{
		frustrumPlanes[i].xyz *= 1.0f / length(frustrumPlanes[i].xyz);
	}
	uint threadCount = WORK_GROUP_SIZE * WORK_GROUP_SIZE;
	//calc how many lights this thread should check
	uint passCount = (gTotalLightCount + threadCount - 1) / threadCount;

	for(uint pass = 0; pass < passCount; ++pass)
	{
		uint lightIndex = pass * threadCount + gl_LocalInvocationIndex;

		if(lightIndex > gTotalLightCount)
			break;

		Light currentLight = lights[lightIndex];
		vec4 pos =  (gView) * vec4(currentLight.Position,1);
		float radius = currentLight.Range;
		bool inFrustrum = true;
		for(uint i = 0; i <= 5 && inFrustrum; i++ )
		{
			float dist = dot(frustrumPlanes[i], pos);
			inFrustrum = (-radius <= dist);
		}
		if(inFrustrum)
		{
			uint id = atomicAdd(sPointLightCount,1);
			sPointLightIndex[id] = lightIndex;
		}
	}
	barrier();
	//make sure we are inside the viewport
	if(any(greaterThan(screenPos, (gScreenOffset + gViewportSize - 1)))){
		//imageStore(output_img, screenPos, vec4(0.39f, 0.61f, 0.93f, 1.0f));
		return;
	}

	vec4 albedo = pow(texture(gAlbedoBuffer, uv), vec4(2.2));
	if(albedo.a < 1){
		imageStore(output_img, screenPos, albedo);
		return;
	}

	vec3 normal = texture(gNormalBuffer, uv).xyz;
	normal = normalize(normal);
	vec2 roughnessMetal = texture(gRoughMetalBuffer,uv).xy;

	vec4 lightColor = vec4(0.0);
	uint i;
	float shadow =  1.0f;//ComputeShadow(posW.xyz);
	for(i = 0; i < sPointLightCount; ++i){
		Light p = lights[sPointLightIndex[i]];
		lightColor += CalcPLight(p, normal, posW.xyz, gCamPos.xyz, albedo.xyz, roughnessMetal.x, roughnessMetal.y );
	}
	for(i = 0; i < numDLights; ++i){
		Light d = lights[MAX_POINTLIGHTS + i];
		lightColor += CalcDLight(d, normal, posW.xyz, gCamPos.xyz, albedo.xyz, roughnessMetal.x, roughnessMetal.y );
	}
	//IBL
	lightColor += CalcIBLLight( normal, posW.xyz, gCamPos.xyz, albedo.xyz, roughnessMetal.x, roughnessMetal.y, g_SkyCubeTex, g_IrradianceCubeTex, g_BRDFTex);
	float luma = dot( lightColor.rgb, vec3(0.299, 0.587, 0.114) );
	vec4 outColor = vec4(Reinhard(lightColor.rgb), luma);
	imageStore(output_img, screenPos, pow(outColor, vec4(1.0 / 2.2)));
	
}
#end_shader

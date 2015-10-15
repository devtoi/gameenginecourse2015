#new_shader vertex
#version 430
layout( location = 0 ) in vec4 PosL;
layout( location = 1 ) in vec4 NormalL;
layout( location = 2 ) in vec4 TexIn;
layout( location = 3 ) in vec4 TangentL;

layout( location = 0 ) out vec3 posW_CS_in;
layout( location = 1 ) out vec3 normalW_CS_in;
layout( location = 2 ) out vec3 tangentW_CS_in;
layout( location = 3 ) out vec2 texCoord_CS_in;

uniform mat4 g_World;
void main(){
	posW_CS_in = (g_World * vec4(PosL.xyz,1)).xyz;
	normalW_CS_in = (g_World * vec4(NormalL.xyz,0)).xyz;
	texCoord_CS_in = TexIn.xy;
	tangentW_CS_in = (g_World * vec4(TangentL.xyz,0)).xyz;
}
#end_shader

#new_shader control
#version 430
layout (vertices = 4) out;

uniform vec4 g_TesFactor;
uniform vec3 g_TesScaling;
uniform vec3 g_CamPos;

layout( location = 0 ) in vec3 posW_CS_in[];
layout( location = 1 ) in vec3 normalW_CS_in[];
layout( location = 2 ) in vec3 tangentW_CS_in[];
layout( location = 3 ) in vec2 texCoord_CS_in[];


layout( location = 0 ) out vec3 posW_EV_in[];
layout( location = 1 ) out vec3 normalW_EV_in[];
layout( location = 2 ) out vec3 tangentW_EV_in[];
layout( location = 3 ) out vec2 texCoord_EV_in[];
layout( location = 4 ) out float innerTesFactor_EV_in[];

float Lod(vec3 posw, vec3 campos){
	float d = length(campos - posw);
	float lod = g_TesFactor.w;
	if(d < g_TesScaling.x) lod = g_TesFactor.x;
	if(g_TesScaling.x <= d && d < g_TesScaling.y) lod = g_TesFactor.y;
	if(g_TesScaling.y <= d && d < g_TesScaling.z) lod = g_TesFactor.z;
	return lod;
}
void main()
{
	// Set output
	posW_EV_in[gl_InvocationID] = posW_CS_in[gl_InvocationID];
	normalW_EV_in[gl_InvocationID] = normalW_CS_in[gl_InvocationID];
	texCoord_EV_in[gl_InvocationID] = texCoord_CS_in[gl_InvocationID];
	tangentW_EV_in[gl_InvocationID] = tangentW_CS_in[gl_InvocationID];

	if(gl_InvocationID == 0){
		vec3 d1 = posW_CS_in[1] + (posW_CS_in[2] - posW_CS_in[1]) * 0.5;
		vec3 d2 = posW_CS_in[0] + (posW_CS_in[2] - posW_CS_in[0]) * 0.5;
		vec3 d3 = posW_CS_in[0] + (posW_CS_in[1] - posW_CS_in[0]) * 0.5;
		float e1 = Lod(d1, g_CamPos);
		float e2 = Lod(d2, g_CamPos);
		float e3 = Lod(d3, g_CamPos);
		gl_TessLevelOuter[0] = Lod(d1, g_CamPos);
		gl_TessLevelOuter[1] = Lod(d2, g_CamPos);
		gl_TessLevelOuter[2] = Lod(d3, g_CamPos);

		float inner = floor((min(e1, min(e2,e3)) + max(e1, max(e2,e3))) * 0.5);
		gl_TessLevelInner[0] = inner;
	}
}
#end_shader

#new_shader evaluation
#version 430
layout(triangles,fractional_even_spacing) in;

uniform mat4 g_ViewProj;
uniform float g_Scale;
uniform vec2  g_Size;
uniform float g_HeightTiling;
uniform float g_TileScale;
layout(binding = 2) uniform sampler2D g_HeightMap;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}
layout( location = 0 ) in vec3 posW_EV_in[];
layout( location = 1 ) in vec3 normalW_EV_in[];
layout( location = 2 ) in vec3 tangentW_EV_in[];
layout( location = 3 ) in vec2 texCoord_EV_in[];
layout( location = 4 ) in float innerTesFactor_EV_in[];

layout( location = 0 ) out vec4 PosW;
layout( location = 1 ) out vec4 NormalW;
layout( location = 2 ) out vec4 TexOut;
layout( location = 3 ) out vec4 TangentW;

void main(){
	//world position
	PosW = vec4(interpolate3D(posW_EV_in[0],posW_EV_in[1],posW_EV_in[2]), 1); // vec4(mix(v1,v2,gl_TessCoord.y),1);
	//normal
	NormalW = vec4(interpolate3D(normalW_EV_in[0],normalW_EV_in[1],normalW_EV_in[2]), 0);
	//tangent
	TangentW = vec4(interpolate3D(tangentW_EV_in[0],tangentW_EV_in[1],tangentW_EV_in[2]), 0);
	//texture coordinates
	TexOut = vec4(interpolate2D(texCoord_EV_in[0],texCoord_EV_in[1],texCoord_EV_in[2]), 0, 0);

	//height mapping
	float h = texture(g_HeightMap, TexOut.xy * g_HeightTiling).r;
	//normal calculation
	const float tilesize = g_TileScale / (gl_TessLevelInner[0]);
	const vec2 texelSize = vec2(tilesize) / g_Size; //vec2(textureSize(g_HeightMap, 0))
	
	NormalW.xyz = normalize(NormalW.xyz);

	vec3 dx = PosW.xyz + vec3(tilesize, 0 ,0);
	vec3 dz = PosW.xyz + vec3(0, 0 ,tilesize);
	float h2 = texture(g_HeightMap, (TexOut.xy + vec2(texelSize.x, 0)) * g_HeightTiling).r;
	float h3 = texture(g_HeightMap, (TexOut.xy + vec2(0, texelSize.y)) * g_HeightTiling).r;
	dx += NormalW.xyz * h2 * g_Scale;
	dz += NormalW.xyz * h3 * g_Scale;

	PosW.xyz += NormalW.xyz * h * g_Scale;
	NormalW.xyz = normalize(cross(dz - PosW.xyz, dx - PosW.xyz));
	//find new tangent
	vec3 c1 = cross(NormalW.xyz, vec3(0.0, 0.0, 1.0)); 
	vec3 c2 = cross(NormalW.xyz, vec3(0.0, 1.0, 0.0)); 
	if(length(c1) > length(c2)) {
		TangentW.xyz = c1;
	}else {
		TangentW.xyz = c2;
	}

	gl_Position = g_ViewProj * vec4(PosW.xyz,1);

}
#end_shader
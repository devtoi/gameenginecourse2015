#new_shader vertex
#version 430

layout (location = 0) in vec4 PosL;
layout (location = 1) in vec4 NormalL;
layout (location = 2) in vec4 TexIn;
layout (location = 3) in vec4 TangentL;
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

#new_shader geometry
#version 430
layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;
#define MAX_VIEWPORTS 4

uniform vec4 g_Viewports[MAX_VIEWPORTS];
uniform int g_FrustrumSegmentCount;
uniform vec2 g_ShadowMapSize;

vec2 GetWindowPos(vec4 posH, uint vp){
	vec2 ndc = posH.xy / posH.w;
	ndc = (ndc + 1) * 0.5;
	vec2 viewPos = (g_Viewports[vp].zw * ndc) + g_Viewports[vp].xy;
	return viewPos;
}

bool BBIntersects(vec4 bb0, vec4 bb1){
	bool xmiss = bb0.x > bb1.z || bb0.z < bb1.x;
	bool ymiss = bb0.y > bb1.w || bb0.w < bb1.y;
	return !xmiss && !ymiss;
}

void SendPrimitive(int vp){
	for(int i = 0; i < gl_in.length();++i){
		gl_Position = gl_in[i].gl_Position;
		gl_ViewportIndex = vp;
		gl_Layer = vp;
		EmitVertex();
	}
	EndPrimitive();
}

void main(){
	const vec4 mapBounds = vec4(0,0,g_ShadowMapSize);
	for(int segment = 0; segment < g_FrustrumSegmentCount; ++segment){
		vec2 startPos = GetWindowPos(gl_in[0].gl_Position, segment);
		vec4 primBounds = vec4(startPos, startPos);
		for(int i = 1; i < gl_in.length(); ++i){
			vec2 windowPos = GetWindowPos(gl_in[i].gl_Position, segment);
			primBounds.x = min(primBounds.x, windowPos.x);
			primBounds.y = min(primBounds.y, windowPos.y);
			primBounds.x = max(primBounds.x, windowPos.x);
			primBounds.x = max(primBounds.y, windowPos.y);
		}
		if(BBIntersects(primBounds, mapBounds)){
			SendPrimitive(segment);
		}
	}
}
#end_shader

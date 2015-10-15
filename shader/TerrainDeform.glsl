#new_shader compute
#version 430
#define WORK_GROUP_SIZE 16
layout(local_size_x = WORK_GROUP_SIZE, local_size_y = WORK_GROUP_SIZE) in;

uniform sampler2D 	g_BrushTex;
uniform bool		g_Additive;
layout(r8, binding = 0) uniform image2D output_img;
uniform vec2 		g_TerrainTexSize;
uniform vec2 		g_BrushSize;
uniform vec2 		g_BrushPos;
uniform float 		g_Strength;
uniform float 		g_Hardness;
uniform int 		g_Direction;
void main(){
	ivec2 screenPos = ivec2(g_BrushPos * g_TerrainTexSize) + ivec2(gl_GlobalInvocationID.xy);
	vec2 terrainuv = screenPos / g_TerrainTexSize;

	if( any(greaterThan(terrainuv,(g_BrushPos + g_BrushSize))) || any(lessThan(terrainuv , g_BrushPos))){
		return;
	} 
	vec2 brushUv = vec2(gl_GlobalInvocationID.xy) / vec2(g_TerrainTexSize); 

	vec4 srcTex = texture(g_BrushTex, brushUv);
	vec4 dstTex = imageLoad(output_img, screenPos);

	float newHeight = 0;
	if(g_Additive){
		newHeight = dstTex.r + (srcTex.r * g_Strength * g_Direction);
	}else {
		newHeight = (srcTex.r + dstTex.r * (1.0 - srcTex.a)) * (g_Strength * g_Direction);
	}
	imageStore(output_img, screenPos, vec4(brushUv.y, 0, 0, 1));
}

#end_shader
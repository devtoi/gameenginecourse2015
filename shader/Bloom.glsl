#new_shader compute
#version 430
#define WORK_GROUP_SIZE 32
layout(local_size_x = 32, local_size_y = 32) in;

#pragma optionNV(fastmath on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

uniform sampler2D 	g_BluredBloomTex;
uniform	sampler2D	g_ColorBuffer;
layout(rgba8, binding = 0) writeonly restrict uniform image2D output_img;
uniform vec2 		g_ScreenSize;
void main(){
	ivec2 screenPos = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(screenPos) / vec2(g_ScreenSize - 1);
	vec4 outColor = texture(g_ColorBuffer, uv) + texture(g_BluredBloomTex, uv);
	imageStore(output_img, screenPos, outColor);
}

#end_shader
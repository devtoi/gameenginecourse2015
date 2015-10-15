#new_shader compute
#version 430
#define WORK_GROUP_SIZE 32
layout(local_size_x = WORK_GROUP_SIZE, local_size_y = WORK_GROUP_SIZE) in;
#pragma optionNV(fastmath on)
#pragma optionNV(ifcvt none)
#pragma optionNV(inline all)
#pragma optionNV(strict on)
#pragma optionNV(unroll all)

uniform sampler2D 	g_Image;
uniform float 		g_Threshold = 0.5;
uniform float 		g_Smoothness = 0.5;
uniform vec2 		g_ScreenSize;
layout(rgba8, binding = 0) writeonly restrict uniform image2D output_img;

void main(){
	ivec2 screenPos = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(screenPos) / vec2(g_ScreenSize - 1);

	vec4 texColor = texture(g_Image, uv);
	//Luma is stored in alpha channel
	vec4 outColor = vec4(0);
	vec3 color = texColor.xyz * vec3(smoothstep(g_Threshold, g_Threshold + g_Smoothness, texColor.a));
	outColor = vec4(color,1);
	imageStore(output_img, screenPos, outColor);
}

#end_shader
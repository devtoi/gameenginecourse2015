#new_shader compute
#version 430
#define WORK_GROUP_SIZE 32
layout(local_size_x = 32, local_size_y = 32) in;
layout(r32f, binding = 0) restrict uniform image2D output_img;
uniform vec4 		g_PixelArea;
void main(){
	ivec2 screenPos = ivec2(g_PixelArea.xy) + ivec2(gl_GlobalInvocationID.xy);
	//check if we are outside the area
	if(any(greaterThan(screenPos, ivec2(g_PixelArea.xy + g_PixelArea.zw)))){
		return;
	}

	vec4 color = vec4(0);
	//simple box filter
	for(int x = -1; x < 2; ++x){
		for(int y = -1; y < 2; ++y){
			color += imageLoad(output_img, screenPos + ivec2(x,y));
		}
	}
	color /= vec4(9);
	imageStore(output_img, screenPos, color);
}

#end_shader
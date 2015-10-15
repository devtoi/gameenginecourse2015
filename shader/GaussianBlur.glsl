#new_shader compute
#version 430
#define WORK_GROUP_SIZE 32
layout(local_size_x = 32, local_size_y = 32) in;

layout(binding = 1) uniform sampler2D g_Texture;
uniform vec2 g_PixelSize;
uniform bool g_Horisontal = false;
uniform vec2 g_ScreenSize;
layout(rgba8, binding = 0) writeonly restrict uniform image2D output_img;

vec3 GaussianBlur( sampler2D tex0, vec2 texcoord, vec2 pixelOffset){
	//35 x 35 filter
 	/*
 	const int stepCount = 9;

    const float weights[stepCount] ={
        0.10855,
        0.13135,
        0.10406,
        0.07216,
        0.04380,
        0.02328,
        0.01083,
        0.00441,
        0.00157
     };
    const float offsets[stepCount] ={
        0.66293,
        2.47904,
        4.46232,
        6.44568,
        8.42917,
        10.41281,
        12.39664,
        14.38070,
        16.36501
     };
	*/
	const int stepCount = 2;
	const float weights[stepCount] ={
		0.44908, 0.05092
	};
	const float offsets[stepCount] ={
		0.53805, 2.0678
	};
     vec3 color = vec3(0);
	for(int i = 0; i < stepCount; i++){
		vec2 texOffset = offsets[i] * pixelOffset;
		vec3 col = texture(tex0,texcoord + texOffset).xyz + texture(tex0,texcoord - texOffset).xyz;
		color += col * weights[i];
	}
	return color;
}
void main(){
	ivec2 screenPos = ivec2(gl_GlobalInvocationID.xy);
	vec2 uv = vec2(screenPos) / vec2(g_ScreenSize - 1);
	vec3 color = vec3(1);
	if(g_Horisontal){
		color = GaussianBlur(g_Texture, uv, vec2(g_PixelSize.x, 0));
	}else{
		color = GaussianBlur(g_Texture, uv, vec2(0, g_PixelSize.y));
	}

	imageStore(output_img, screenPos, vec4(color, 1));
	//imageStore(output_img, screenPos, vec4(texture(g_Texture, uv).xyz, 1));
}
#end_shader
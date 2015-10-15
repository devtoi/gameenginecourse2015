#new_shader compute
#version 430
layout(local_size_x = 32, local_size_y = 32) in;

uniform float hardness;
layout(binding = 0, rgba8) writeonly uniform image2D Tex;

void main(){
	ivec2 threadIndex = ivec2( gl_GlobalInvocationID.xy );
	ivec2 texSize = imageSize(Tex);
	vec2 uv = vec2(threadIndex) / vec2(texSize);
	float dist = length(uv - vec2(0.5));

	vec4 color = vec4(0);
	if(dist <= 0.5){
		color = vec4(1,1,1, hardness * (0.5 - dist) * 2);
		color.xyz *= color.a; //use premultiplied alpha
	} else {
		color = vec4(0);
	}
	imageStore( Tex, threadIndex, color);
}
#end_shader
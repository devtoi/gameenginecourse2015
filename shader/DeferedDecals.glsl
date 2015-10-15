#new_shader vertex
#version 430
layout (location = 0) in vec3 PosL;
uniform mat4 g_World;
uniform mat4 g_ViewProj;
out vec4 PosH;

void main() {
  vec3 PosW = (g_World * vec4(PosL.xyz,1)).xyz;
  gl_Position = g_ViewProj * vec4(PosW,1);
}
#end_shader

#new_shader fragment
#version 430
uniform sampler2D g_DecalAlbedo;
uniform sampler2D g_Depth;
uniform mat4 g_InvWorld;
uniform mat4 g_InvViewProj;
uniform vec2 g_ViewportSize;
uniform vec2 g_ViewportOffset;
uniform vec4 g_Tint;

layout(location = 0) out vec4 FragmentColor;

void main() {
	vec2 viewportuv = (gl_FragCoord.xy - vec2(0.5f) - g_ViewportOffset) / g_ViewportSize.xy;

	float depth = texelFetch(g_Depth, ivec2(gl_FragCoord.xy), 0).x * 2.0f - 1.0f;
  	vec4 sPos = vec4(viewportuv * 2 - 1, depth, 1);
  	sPos = g_InvViewProj * sPos;
  	vec4 GbufferPosW = vec4(sPos.xyz / sPos.w, 1.0f);

	vec4 localPos = g_InvWorld * GbufferPosW;
	localPos = vec4(localPos.xyz / localPos.w,1);

	bvec2 inBox = lessThan(abs(localPos.xz), vec2(1.0f));

	if(all(inBox)) {
    	vec2 uv = localPos.xz * 0.5f + 0.5f;

		vec4 diffuse = texture(g_DecalAlbedo, uv) * g_Tint;
	    FragmentColor = diffuse;
	    FragmentColor.a = 1;
	} else {
	    discard;
  	}
}
#end_shader

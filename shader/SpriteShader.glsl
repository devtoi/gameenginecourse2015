#new_shader vertex
#version 330
void main(){

}
#end_shader

#new_shader geometry
#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform vec4 g_Pos; //in 0 -> 1 space, xy screen coords, zw texturecoords
uniform vec4 g_Size; //0 -> 1, xy screen, zw texture

out vec2 texcoord;
void main(){
	vec2 pos1 = vec2(g_Pos.x + g_Size.x, g_Pos.y);
	pos1 = pos1 * 2 - 1;
	vec2 tex1 = vec2(g_Pos.z + g_Size.z, g_Pos.w);
	gl_Position = vec4( pos1.x, pos1.y, 0.0, 1.0 );
	texcoord = tex1;
	EmitVertex();

	vec2 pos4 = vec2(g_Pos.x, g_Pos.y);
	pos4 = pos4 * 2 - 1;
	vec2 tex4 = vec2(g_Pos.z, g_Pos.w);
	gl_Position = vec4( pos4.x, pos4.y, 0.0, 1.0 );
	texcoord = tex4;
	EmitVertex();

	vec2 pos2 = vec2(g_Pos.x + g_Size.x, g_Pos.y - g_Size.y);
	pos2 = pos2 * 2 - 1;
	vec2 tex2 = vec2(g_Pos.z + g_Size.z, g_Pos.w - g_Size.w);
	gl_Position = vec4( pos2.x, pos2.y, 0.0, 1.0 );
	texcoord = tex2;
	EmitVertex();

	vec2 pos3 = vec2(g_Pos.x, g_Pos.y - g_Size.y);
	pos3 = pos3 * 2 - 1;
	vec2 tex3 = vec2(g_Pos.z, g_Pos.w - g_Size.w);
	gl_Position = vec4( pos3.x, pos3.y, 0.0, 1.0 );
	texcoord = tex3;
	EmitVertex();
	EndPrimitive();
}
#end_shader
#new_shader fragment
#version 430
in vec2 texcoord;
out vec4 FragmentColor;
layout(binding = 0)uniform sampler2D g_Texture;
uniform vec4 g_Color;
uniform bool g_GreyScale;
void main(){
	if(g_GreyScale){
		vec4 c = texture(g_Texture, texcoord);
		FragmentColor = vec4(c.r,c.r,c.r,c.a);
	}
	else{
		FragmentColor = texture(g_Texture, texcoord) * g_Color;
	}
}

#end_shader
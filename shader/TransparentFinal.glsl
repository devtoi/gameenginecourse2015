#new_shader vertex
#version 330
void main()
{}
#end_shader
#new_shader geometry
#version 330 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;
out vec2 texcoord;
void main()
{
	gl_Position = vec4( 1.0, 1.0, 0.0, 1.0 );
	texcoord = vec2( 1.0, 1.0 );
	EmitVertex();
	gl_Position = vec4(-1.0, 1.0, 0.0, 1.0 );
	texcoord = vec2( 0.0, 1.0 );
	EmitVertex();
	gl_Position = vec4( 1.0,-1.0, 0.0, 1.0 );
	texcoord = vec2( 1.0, 0.0 );
	EmitVertex();
	gl_Position = vec4(-1.0,-1.0, 0.0, 1.0 );
	texcoord = vec2( 0.0, 0.0 );
	EmitVertex();
	EndPrimitive();
}
#end_shader
#new_shader fragment
#version 330
in vec2 texcoord;
out vec4 FinalColor;

uniform sampler2D acumTexture;
uniform sampler2D revealageTexture;

void main(){
	vec4 acum = texture(acumTexture, texcoord);
	float r = texture(revealageTexture, texcoord).r;
	FinalColor = vec4(acum.rgb / clamp(acum.a, 1e-4, 5e4), r);
}
#end_shader
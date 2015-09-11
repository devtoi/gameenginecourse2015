#new_shader vertex
#version 330
layout(location = 0)in vec4 Position;
layout(location = 1)in vec4 VelocityTTL;

out vec4 VelTTL;

void main(){
	gl_Position = Position;
	VelTTL = VelocityTTL;
}
#end_shader

#new_shader fragment
#version 330

in vec4 VelTTL;

out vec4 color;
void main(){
	color = vec4(VelTTL.xyz * 0.5 + 0.5,1);
}
#end_shader
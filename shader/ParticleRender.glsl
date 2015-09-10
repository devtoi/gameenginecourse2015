#new_shader vertex
#version 330
layout(location = 0)in vec4 Position;

void main(){
	gl_Position = Position;
}
#end_shader

#new_shader fragment
#version 330

out vec4 color;
void main(){
	color = vec4(1, 0, 1, 1);
}
#end_shader
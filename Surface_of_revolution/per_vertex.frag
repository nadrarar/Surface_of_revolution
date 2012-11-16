/*	Perry Kivolowitz
	Computer Sciences Department - University of Wisconsin.
*/

#version 400

in vec3 color;
in vec3 back_color;
layout (location = 0) out vec4 fragment_color;

void main(void)
{
		fragment_color = vec4(gl_FrontFacing ? color : back_color, 1.0);
}

#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Colour;

//Matrix Uniforms as specified with glUniformMatrix4fv
//uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
//uniform mat4 NormalMatrix;

//gl_PointSize = 100.0;

out vec3 pass_Colour;

void main(void)
{
    gl_Position = (ProjectionMatrix  * ViewMatrix ) * vec4(in_Position, 1.0);
//    gl_PointSize.xyz = in_Position;
//    gl_Position.w = 1.0;
	pass_Colour = in_Colour;
}

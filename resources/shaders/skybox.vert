

#version 150
#extension GL_ARB_explicit_attrib_location : require

layout (std140) uniform CameraBlock {
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};

// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_Texcoord;
layout(location = 3) in vec3 in_Tangent;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;


out vec2 pass_Texcoord;

void main(void)
{
    //cast view matrix to get only rotation part
    mat4 camAngle = mat4( mat3(ViewMatrix));
    
	gl_Position = (ProjectionMatrix * camAngle * ModelMatrix ) * vec4(in_Position, 1.0);

    pass_Texcoord = in_Texcoord;
    
    
    
}

//with additions from https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model

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
//uniform mat4 ViewMatrix;
//uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;
//assignment 3:
uniform vec3 SunPosition;
uniform vec3 DiffuseColour;
uniform int ShaderMode;




out vec3 pass_Normal;
out vec3 pass_VertexViewPosition;
out vec3 pass_LightSourceViewPosition;
out vec3 pass_diffuseColour;
out float pass_ShaderMode;
//assignment 4:
out vec2 pass_Texcoord;
//ass4 extn
out vec3 pass_Tangent;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;

    //assignment3
    pass_VertexViewPosition = vec3(ViewMatrix * ModelMatrix * vec4(in_Position, 1.0));

    pass_LightSourceViewPosition = SunPosition;
    pass_diffuseColour = DiffuseColour;
    
    //cast to float because fragment shader doesn't accept an int
    pass_ShaderMode = float(ShaderMode);
    
    //assignment4
    pass_Texcoord = in_Texcoord;
    
    //assignment 4 extension - multiply tangent into view space
    pass_Tangent = vec3(vec4(in_Tangent, 1.0) * NormalMatrix);
    
    
}

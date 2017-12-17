#version 150

//in  vec3 pass_Colour;
in vec2 pass_TexCoord;

uniform sampler2D TexID;
uniform int Color;

out vec4 out_Color;

void main() {
    
    out_Color = vec4(texture(TexID, pass_TexCoord));

}

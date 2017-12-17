#version 150

//in  vec3 pass_Colour;
in vec2 pass_TexCoord;

uniform sampler2D TexSampler;

out vec4 out_Color;

void main() {
    
   // out_Color = vec4(texture(TexSampler, pass_TexCoord));
    
   out_Color = vec4(1.0, 1.0, 1.0, 1.0f);
}

#version 150

//in  vec3 pass_Colour;
in vec2 pass_TexCoord;

uniform sampler2D TexID;
//uniform int Color;

out vec4 out_Color;

void main() {
    
//    if (pass_TexCoord.x > 0.95 || pass_TexCoord.y > 0.95) {
//        out_Color = vec4(1.0, 1.0, 1.0, 1.0);
//    }
//    else
//        out_Color = vec4(0.0, 0.0, 0.0, 1.0);
    
    vec2 newCoord = vec2(pass_TexCoord.x / 4.0, pass_TexCoord.y / 4.0);
    
    out_Color = vec4(texture(TexID, newCoord));

}

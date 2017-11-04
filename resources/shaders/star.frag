#version 150

in  vec3 pass_Colour;
out vec4 out_Color;

void main() {
    
  out_Color = vec4(pass_Colour, 1.0f);
}

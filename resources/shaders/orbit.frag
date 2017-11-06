#version 150

//in  vec3 pass_Colour;
out vec4 out_Color;

//send constant colour to draw orbit

void main() {
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
    out_Color = vec4(1.0, 1.0, 1.0, 1.0);
}

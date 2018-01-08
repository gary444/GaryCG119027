#version 150

in vec2 pass_Texcoord;

uniform sampler2D ColourTex;

out vec4 out_Color;

void main() {
    
    //adjust co-ordinates to better fit over planets
    //something weird going on with texture mapping...
    float y = (pass_Texcoord.y + 1.0) * 0.5;
    float x = (pass_Texcoord.x + 1.0) * 0.25;
    vec2 newCoord = vec2(x, y);

    vec4 colour = texture(ColourTex, newCoord);
    out_Color = colour * 0.5;
    
}





#version 150

//in  vec3 pass_Colour;
in vec2 pass_TexCoord;
in vec4 gl_FragCoord;

//assigbnment 5
uniform sampler2D TexID;
uniform int PP_FLAG;

out vec4 out_Color;

//define binary flags
#define GREYSCALE 1
#define H_MIRRORED 2
#define V_MIRRORED 4
#define BLUR 8

void main() {
    
//    if (pass_TexCoord.x > 0.95 || pass_TexCoord.y > 0.95) {
//        out_Color = vec4(1.0, 1.0, 1.0, 1.0);
//    }
//    else
//        out_Color = vec4(0.0, 0.0, 0.0, 1.0);
    
    
    vec2 newCoord = vec2(pass_TexCoord.x / 4.0, pass_TexCoord.y / 4.0);
    
    //horizontal mirroring
    if ( (PP_FLAG & H_MIRRORED ) == H_MIRRORED ) {
        //flip texture y(v) co-ordinate to horizontally mirror
        newCoord = vec2(newCoord.x, 1.0 - newCoord.y);
    }
    
    //vertical mirroring
    if ( (PP_FLAG & V_MIRRORED ) == V_MIRRORED ) {
        //flip texture x(u) co-ordinate to vertically mirror
        newCoord = vec2(1.0 - newCoord.x, newCoord.y);
    }
    
    
    out_Color = texture(TexID, newCoord);
    
    //greyscale
    if ( (PP_FLAG & GREYSCALE ) == GREYSCALE ) {
        //calculate luminescance preserving grey value and assign to output
        vec3 greyVec = vec3(0.2126, 0.7152, 0.0722) * vec3(out_Color);
        float grey = dot(greyVec, vec3(1.0, 1.0, 1.0));
        out_Color = vec4(grey, grey, grey, 1.0);
    }
    
    //blurring
    if ( (PP_FLAG & BLUR ) == BLUR ) {
        
        //calc pixel size as a vec2 (x and y)
        vec2 pixelSize = newCoord / vec2(gl_FragCoord);
        
        //sample colour with adjusted co-ordinates for 9 pixel kernel
        vec3 sumColour = vec3(out_Color) * 0.25;//centre
        
        sumColour += 0.125 * vec3(texture(TexID, vec2(newCoord.x - pixelSize.x, newCoord.y)));//left
        sumColour += 0.125 * vec3(texture(TexID, vec2(newCoord.x + pixelSize.x, newCoord.y)));//right
        sumColour += 0.125 * vec3(texture(TexID, vec2(newCoord.x, newCoord.y + pixelSize.y)));//top
        sumColour += 0.125 * vec3(texture(TexID, vec2(newCoord.x, newCoord.y - pixelSize.y)));//bottom
        
        
        sumColour += 0.0625 * vec3(texture(TexID, vec2(newCoord.x - pixelSize.x, newCoord.y + pixelSize.y)));//left top
        sumColour += 0.0625 * vec3(texture(TexID, vec2(newCoord.x - pixelSize.x, newCoord.y - pixelSize.y)));//left bottom
        sumColour += 0.0625 * vec3(texture(TexID, vec2(newCoord.x + pixelSize.x, newCoord.y + pixelSize.y)));//right top
        sumColour += 0.0625 * vec3(texture(TexID, vec2(newCoord.x + pixelSize.x, newCoord.y - pixelSize.y)));//right bottom
        
        
        //sum together and output
        out_Color = vec4(sumColour, 1.0);
        
    }
    
    

}

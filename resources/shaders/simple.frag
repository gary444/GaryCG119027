#version 150

in vec3 pass_Normal;
in vec3 pass_VertexViewPosition;
in vec3 pass_LightSourceViewPosition;
in vec3 pass_diffuseColour;

out vec4 out_Color;

float ambientK = 0.5;
//double ambientI;
float diffuseK = 0.5;
float specularK = 10.0;
vec3 specularColour = vec3(1.0, 1.0, 1.0);

void main() {
    
    
    vec3 normal = normalize(pass_Normal);
    vec3 lightDir = normalize(pass_LightSourceViewPosition - pass_VertexViewPosition);
    vec3 viewDir = normalize(pass_VertexViewPosition);
    

    
    //vec3 halfwayVector = normalize(viewDir + pass_LightSourceViewPosition);

    //ambient light
    vec3 ambient = ambientK * pass_diffuseColour;
    
    //calc diffuse light
    vec3 diffuse = pass_diffuseColour * diffuseK * (normal * lightDir);
    

    
    
    
    
    out_Color = vec4(ambient + diffuse, 1.0);
    
   //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}

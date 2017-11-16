#version 150

//with additions from https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model


in vec3 pass_Normal;
in vec3 pass_VertexViewPosition;
in vec3 pass_LightSourceViewPosition;
in vec3 pass_diffuseColour;

out vec4 out_Color;

float ambientK = 0.3;
//double ambientI;
float diffuseK = 0.6;
float specularK = 10.0;
vec3 specularColour = vec3(1.0, 1.0, 1.0);

void main() {
    
    
    vec3 normal = normalize(pass_Normal);
    vec3 lightDir = normalize(pass_LightSourceViewPosition - pass_VertexViewPosition);
    
    

    

    //ambient light
    vec3 ambient = ambientK * pass_diffuseColour;
    
    //calc diffuse light
    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;
    
    if (lambertian > 0.0) {
        
        vec3 viewDir = normalize(-pass_VertexViewPosition);
        vec3 halfwayVector = normalize(viewDir + lightDir);
        
        float specAngle = max(dot(halfwayVector, normal), 0.0);
        
        specular = pow(specAngle, specularK);
    }
    
    vec3 diffuse = lambertian * pass_diffuseColour;
    
    //specular
    
    vec3 specularLight = specularColour * specular;
    
    

    
    
    
    
    out_Color = vec4(ambient + diffuse + specularLight, 1.0);
    
   //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
}

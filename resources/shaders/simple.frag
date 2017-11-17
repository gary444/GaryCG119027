#version 150

//with additions from https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model


in vec3 pass_Normal;
in vec3 pass_VertexViewPosition;
in vec3 pass_LightSourceViewPosition;
in vec3 pass_diffuseColour;
in float pass_ShaderMode;

out vec4 out_Color;

float ambientK = 0.1;
float diffuseK = 0.8;
float specularK = 50.0;
vec3 specularColour = vec3(1.0, 1.0, 1.0);


void main() {
    
    if (pass_ShaderMode == 1.0) {
        ambientK = 0.1;
    }
    else {
        ambientK = 1.0;
    }
    

    
    vec3 normal = normalize(pass_Normal);
    vec3 lightDir = normalize(pass_LightSourceViewPosition - pass_VertexViewPosition);
    
    

    

    //ambient light
    vec3 ambient = ambientK * pass_diffuseColour;
    
    //calc diffuse light
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lambertian * pass_diffuseColour * diffuseK;
    
    //specular
    float specularIntensity = 0.0;
    if (lambertian > 0.0) {
        
        
        vec3 viewDir = normalize(-pass_VertexViewPosition);
        vec3 halfwayVector = normalize(viewDir + lightDir);
        
        float specAngle = max(dot(halfwayVector, normal), 0.0);
        //float specAngle = dot(halfwayVector, normal);
        
        specularIntensity = pow(specAngle, specularK);
    }
    
    vec3 specular = specularColour * specularIntensity;
    
    
    out_Color = vec4(ambient + diffuse + specular, 1.0);
    //out_Color = vec4(ambient + diffuse , 1.0);
    
}

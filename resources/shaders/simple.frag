#version 150

//with additions from https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model


in vec3 pass_Normal;
in vec3 pass_VertexViewPosition;
in vec3 pass_LightSourceViewPosition;
in vec3 pass_diffuseColour;
in float pass_ShaderMode;

out vec4 out_Color;

float ambientK = 0.3;
float diffuseK = 0.8;
float specularK = 3.0;
vec3 specularColour = vec3(1.0, 1.0, 1.0);
vec3 outlineColour = vec3(0.850, 0.968, 0.956);

//function for clamping colours to a discrete number of shades
//ref from: http://sunandblackcat.com/tipFullView.php?l=eng&topicid=15
float cellify(float inputIntensity, int numShades){
    
    //float outI = 1.0;
    float outI = ceil(inputIntensity * numShades) / numShades;
    
    return outI;
}

void main() {

    

    
    vec3 normal = normalize(pass_Normal);
    vec3 lightDir = normalize(pass_LightSourceViewPosition - pass_VertexViewPosition);
    
    

    

    //ambient light
    vec3 ambient = ambientK * pass_diffuseColour;
    
    //calc diffuse light
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lambertian * pass_diffuseColour * diffuseK;
    
    vec3 viewDir = normalize(-pass_VertexViewPosition);
    
    //specular
    float specularIntensity = 0.0;
    if (lambertian > 0.0) {
        
        
        
        vec3 halfwayVector = normalize(viewDir + lightDir);
        
        float specAngle = max(dot(halfwayVector, normal), 0.0);
        //float specAngle = dot(halfwayVector, normal);
        
        specularIntensity = pow(specAngle, specularK);
    }
    
    vec3 specular = specularColour * specularIntensity;
    
    
    out_Color = vec4(ambient + diffuse + specular, 1.0);
    
    
    //cel shading=============
    if (pass_ShaderMode == 1.0) {
        
        //calc. cos of angle between normal and view direction
        float viewAngleCosine = dot(normal, viewDir);
        
        //cosine decreases as angle approaches 90 degrees
        //if cos value is less than x, colour to outline colour
        if (viewAngleCosine < 0.1) {
            out_Color = vec4(outlineColour, 1.0);
        }
        else {
            //split into rgb
            float rI = out_Color.x;
            float gI = out_Color.y;
            float bI = out_Color.z;
            
            //cellify each element
            rI = cellify(rI, 4);
            gI = cellify(gI, 4);
            bI = cellify(bI, 4);
            
            
            out_Color = vec4(rI, gI, bI, 1.0);
        }
        
        
    }
  
}





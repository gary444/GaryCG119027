#version 150

//with additions from https://en.wikipedia.org/wiki/Blinn%E2%80%93Phong_shading_model



in vec3 pass_Normal;
in vec3 pass_VertexViewPosition;
in vec3 pass_LightSourceViewPosition;
in vec3 pass_diffuseColour;
in float pass_ShaderMode;
in vec2 pass_Texcoord;
in vec3 pass_Tangent;

//assignment 4
uniform sampler2D ColourTex;
//assignment 4 extn
uniform sampler2D NormalMapIndex;
uniform bool UseBumpMap;

out vec4 out_Color;

//layout(location = 0) out vec3 out_Color;

float ambientK = 0.3;
float diffuseK = 0.8;
float specularK = 0.2;
float glossiness = 3.0; // low glossiness as planets are not that shiny!
vec3 specularColour = vec3(1.0, 1.0, 1.0);
vec3 outlineColour = vec3(0.850, 0.968, 0.956);


void main() {
    
    //out_Color = vec3(1.0, 1.0, 1.0);
    
    //adjust co-ordinates to better fit over planets
    vec2 newCoord = vec2((pass_Texcoord.x * 0.25 - 0.5) * 1.0, (pass_Texcoord.y * 0.5) + 0.5);
    
    vec3 baseColour = vec3(texture(ColourTex, newCoord));
    //vec3 baseColour = pass_diffuseColour;
    
    //assignment4 extn--------------------------------------------------
    //normal mapping
    
    vec3 normal;
    
    if (UseBumpMap) {
        
        vec3 bumpyNormal = normalize(vec3(texture(NormalMapIndex, newCoord)));
        //translate to tangent space by scaling
        bumpyNormal = vec3(bumpyNormal.x * 2.0 - 1.0, bumpyNormal.y * 2.0 - 1.0, bumpyNormal.z);
        
        normal = normalize(pass_Normal);
        vec3 tangent = normalize(pass_Tangent);
        
        //normal = pass_Normal;
        //tangent = pass_Tangent;
        
        //calculate bitangent using cross product of N and T
        vec3 bitangent = cross(normal, tangent);
        //create matrix
        mat3 tangentMatrix = transpose(mat3(tangent,bitangent,normal));
        bumpyNormal = bumpyNormal * tangentMatrix;
        
        normal = normalize(bumpyNormal);
        
    }
    else {
        normal = normalize(pass_Normal);
    }
    
    
    
    //---------------------------------------------------------------
    
    
    //create vector for dorection of 'light' - from origin to vertex positions in view space
    vec3 lightDir = normalize(pass_LightSourceViewPosition - pass_VertexViewPosition);
    
    //calculate ambient light
    vec3 ambient = ambientK * baseColour;
    
    //calculate diffuse light
    //lambertian is cos of angle between light and normal
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lambertian * baseColour * diffuseK;
    
    vec3 viewDir = normalize(-pass_VertexViewPosition);
    
    //specular
    float specularIntensity = 0.0;
    if (lambertian > 0.0) {
        
        vec3 halfwayVector = normalize(viewDir + lightDir);
        float specAngle = max(dot(halfwayVector, normal), 0.0);
        specularIntensity = pow(specAngle, glossiness);
    }
    vec3 specular = specularK * specularColour * specularIntensity;
    
    //combine specular, diffuse and ambient
    out_Color = vec4(ambient + diffuse + specular, 1.0);
    //out_Color = ambient + diffuse + specular;
    
    
    //cel shading=============
    if (pass_ShaderMode == 2.0) {
        
        //calc. cos of angle between normal and view direction
        float viewAngleCosine = dot(normal, viewDir);
        
        //cosine decreases as angle approaches 90 degrees
        //if cos value is less than x, colour to outline colour
        if (viewAngleCosine < 0.3) {
            out_Color = vec4(outlineColour, 1.0);
            //out_Color = outlineColour;
            
        }
        //else if pixel is not on outline....
        //ref from: http://sunandblackcat.com/tipFullView.php?l=eng&topicid=15
        else {

            //apply cel shading
            out_Color = ceil(out_Color * 4) / 4;
            //out_Color = vec3(ceil(out_Color * 4) / 4);
            
        }
    }
}





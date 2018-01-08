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

float ambientK = 0.3;
float diffuseK = 0.8;
float specularK = 0.2;
float glossiness = 3.0; // low glossiness as planets are not that shiny!
vec3 specularColour = vec3(1.0, 1.0, 1.0);
vec3 outlineColour = vec3(0.850, 0.968, 0.956);


void main() {
    
    
    
    //adjust co-ordinates to better fit over planets
    //something weird going on with texture mapping...
    float y = (pass_Texcoord.y + 1.0) * 0.5;
    float x = (pass_Texcoord.x + 1.0) * 0.25;
    vec2 newCoord = vec2(x, y);
//
    vec4 colour = texture(ColourTex, newCoord);
    //vec3 baseColour = pass_diffuseColour;
    
    //assignment4 extn--------------------------------------------------
    //normal mapping
    
    
    
    vec3 normal = normalize(pass_Normal);
    
    if (UseBumpMap) {
        
        vec3 bumpyNormal = normalize(vec3(texture(NormalMapIndex, pass_Texcoord)));
        //translate to tangent space by scaling
        bumpyNormal = vec3((bumpyNormal.x - 0.5) * 2.0, (bumpyNormal.y - 0.5) * 2.0, bumpyNormal.z);

        vec3 tangent = normalize(pass_Tangent);

        //calculate bitangent using cross product of N and T
        vec3 bitangent = cross(normal, tangent);
        //create matrix
        mat3 tangentMatrix = transpose(mat3(tangent,bitangent,normal));
        bumpyNormal = tangentMatrix * bumpyNormal;

        normal = normalize(bumpyNormal);
        
    }
    
    
    
    
    //---------------------------------------------------------------
    
    
    //create vector for dorection of 'light' - from origin to vertex positions in view space
    vec3 lightDir = normalize(pass_LightSourceViewPosition - pass_VertexViewPosition);
    vec3 viewDir = normalize(-pass_VertexViewPosition);
    
    vec3 baseColor = vec3(colour);
    
    //calculate ambient light
    vec3 ambient = ambientK * baseColor;
    
    
    
    //calculate diffuse light
    //lambertian is cos of angle between light and normal
    float lambertian = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = lambertian * baseColor * diffuseK;
    
    
    
    //specular
    float specularIntensity = 0.0;
//    if (lambertian > 0.0) {
//
//
//    }
    
    vec3 halfwayVector = normalize(viewDir + lightDir);
    float specAngle = max(dot(halfwayVector, normal), 0.0);
    specularIntensity = pow(specAngle, glossiness);
    
    vec3 specular = specularK * specularColour * specularIntensity;
    
    //combine specular, diffuse and ambient
    out_Color = vec4(ambient + diffuse + specular, 1.0);
    
    
    //cel shading=============
    if (pass_ShaderMode == 2.0) {
        
        //calc. cos of angle between normal and view direction
        float viewAngleCosine = dot(normal, viewDir);
        
        //cosine decreases as angle approaches 90 degrees
        //if cos value is less than x, colour to outline colour
        if (viewAngleCosine < 0.3) {
            out_Color = vec4(outlineColour, 1.0);
            
        }
        //else if pixel is not on outline....
        //ref from: http://sunandblackcat.com/tipFullView.php?l=eng&topicid=15
        else {

            //apply cel shading
            out_Color = ceil(out_Color * 4) / 4;
            
        }
    }
    
    
    //out_Color = vec4(1.0, 1.0, 1.0, 1.0);
}





#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <iostream>

//#define NUM_PLANETS 10
#define NUM_STARS 1000
#define NUM_POINTS_ON_ORBIT 100


model star_model{};
model planet_model{};
model orbit_model{};
model screenquad_model{};
//model skybox_model{};

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
, planet_object{}, star_object{}, orbit_object{}, skybox_object{}, screenquad_object{}
{
    //set states
    //motionOn = true;
    orbitsOn = true;
    starsOn = false;
    
    
  
    //fill star buffer here with random position and colours
    star_object.num_elements = NUM_STARS;
    for (int i = 0; i < star_object.num_elements; i++){
        
        starBuffer.push_back(randPos());
        starBuffer.push_back(randPos());
        starBuffer.push_back(randPos());
        
        starBuffer.push_back(randCol());
        starBuffer.push_back(randCol());
        starBuffer.push_back(randCol());
    }
    
    
    //fill orbit buffer
    orbit_object.num_elements = NUM_POINTS_ON_ORBIT;
    fillOrbits();

    //load textures
    loadAllTextures();
    
    //load normal map
    loadNormalMap(GL_TEXTURE12);
    
    //initialise frame buffers
    //setupOffscreenRendering();
    
    
    //load models
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD | model::TANGENT);
    
    //star model uses 'normal' space for colour attributes - both use 3 floats
    star_model = {starBuffer, model::POSITION | model::NORMAL};
    //only use position for orbits
    orbit_model = {orbitBuffer, model::POSITION};
    screenquad_model = {screenQuad, model::POSITION};

    
    //set starting view
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, 10.0f });
    m_view_transform = glm::rotate(m_view_transform, glm::radians(-10.0f), glm::fvec3{ 1.0f, 0.0f, 0.0f });
    
    
    //initialisations
    initializeGeometry();
    initializeShaderPrograms();
  
}

void ApplicationSolar::setupOffscreenRendering(){
    
    //get screen size
    GLint viewportData[4];
    glGetIntegerv(GL_VIEWPORT, viewportData);
    
    
    
    //create texture
    //GLuint drawBufferTexture;
    //switch active texture
    //glActiveTexture(GL_TEXTURE13);//needed?
    //generate texture object
    glGenTextures(1, &drawBufferTexture);
    //bind texture to 2D texture binding point of active unit
    glBindTexture(GL_TEXTURE_2D, drawBufferTexture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportData[2], viewportData[3], 0,
                 GL_RGB, GL_FLOAT, 0);
    //define sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    
    //create render buffer (for depth buffer)
    //GLuint rb_handle;
    glGenRenderbuffers(1, &rb_handle);
    glBindRenderbuffer(GL_RENDERBUFFER, rb_handle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, viewportData[2], viewportData[3]);
    
    
    //setup FBO
    GLuint fbo_handle = 1;
    glGenFramebuffers(1, &fbo_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);
    //define attachments
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, drawBufferTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, rb_handle);
    
    //create draw buffers
    GLenum draw_buffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, draw_buffers);
    
    //check validity
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE){
        throw std::logic_error("framebuffer not correctly initialised");
    }
    
    
    //set to render to texture (via FBO)
    //glBindFramebuffer(GL_FRAMEBUFFER, drawBufferTexture);
    
    
}

//loads a normal map
void ApplicationSolar::loadNormalMap(GLenum targetTextureUnit){
    
    //load texture from file for this planet
    pixel_data newTexture = texture_loader::file(m_resource_path + "normal_maps/earth_bumpmap.png");
    
    GLuint TEX_POSITION = 11;
    
    //set ID
    texBufferIDs[TEX_POSITION] = TEX_POSITION;
    
    //switch active texture
    glActiveTexture(targetTextureUnit);
    //generate texture object
    glGenTextures(1, &texBufferIDs[TEX_POSITION]);
    //bind texture to 2D texture binding point of active unit
    glBindTexture(GL_TEXTURE_2D, texBufferIDs[TEX_POSITION]);
    
    //define sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //define texture data and texture format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)newTexture.width, (GLsizei)newTexture.height, 0, newTexture.channels, newTexture.channel_type, newTexture.ptr());
}

//cycle through all planets and load relevant textures
void ApplicationSolar::loadAllTextures(){
    
    //holds texture info
    pixel_data newTexture;
    
    //for each planet
    int i;
    for (i = 0; i < (sizeof(planets) / sizeof(planets[0])); i++) {

        loadTexture(planets[i].name, i);
        
    }

    //load texture for skybox manually
    newTexture = texture_loader::file(m_resource_path + "textures/stars.png");
    
    //set ID
    texBufferIDs[i + 1] = (GLuint)i + 1;
    
    //switch active texture
    glActiveTexture((GLenum) (GL_TEXTURE10));
    //generate texture object
    glGenTextures(1, &texBufferIDs[i + 1]);
    //bind texture to 2D texture binding point of active unit
    glBindTexture(GL_TEXTURE_2D, texBufferIDs[i + 1]);
    
    //define sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //define texture data and texture format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)newTexture.width, (GLsizei)newTexture.height, 0, newTexture.channels, newTexture.channel_type, newTexture.ptr());
    
    
}

//loads a single texture given a file name and a texture index
void ApplicationSolar::loadTexture(std::string name, GLuint texId){
    
    //load texture from file for this planet
    pixel_data newTexture = texture_loader::file(m_resource_path + "textures/" + name + ".png");
    
    //set ID
    texBufferIDs[texId] = (GLuint)texId;
    
    //switch active texture
    glActiveTexture((GLenum) (GL_TEXTURE0 + texId));
    //generate texture object
    glGenTextures(1, &texBufferIDs[texId]);
    //bind texture to 2D texture binding point of active unit
    glBindTexture(GL_TEXTURE_2D, texBufferIDs[texId]);
    
    //define sampling parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //define texture data and texture format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)newTexture.width, (GLsizei)newTexture.height, 0, newTexture.channels, newTexture.channel_type, newTexture.ptr());
    
}




void ApplicationSolar::fillOrbits(){
    
    //FOR each planet/moon
    // calculate values and add append to orbit points array
    
    //cycle through planet array, not including sun
    for (int i = 0; i < (sizeof(planets) / sizeof(planets[0])); i++) {
        
        planet thisPlanet = planets[i];
        
        int pointsPerOrbit = NUM_POINTS_ON_ORBIT;
        float increment = 2.0f * (float)M_PI / (float)pointsPerOrbit;
        float radius = thisPlanet.distToOrigin;
        float skew = thisPlanet.orbitSkew;
        
        
        for (float rad = 0.f; rad < (2.0f * M_PI); rad += increment){
            
            //x
            orbitBuffer.push_back(radius * cosf(rad));
            //y
            orbitBuffer.push_back(radius * -skew * cosf(rad));
            //z
            orbitBuffer.push_back(radius * sinf(rad));
            
        }
    }
    
}


void ApplicationSolar::render() const {
    
    
    
    
    
    
    //set background colour...hard coded to dark blue
    //glClearColor(0.031f, 0.043f, 0.231f, 1.0);
    
    //==================================================================
    //planets
    
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);
    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // cycle through members of planet array and apply model matrix
    for (int i = 0; i < (sizeof(planets) / sizeof(planets[0])); i++) {
        
        planet testPlanet = planets[i];
        
        // don't render if 'planet' is a moon
        if (testPlanet.isMoon == false) {
            
            // upload planet
            upload_planet_transforms(i);
        }
        
    }
    
    //==================================================================
    //stars
    
    if (starsOn) {
        upload_stars();
    }
    
    upload_skybox();
    
    //==================================================================
    //orbit(s)
    if (orbitsOn) {
        upload_Orbits();
    }
    
    upload_quad();
    
    
}

//upload screen quad for assignment 5
void ApplicationSolar::upload_quad() const{
    
    glUseProgram(m_shaders.at("quad").handle);

    glActiveTexture(GL_TEXTURE13);
    glUniform1i(m_shaders.at("quad").u_locs.at("Texture"), drawBufferTexture);
    //glActiveTexture(GL_TEXTURE4);
    //glUniform1i(m_shaders.at("quad").u_locs.at("Texture"), 4);
    
    glBindVertexArray(screenquad_object.vertex_AO);

    glDrawArrays(screenquad_object.draw_mode, 0, screenquad_object.num_elements);
 
}

//assignment 2 extension - draw planet's orbit(s)
void ApplicationSolar::upload_Orbits() const{
    
    
    //bind shader and array
    glUseProgram(m_shaders.at("orbit").handle);
    glBindVertexArray(orbit_object.vertex_AO);
    
    //num of orbits = num of planets
    int numOrbits = sizeof(planets) / sizeof(planets[0]);
    
    //cycle through array of planets from 1 - planet 0 (sun) doesnt need an orbit
    for (int i = 1; i < numOrbits; i++) {
        
        //if planet is not a moon...
        if (planets[i].isMoon == false) {
            
            //don't move shader model matrix - orbit is a static loop
            glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ModelMatrix"),
                               1, GL_FALSE, glm::value_ptr(glm::fmat4{}));
        
        
            //draw orbit
            glDrawArrays(orbit_object.draw_mode, i * orbit_object.num_elements, orbit_object.num_elements);
            
            //if this planet has a moon
            if (planets[i].hasMoonAtIndex > 0) {
                
                planet earth = planets[i];
                
                //create rotated and translated matrix with planet information
                glm::fmat4 m_earth;
                
                
               
                m_earth = glm::rotate(glm::fmat4{}, float(glfwGetTime() * earth.rotationSpeed), glm::fvec3{ 0.0f, 1.0f, 0.0f });
                m_earth = glm::translate(m_earth, glm::fvec3{ 0.0f, 0.0f, earth.distToOrigin });
                m_earth = glm::rotate(m_earth, float (M_PI / 2.f), glm::fvec3{ 0.0f, 0.0f, 1.0f });
                
                
                
                glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ModelMatrix"),
                                   1, GL_FALSE, glm::value_ptr(m_earth));

                
                //draw orbit
                glDrawArrays(orbit_object.draw_mode, earth.hasMoonAtIndex * 100, orbit_object.num_elements);
                
            }
        }
    }
}

//function added assignment 2
void ApplicationSolar::upload_stars() const{
    
    // order from lecture 3, slide 15
    
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("star").handle);
    // bind the VAO to draw
    glBindVertexArray(star_object.vertex_AO);
    //draw all
    glDrawArrays(GL_POINTS, 0, star_object.num_elements);
    
    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);
    
    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
    
    
}

//render skybox - use planet model
void ApplicationSolar::upload_skybox() const{
    
    glUseProgram(m_shaders.at("planet").handle);
    
    // use rotation speed and planet skew to create planet's orbit
    glm::fmat4 model_matrix;
    
    // scale planet according to planet size
    float skyboxSize = 80.f;
    model_matrix = glm::scale(model_matrix, glm::fvec3{skyboxSize, skyboxSize, skyboxSize});
    
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));
    
    //extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(normal_matrix));
    
    
    //tex index is last member of texIds list
    GLuint textureIndex = texBufferIDs[sizeof(planets) / sizeof(planets[0])];
    
    glActiveTexture(GL_TEXTURE10);
    //glActiveTexture((GLenum)(GL_TEXTURE0 + textureIndex));
    //glBindTexture(GL_TEXTURE_2D, texBufferIDs[planetIndex]);
    glUniform1i(m_shaders.at("planet").u_locs.at("ColourTex"), 10);
    
    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);
    
    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
    
    
}

// added function assignment 1
void ApplicationSolar::upload_planet_transforms(int planetIndex) const
{
    
    planet planetToDisplay = planets[planetIndex];
    
    // use rotation speed and planet skew to create planet's orbit
    glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * planetToDisplay.rotationSpeed), glm::fvec3{ planetToDisplay.orbitSkew, 1.0f, 0.0f });
    
    // use planet.distToOrigin to translate planet away from origin
    model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0f, 0.0f, planetToDisplay.distToOrigin });

    
    // if this planet has a moon, add a moon using planet's location as a starting point
    if (planetToDisplay.hasMoonAtIndex > 0) {
        
        planet moon = planets[planetToDisplay.hasMoonAtIndex];
        
        //rotate at moon's speed
        glm::fmat4 model_matrix2 = glm::rotate(model_matrix, float(glfwGetTime() * moon.rotationSpeed), glm::fvec3{ 1.0f, 0.0f, 0.0f });
        
        //translate by moon's orbit
        model_matrix2 = glm::translate(model_matrix2, glm::fvec3{ 0.0f, 0.0f, moon.distToOrigin });
        
        // scale moon according to planet size
        model_matrix2 = glm::scale(model_matrix2, glm::fvec3{ moon.size, moon.size, moon.size});
        
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                           1, GL_FALSE, glm::value_ptr(model_matrix2));
        
        //extra matrix for normal transformation to keep them orthogonal to surface
        glm::fmat4 normal_matrix2 = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix2);
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                           1, GL_FALSE, glm::value_ptr(normal_matrix2));
        
        //texture------------
        glActiveTexture((GLenum)(GL_TEXTURE0 + planetToDisplay.hasMoonAtIndex));
        glUniform1i(m_shaders.at("planet").u_locs.at("ColourTex"), planetToDisplay.hasMoonAtIndex);
        
        // bind the VAO to draw
        glBindVertexArray(planet_object.vertex_AO);
        
        // draw bound vertex array using bound shader
        glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
        
    }
    
    // scale planet according to planet size
    model_matrix = glm::scale(model_matrix, glm::fvec3{ planetToDisplay.size, planetToDisplay.size, planetToDisplay.size });
    //add planet rotation on it's axis - const for all
    model_matrix = glm::rotate(model_matrix, float(glfwGetTime() * M_PI / 10), glm::fvec3{ 0.0f, 1.0f, 0.0f });
    
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));
    
    //extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(normal_matrix));
    
    //upload diffuse colour to shader (assignment 3)
    glm::vec3 planetColour = planetToDisplay.RGBColour;
    glUniform3fv(m_shaders.at("planet").u_locs.at("DiffuseColour"), 1, glm::value_ptr(planetColour));
    
    //this is to make the sun 'shine' - upload origin with 0.0 as w co-ord
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);
    glm::vec4 origin;
    if (planetToDisplay.name == "sun" ) {
        
        //create vec 4 of origin
        origin = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        
    }
    else {
        //create vec 4 of origin
        origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        
    }
    //multiply by view matrx, cast to vec3
    glm::vec3 sunPos(view_matrix * origin);
    //upload vec3 to planet shader
    
    glUniform3fv(m_shaders.at("planet").u_locs.at("SunPosition"), 1, glm::value_ptr(sunPos));

    
    //textures========================================================

    
    //get location of sampler uniform
    glActiveTexture(GL_TEXTURE0 + planetIndex);
    glUniform1i(m_shaders.at("planet").u_locs.at("ColourTex"), planetIndex);
    
    //normal map
    glUniform1i(m_shaders.at("planet").u_locs.at("NormalMapIndex"), (int)texBufferIDs[11]);
    
    if (planetToDisplay.name == "earth") {
        glUniform1b(m_shaders.at("planet").u_locs.at("UseBumpMap"), true);
    }
    else {
        glUniform1b(m_shaders.at("planet").u_locs.at("UseBumpMap"), false);
    }
    
    
    //end textures====================================================
    
    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);
    
    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
}

void ApplicationSolar::updateView() {
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);
    
    glUseProgram(m_shaders.at("planet").handle);
    
    //added for assignment 3 - upload sun's position to planet shader
    //create vec 4 of origin
    glm::vec4 origin(0.0f, 0.0f, 0.0f, 1.0f);
    //multiply by view matrx, cast to vec3
    glm::vec3 sunPos(view_matrix * origin);
    //upload vec3 to planet shader
    glUniform3fv(m_shaders.at("planet").u_locs.at("SunPosition"), 1, glm::value_ptr(sunPos));
    
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
    
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
    
    glUseProgram(m_shaders.at("orbit").handle);
    glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
    

    
}

void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
    
    glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    glUseProgram(m_shaders.at("orbit").handle);
    glUniformMatrix4fv(m_shaders.at("orbit").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    
    //update render buffer size
//    glBindRenderbuffer(GL_RENDERBUFFER, rb_handle);
//    //get screen size
//    GLint data[4];
//    glGetIntegerv(GL_VIEWPORT, data);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, data[2], data[3]);
    


}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();
  
  
  updateView();
  updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
	//move scene toward camera
    if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.3f});
        updateView();
    }
    // move scene away from camera
    else if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.3f});
        updateView();
    }
    // move scene right
    else if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{ -1.0f, 0.0f, 0.0f });
        updateView();
    }
    //move scene left
    else if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 1.0f, 0.0f, 0.0f });
        updateView();
    }
    
    //switch between shading modes - mode 1
    else if (key == GLFW_KEY_1 && action != GLFW_PRESS) {
        
        glUseProgram(m_shaders.at("planet").handle);
        glUniform1i(m_shaders.at("planet").u_locs.at("ShaderMode"), 1);
        
    }
    //switch between shading modes - mode 2
    else if (key == GLFW_KEY_2 && action != GLFW_PRESS) {
        glUseProgram(m_shaders.at("planet").handle);
        glUniform1i(m_shaders.at("planet").u_locs.at("ShaderMode"), 2);
        
    }
    //toggle motion with P key
//    else if (key == GLFW_KEY_P && action != GLFW_PRESS) {
//        motionOn = !motionOn;
//        
//    }
    else if (key == GLFW_KEY_O && action != GLFW_PRESS) {
        
        orbitsOn = !orbitsOn;
    }
    else if (key == GLFW_KEY_L && action != GLFW_PRESS) {
        
        starsOn = !starsOn;
    }
    
    

    

}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling

	float x = 0.0f;
	float y = 0.0f;

	//move factors - how much the camera moves in relation to mouse movement
	const float X_MOVE_FACTOR = 0.8f;
	const float Y_MOVE_FACTOR  = 0.2f;

	if (pos_x > 0)
		x = -X_MOVE_FACTOR;
	else if (pos_x < 0)
		x = X_MOVE_FACTOR;

	if (pos_y > 0)
		y = -Y_MOVE_FACTOR;
	else if (pos_y < 0)
		y = Y_MOVE_FACTOR;

	// rotate according to x and y movements of mouse
	m_view_transform = glm::rotate(m_view_transform, glm::radians(x), glm::fvec3{ 0.0f, 1.0f, 0.0f });
	m_view_transform = glm::rotate(m_view_transform, glm::radians(y), glm::fvec3{ 1.0f, 0.0f, 0.0f });

	updateView();
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
    
    // store shader program objects in container
    m_shaders.emplace("planet", shader_program{m_resource_path + "shaders/simple.vert",
                                           m_resource_path + "shaders/simple.frag"});
    // request uniform locations for shader program
    m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
    m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
    m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
    m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
    m_shaders.at("planet").u_locs["SunPosition"] = -1;
    m_shaders.at("planet").u_locs["DiffuseColour"] = -1;
    m_shaders.at("planet").u_locs["ShaderMode"] = -1;
    m_shaders.at("planet").u_locs["ColourTex"] = -1;
    m_shaders.at("planet").u_locs["NormalMapIndex"] = -1;
    m_shaders.at("planet").u_locs["UseBumpMap"] = -1;
    
    
    // add star shader here
    m_shaders.emplace("star", shader_program{m_resource_path + "shaders/star.vert",
        m_resource_path + "shaders/star.frag"});
    // request uniform locations for shader program
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
    
    // add orbit shader here
    m_shaders.emplace("orbit", shader_program{m_resource_path + "shaders/orbit.vert",
        m_resource_path + "shaders/orbit.frag"});
    // request uniform locations for shader program
    m_shaders.at("orbit").u_locs["ModelMatrix"] = -1;
    m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
    m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;
    
    //add screen quad shader
    m_shaders.emplace("quad", shader_program{m_resource_path + "shaders/quad.vert",
        m_resource_path + "shaders/quad.frag"});
    m_shaders.at("quad").u_locs["Texture"] = -1;
    
    
    
}

// load models
void ApplicationSolar::initializeGeometry() {
    
    //=================================================================
    // planet initialisation
    
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
    
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

    // activate third attribute on gpu - texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);
    
    // activate fourth attribute on gpu - tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, model::TANGENT.components, model::TANGENT.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TANGENT]);
    
    

   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());
    
    
  //======================================================================
  // star initialisation - lecture 4 slide 8
    
    
    //generate vertex array object
    glGenVertexArrays(1, &star_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(star_object.vertex_AO);
    
    // generate generic buffer
    glGenBuffers(1, &star_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * star_model.data.size(), star_model.data.data(), GL_STATIC_DRAW);
    
    
    
    // activate first attribute on gpu - position
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    
    //reinstate
    glVertexAttribPointer(0, star_model.POSITION.components, star_model.POSITION.type, GL_FALSE, star_model.vertex_bytes, star_model.offsets[model::POSITION]);

    
    // activate second attribute on gpu - colour
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with offset & stride of 3 floats
    
    glVertexAttribPointer(1, star_model.NORMAL.components, star_model.NORMAL.type, GL_FALSE, star_model.vertex_bytes, star_model.offsets[model::NORMAL]);
    
    
    // end star initialisation
  //======================================================================
    //orbit initialisation
    
    
    // generate vertex array object
    glGenVertexArrays(1, &orbit_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(orbit_object.vertex_AO);
    
    // generate generic buffer
    glGenBuffers(1, &orbit_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * orbit_model.data.size(), orbit_model.data.data(), GL_STATIC_DRAW);
    
    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, orbit_model.vertex_bytes, orbit_model.offsets[model::POSITION]);

    // store type of primitive to draw
    orbit_object.draw_mode = GL_LINE_LOOP;
    
    
    
    //======================================================================
    //screen quad
    
    // generate vertex array object
    glGenVertexArrays(1, &screenquad_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(screenquad_object.vertex_AO);
    
    // generate generic buffer
    glGenBuffers(1, &screenquad_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, screenquad_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * screenquad_model.data.size(), screenquad_model.data.data(), GL_STATIC_DRAW);
    
    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, screenquad_model.vertex_bytes, screenquad_model.offsets[model::POSITION]);
    
    
    // transfer number of indices to model object
    screenquad_object.num_elements = GLsizei(screenquad_model.data.size());
    screenquad_object.draw_mode = GL_TRIANGLE_STRIP;
    

    
}


//returns a position float, up to specified max value
//assigns random sign to value (-/+)
float ApplicationSolar::randPos(){
    
    float maxDistFromCentre = 80.0;
    
    //multiply and divide by factor of 10 to give more precision
    int range = int (maxDistFromCentre * 10);
    
    float out = float(rand() % range) / 10.f;
    
    //assign random sign - make value negative if rand num is even
    if (rand() % 2 == 0){
        out = -out;
    }
    
    return out;
}

//returns a colour between 0.0 and 1.0;
float ApplicationSolar::randCol(){
    float out = float(rand() % 100) / 100.0f;
    return out;
}



ApplicationSolar::~ApplicationSolar() {
    
    //delete planet buffers
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);

    //delete star buffers
    glDeleteBuffers(1, &star_object.vertex_BO);
    glDeleteVertexArrays(1, &star_object.vertex_AO);
    
    //delete star buffers
    glDeleteBuffers(1, &orbit_object.vertex_BO);
    glDeleteVertexArrays(1, &orbit_object.vertex_AO);
    
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}



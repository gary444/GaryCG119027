#include "application_solar.hpp"
#include "launcher.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#define NUM_STARS 1000

model star_model{};
model planet_model{};

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
, planet_object{}, star_object{}
{
  
    star_object.num_elements = NUM_STARS;
    
    
    for (int i = 0; i < star_object.num_elements; i++){
        
        starBuffer.push_back(randPos());
        starBuffer.push_back(randPos());
        starBuffer.push_back(randPos());
        
        starBuffer.push_back(randCol());
        starBuffer.push_back(randCol());
        starBuffer.push_back(randCol());
    }
    
    
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
    star_model = {starBuffer, model::POSITION | model::NORMAL};

    //set starting view
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 0.0f, 0.0f, 10.0f });
    m_view_transform = glm::rotate(m_view_transform, glm::radians(-10.0f), glm::fvec3{ 1.0f, 0.0f, 0.0f });
    
    
    
    initializeGeometry();
    initializeShaderPrograms();
    

  
}

void ApplicationSolar::render() const {
    
    
    
    
    //==================================================================
    //planets
    
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);
    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);

    // cycle through members of planet array and apply model matrix
    //sizeof(ar) / sizeof(ar[0]);
    for (int i = 0; i < (sizeof(planets) / sizeof(planets[0])); i++) {
        
        planet testPlanet = planets[i];
        
        // don't render if 'planet' is a moon
        if (testPlanet.isMoon == false) {
            
            // upload planet
            upload_planet_transforms(testPlanet);
        }
        
    }
    
    
    //==================================================================
    //stars
    
    upload_stars();
}

//function added assignment 2
void ApplicationSolar::upload_stars() const{
    
    // order from lecture 3, slide 15
    
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("star").handle);
    
    // bind the VAO to draw
    glBindVertexArray(star_object.vertex_AO);

    glDrawArrays(GL_POINTS, 0, star_object.num_elements);
    
    
}

// added function assignment 1
void ApplicationSolar::upload_planet_transforms(planet planetToDisplay) const
{
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);
    
    // use rotation speed and planet skew to create planet's orbit
    glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * planetToDisplay.rotationSpeed), glm::fvec3{ planetToDisplay.orbitSkew, 1.0f, 0.0f });
    
    // use planet.distToOrigin to translate planet away from origin
    model_matrix = glm::translate(model_matrix, glm::fvec3{ 0.0f, 0.0f, planetToDisplay.distToOrigin });

    
    // if this planet has a moon, add a moon using planet's location as a starting point
    if (planetToDisplay.hasMoonAtIndex > 0) {
        
        // bind shader to upload uniforms
        glUseProgram(m_shaders.at("planet").handle);
        
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
        
        // bind the VAO to draw
        glBindVertexArray(planet_object.vertex_AO);
        
        // draw bound vertex array using bound shader
        glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
        
    }
    
    // scale planet according to planet size
    model_matrix = glm::scale(model_matrix, glm::fvec3{ planetToDisplay.size, planetToDisplay.size, planetToDisplay.size });
    
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                       1, GL_FALSE, glm::value_ptr(model_matrix));
    
    //extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                       1, GL_FALSE, glm::value_ptr(normal_matrix));
    
    // bind the VAO to draw
    glBindVertexArray(planet_object.vertex_AO);
    
    // draw bound vertex array using bound shader
    glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
}

void ApplicationSolar::updateView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
    
    glUseProgram(m_shaders.at("star").handle);

    
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
    

    glUseProgram(m_shaders.at("planet").handle);

    
}

void ApplicationSolar::updateProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    glUseProgram(m_shaders.at("star").handle);

    
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
    
    
    glUseProgram(m_shaders.at("planet").handle);


}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
  updateUniformLocations();
  
  // bind new shader
  glUseProgram(m_shaders.at("planet").handle);
    
  //star shader
  //glUseProgram(m_shaders.at("star").handle);
  
  updateView();
  updateProjection();
}

// handle key input


void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
	//move scene toward camera
  if (key == GLFW_KEY_W && action == GLFW_PRESS) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.3f});
    updateView();
  }
  // move scene away from camera
  else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.3f});
    updateView();
  }
  // move scene right
  else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
	  m_view_transform = glm::translate(m_view_transform, glm::fvec3{ -1.0f, 0.0f, 0.0f });
	  updateView();
  }
  //move scene left
  else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
	  m_view_transform = glm::translate(m_view_transform, glm::fvec3{ 1.0f, 0.0f, 0.0f });
	  updateView();
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
    
    
    // add star shader here
    m_shaders.emplace("star", shader_program{m_resource_path + "shaders/star.vert",
        m_resource_path + "shaders/star.frag"});
    // request uniform locations for shader program
    //m_shaders.at("star").u_locs["NormalMatrix"] = -1;
    //m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
    
    
}

// load models
void ApplicationSolar::initializeGeometry() {
    
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
    
    
    
    // generate generic buffer
    glGenBuffers(1, &star_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * star_model.indices.size(), star_model.indices.data(), GL_STATIC_DRAW);

    
    
    // end star initialisation
  //======================================================================
    
    
    
    
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
    glDeleteBuffers(1, &star_object.element_BO);
    glDeleteVertexArrays(1, &star_object.vertex_AO);
    
}

// exe entry point
int main(int argc, char* argv[]) {
  Launcher::run<ApplicationSolar>(argc, argv);
}



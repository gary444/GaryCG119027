#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

using namespace gl;

#include <vector>

#define NUM_SPHERES 10


// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);

  // draw all objects
  void render() const;

    

 protected:
    void initializeShaderPrograms();
    void initializeGeometry();
    void updateView();
    void upload_planet_transforms(planet planetToDisplay) const;
    void upload_stars() const;
    void upload_Orbits() const;
    
private:
    void fillOrbits();
    float randPos();
    float randCol();
    void loadTextures();
    
    
    std::vector< float > starBuffer;
    std::vector< float > orbitBuffer;
    
    
    
    // cpu representation of models
    model_object planet_object;
    model_object star_object;
    model_object orbit_object;
    
    GLuint texBufferID;
    

    //relative earth values
  float EARTH_SIZE = 0.45f;
  float EARTH_SPEED = 0.4f;
  float EARTH_ORBIT = EARTH_SIZE * 20 ;

  //planet attributes originally copied from https://nssdc.gsfc.nasa.gov/planetary/factsheet/planet_table_ratio.html
  // then adapted for aesthetic purposes
  //planet struct initialisation: {size, rotation speed, dist to origin, orbitSkew, hasMoonAtIndex, isMoon}
  planet planets[NUM_SPHERES] = { {"sun", EARTH_SIZE * 6.0f,  1.0f,                0.0f,                  0.0f,   -1, false, {0.933, 0.741, 0.184}},//sun
					   {"mercury",  EARTH_SIZE * 1.383f,  EARTH_SPEED * 5.8f,  EARTH_ORBIT * 0.387f,  0.2f,   -1, false , {0.760, 0.670, 0.439}},//mercury
					   {"venus",  EARTH_SIZE * 0.949f,  EARTH_SPEED * 2.4f,   EARTH_ORBIT * 0.723f, -0.2f,   -1, false , {0.827, 0.392, 0.035}},//venus
					   {"earth",  EARTH_SIZE,           EARTH_SPEED,         EARTH_ORBIT,           0.0f,    4, false , {0.231, 0.349, 0.568}},//earth
					   {"moon",  EARTH_SIZE * 0.2724f, EARTH_SPEED / 0.2f, EARTH_ORBIT * 0.08f,   0.0f,   -1, true , {0.980, 0.980, 0.937}},//MOON
					   {"mars",  EARTH_SIZE * 0.532f,  EARTH_SPEED * 1.03f, EARTH_ORBIT * 1.52f,   0.03f,  -1, false , {0.741, 0.325, 0.039}},//mars
					   {"jupiter",  EARTH_SIZE * 6.21f,   EARTH_SPEED * 0.95f, EARTH_ORBIT * 3.2f,   -0.1f,   -1, false , {0.741, 0.243, 0.039}},//jupiter
					   {"saturn",  EARTH_SIZE * 6.45f,   EARTH_SPEED * 0.9f,  EARTH_ORBIT * 4.58f,   0.05f,  -1, false , {0.741, 0.705, 0.039}},//saturn
					   {"uranus",  EARTH_SIZE * 3.01f,   EARTH_SPEED * 0.85f, EARTH_ORBIT * 5.2f,    -0.05f,  -1, false , {0.039, 0.596, 0.741}},//uranus
					   {"neptune",  EARTH_SIZE * 2.88f,   EARTH_SPEED * 0.8f,   EARTH_ORBIT * 6.05f,   -0.02f,  -1, false , {0.247, 0.223, 0.952}} };//neptune
 


};

#endif
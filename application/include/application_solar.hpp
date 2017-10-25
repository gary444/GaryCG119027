#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

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

  // cpu representation of model
  model_object planet_object;


  //planet struct: {size, rotation speed, dist to origin, orbitSkew, hasMoonAtIndex, isMoon}
  float EARTH_SIZE = 0.2f;
  float EARTH_SPEED = 0.1f;
  float EARTH_ORBIT = EARTH_SIZE * 80 ;

  //planet attributes originally copied from https://nssdc.gsfc.nasa.gov/planetary/factsheet/planet_table_ratio.html
  // then adapted for aesthetic purposes


  //planet struct initialisation: {size, rotation speed, dist to origin, orbitSkew, hasMoonAtIndex, isMoon}
  planet planets[10] = { {EARTH_SIZE * 10.0f,  1.0f,                0.0f,                  0.0f,   -1, false},//sun
					   { EARTH_SIZE * 1.383f,  EARTH_SPEED * 5.8f,  EARTH_ORBIT * 0.387f,  0.2f,   -1, false },//mercury
					   { EARTH_SIZE * 0.949f,  EARTH_SPEED * 2.4,   EARTH_ORBIT * 0.723f, -0.2f,   -1, false },//venus
					   { EARTH_SIZE,           EARTH_SPEED,         EARTH_ORBIT,           0.0f,    4, false },//earth
					   { EARTH_SIZE * 0.2724f, EARTH_SPEED / 0.07f, EARTH_ORBIT * 0.03f,   0.0f,   -1, true },//MOON
					   { EARTH_SIZE * 0.532f,  EARTH_SPEED * 1.03f, EARTH_ORBIT * 1.52f,   0.03f,  -1, false },//mars
					   { EARTH_SIZE * 9.21f,   EARTH_SPEED * 0.95f, EARTH_ORBIT * 3.2f,   -0.1f,   -1, false },//jupiter
					   { EARTH_SIZE * 9.45f,   EARTH_SPEED * 0.9f,  EARTH_ORBIT * 4.58f,   0.05f,  -1, false },//saturn
					   { EARTH_SIZE * 4.01f,   EARTH_SPEED * 0.85f, EARTH_ORBIT * 5.2,    -0.05f,  -1, false },//uranus
					   { EARTH_SIZE * 3.88f,   EARTH_SPEED * 0.8,   EARTH_ORBIT * 6.05,   -0.02f,  -1, false } };//neptune



};

#endif
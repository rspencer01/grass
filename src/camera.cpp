#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <camera.h>
#include <shaders.h>
#include <gtc/matrix_transform.hpp>

/// Construct a new camera, putting its matrix and position in the given locations
/// @param gm The game in which this camera resides
/// @param sp The shader to which this camera belongs
Camera::Camera(glm::mat4* matrix, glm::vec4* position)
{
  // Initialise to some useful directions
  Position = glm::vec3(0,0,1);
  ViewDir = glm::vec3(1,0,0);
  UpVector = glm::vec3(0,1,0);
  // No rotation yet
  RotatedX = 0;
  // Remember the environment
  matrixData = matrix;
  positionData = position;
  // What are we attaching to
  attachment = NULL;
  followingDistance = 10.f;
}

/// Modifys the game world transform to reflect the postion of the camera.
/// Actually constructs a viewing matrix.
void Camera::Render()
{
  // Calculate the right vector
  glm::vec3 Right = glm::cross(ViewDir,UpVector);
  // Normalise all the other vectors
  Right = glm::normalize(Right);
  UpVector = glm::normalize(UpVector);
  ViewDir = glm::normalize(ViewDir);

  if (attachment!=NULL)
  {
    Position = (10.f*Position + (attachment->getPosition() - followingDistance*ViewDir))/11.f;
  }

  // Construct a matrix that transforms the object in the correct way
  *matrixData = glm::lookAt(Position,Position+ViewDir,UpVector);

  // Update the position of the camera, also
  *positionData = glm::vec4(Position,1);
}

/// Moves the camera in the direction it is facing
/// @param d The distance to move
void Camera::MoveForward( float d)
{
  Position = Position + ViewDir*d;
}

/// Rotates the camera about the Y axis (horisontal)
/// @param theta The angle to turn by
void Camera::RotateY( float theta )
{
  // Calculate the right hand vector
  glm::vec3 rightDir = glm::normalize(glm::cross(ViewDir,UpVector));
  ViewDir = glm::normalize(ViewDir*(float)cos(theta) - rightDir*(float)sin(theta));
}

/// Rotates the camera about the X axis (vertical)
/// @param theta The angle to turn by
void Camera::RotateX( float theta )
{
  // Calculate the right hand vector
  glm::vec3 rightDir = glm::normalize(glm::cross(ViewDir,UpVector));
  ViewDir = ViewDir*(float)cos(theta) + UpVector*(float)sin(theta);
  UpVector = glm::cross(rightDir,ViewDir);
  ViewDir  = glm::normalize(ViewDir);
  UpVector = glm::normalize(UpVector);
  // Log this rotation
  RotatedX += theta;
}

/// Rotates in the xz-plane.
/// @param theta The angle to turn by
void Camera::RotateFlat( float theta )
{
  // We are going to antirotate up to the horison, do a horisontal rotation and then look down again
  float rx = RotatedX;
  // Look up...
  RotateX(-rx);
  // Turn...
  RotateY(theta);
  // Look down.
  RotateX(rx);
}

/// @return The object of the camera in game space
glm::vec3 Camera::getPosition()
{
  return Position;
}

/// @param The new position of the camera
void Camera::setPosition(glm::vec3 newPos)
{
  Position = newPos;
}

/// @return The direction the camera is looking
glm::vec3 Camera::getViewDirection()
{
  return ViewDir;
}

/// @param The new direction for the camera to look in
void Camera::setViewDirection(glm::vec3 newDir)
{
  ViewDir = newDir;
}

void Camera::getInputFromWindow(Window* win)
{
  glm::vec2 diff = win->getMouseCentreOffset();
  win->setMouseCentre();
  RotateX(-diff.y*0.005);
  RotateFlat(-diff.x*0.005);
}

void Camera::zoomIn(float am)
{
  followingDistance += am;
}
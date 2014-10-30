#ifndef BALL_H
#define BALL_H

#include <magrathea.h>
#include <texture.h>
#include <object.h>

class Ball : public Object
{
  private:
    glm::vec3 velocity;
  public:
    Ball(glm::vec3,glm::vec3,Game* parent);
    void Render(float);
};

#endif
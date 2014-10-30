#ifndef GRASS_H
#define GRASS_H

#include <magrathea.h>
#include <texture.h>
#include <object.h>

class Grass : public Object
{
  public:
    Grass(glm::vec3,Game* parent);
    void Render();
};

#endif
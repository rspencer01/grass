#include <log.h>
#include <gtx/random.hpp>
#include <game.h>
#include <images.h>
#include <grass.h>

Grass::Grass(glm::vec3 pos,Game* game) : Object(pos,game)
{
  shaderID = game->shaderManager->newShader("grass",VERTEX_SHADER|GEOMETRY_SHADER|FRAGMENT_SHADER);
  int n = 50;
  int lod = 9;
  clearTriangleData(2*lod*n,2*(lod-1)*n);
  for (int j = 0;j<n;j++)
  {
    glm::vec3 pos = glm::vec3(glm::linearRand(-0.5,0.5),0,glm::linearRand(-0.5,0.5));
    glm::vec3 crs = glm::vec3(glm::circularRand(1.f),0);
    crs.z = crs.y;crs.y = 0;
    glm::vec3 nrm = glm::cross(glm::vec3(0,1,0),crs);

    for (int i = 0;i<lod;i++)
    {
      
      addPoint(2*lod*j+2*i  ,pos + 0.01f*crs + glm::vec3(0,i/float(lod),0),nrm,glm::vec4(1));
      addPoint(2*lod*j+2*i+1,pos - 0.01f*crs + glm::vec3(0,i/float(lod),0),nrm,glm::vec4(1));
      if (i>0)
      {
        addTriangle(2*(lod-1)*j+2*i,2*lod*j+2*i,2*lod*j+2*i+1,2*lod*j+2*(i-1));
        addTriangle(2*(lod-1)*j+2*i+1,2*lod*j+2*(i-1),2*lod*j+2*(i-1)+1,2*lod*j+2*i+1);
      }
    }
  }

  pushTriangleData();
}

void Grass::Render()
{
  Object::Render();
}
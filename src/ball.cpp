#include <log.h>
#include <gtx/random.hpp>
#include <game.h>
#include <grassDatamap.h>
#include <images.h>

Ball::Ball(glm::vec3 pos,glm::vec3 vel,Game* game) : Object(pos,game)
{
  velocity = vel;
  shaderID = game->shaderManager->newShader("ball",VERTEX_SHADER|GEOMETRY_SHADER|FRAGMENT_SHADER);
  clearTriangleData(12*10,11*20);
  for (int j = 0;j<10;j++)
    addPoint(j,glm::vec3(0,0,-0.5),
    glm::normalize(glm::vec3(0,0,-1)),glm::vec4(1));
  int t = 0;
  for (int i = 1;i<11;i++)
  {
    for (int j = 0;j<10;j++)
    {
      float z = -0.5+0.1*i;
      addPoint(10*i+j,
                       glm::vec3(sqrt(0.25-z*z)*cos(3.1415*2/10*j),sqrt(0.25-z*z)*sin(3.1415*2/10*j),z),
        glm::normalize(glm::vec3(sqrt(0.25-z*z)*cos(3.1415*2/10*j),sqrt(0.25-z*z)*sin(3.1415*2/10*j),z)),glm::vec4(1));
      addTriangle(t,i*10+j,i*10+(j+1)%10,(i-1)*10+j);
      addTriangle(t+1,i*10+(j+1)%10,(i-1)*10+j,(i-1)*10+(j+1)%10);
      t++;t++;
    }
  }
  for (int j = 0;j<10;j++)
  {
    addPoint(110+j,glm::vec3(0,0,0.5),
    glm::normalize(glm::vec3(0,0,1)),glm::vec4(1));
    addTriangle(t,110+j,110+(j+1)%10,100+j);
    t++;
  }

  pushTriangleData();
}

void Ball::Render(float ms)
{
  ms *= 0.2;
  if (ms<1)
  {
    glm::vec3 acc = glm::vec3(0,-9.8,0) - velocity*0.1f;
    position += velocity *ms;
    velocity += acc*ms;
    if (position.y<0)
    {
      position.y = -position.y;
      velocity.y = -0.7*velocity.y;
    }
  }
  for (float x = position.x-0.5;x<position.x+0.5;x+=0.005)
    for (float y = position.z-0.5;y<position.z+0.5;y+=0.005)
      if ((x-position.x)*(x-position.x) + (y-position.z)*(y-position.z)<0.25)
      {
        updateGrassDataMap(x,y,position.y-(sqrt(0.25-(x-position.x)*(x-position.x) - (y-position.z)*(y-position.z))));
      }
  updateMatrix();
  Object::Render();
}
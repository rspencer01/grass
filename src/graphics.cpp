#include <graphics.h>
#include <gtc/matrix_transform.hpp>

void makePerspectiveMatrix(glm::mat4* mat,float aspect)
{
  //*mat = glm::perspective(35.f,1.f,0.1f,100.f),sizeof(glm::mat4);
  *mat = glm::perspective(45.0f,aspect,0.2f,10000.f);
  //*mat = glm::ortho(-1,1,-1,1);
}

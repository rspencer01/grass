#include <noise.h>
#include <log.h>
#include <gtc\noise.hpp>

 Texture* perlinNoise;
void initNoise()
{
  logi.log("Creating noise");
  int width,height;
  width = 100;
  height = 100;

  perlinNoise =  new Texture(NOISE_TEXTURE,width,height);
  float* data = new float[width*height];
  glm::vec2 w = glm::vec2(10.f);
  glm::mat2 rotMat1 = glm::mat2(cos(20.f),-sin(20.f),sin(20.f),cos(20.f));
  for (int i = 0;i<width;i++)
    for (int j = 0;j<height;j++)
    {
      glm::vec2 p = 10.f*glm::vec2(i/float(width),j/float(height));
      data[i*width+j] = 0.5*(glm::perlin(p,w)+0.5*glm::perlin(2.f*rotMat1*p,w*2.f)+0.25*glm::perlin(4.f*p,w*4.f)+0.125*glm::perlin(rotMat1*p*8.f,w*8.f));
    }
  perlinNoise->loadBumpData(data);
  perlinNoise->toTGA("noise.tga");
  perlinNoise->load();
}
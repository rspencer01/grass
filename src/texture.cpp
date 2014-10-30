#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <texture.h>
#include <cmath>
#include <glm.hpp>
#include <noise.h>
#include <gtc\noise.hpp>
using namespace std;

Texture::Texture(int type, int w, int h)
{
  textureType = type;
  width = w;
  height = h;
  data = new float[width*height*4];
  for (int i = 0;i<width*height*4;i++)
    data[i] = i%4<4?0:1;
  glGenTextures(1,&textureNumber);
  glActiveTexture(textureType);
  glBindTexture(GL_TEXTURE_2D,textureNumber);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
}

void Texture::toTGA(const char* file)
{
  FILE* fp = fopen(file,"wb");
  putc(0,fp);
  putc(0,fp);
  putc(2,fp);                         /* uncompressed RGB */
  putc(0,fp); putc(0,fp);
  putc(0,fp); putc(0,fp);
  putc(0,fp);
  putc(0,fp); putc(0,fp);           /* X origin */
  putc(0,fp); putc(0,fp);           /* y origin */
  putc((width & 0x00FF),fp);
  putc((width & 0xFF00) / 256,fp);
  putc((height & 0x00FF),fp);
  putc((height & 0xFF00) / 256,fp);
  putc(32,fp);                        /* 24 bit bitmap */
  putc(0,fp);
  for (int i = 0;i<width;i++)
    for (int j = 0;j<height;j++)
    {
      float f = abs(data[(i*height+j)*4+0]);
      if (f<0)f=-f;
      unsigned char c = (abs(data[(i*height+j)*4+0])*255);
      putc((unsigned char)(abs(data[(i*height+j)*4+0]*255)),fp);
      putc((unsigned char)(abs(data[(i*height+j)*4+1]*255)),fp);
      putc((unsigned char)(abs(data[(i*height+j)*4+2]*255)),fp);
      putc((unsigned char)(abs(data[(i*height+j)*4+3]*255)),fp);
    }
  fclose(fp);
}

void Texture::loadData(float* inData)
{
  memcpy(data,inData,width*height*4*sizeof(float));
  glActiveTexture(textureType);
  glBindTexture(GL_TEXTURE_2D,textureNumber);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data);
}

void Texture::loadBumpData(float* inData)
{
  for (int i=0;i<width*height;i++)
  {
    glm::vec3 no = glm::vec3(1.f);
    if ((i%width)>0 && (i/width>0) && (i%width<width-1) && (i/width<height-1))
    {
      float a,b,c,d,m;
      a = inData[i-width];
      b = inData[i+1];
      c = inData[i+width];
      d = inData[i-1];
      m = inData[i];
      no = -glm::cross(glm::vec3(0.f,a-m,-1.f/height),glm::vec3(1.f/width,b-m,0.f))
           -glm::cross(glm::vec3(1.f/width,b-m,0.f),glm::vec3(0.f,c-m,1.f/height))
           -glm::cross(glm::vec3(0.f,c-m,1.f/height),glm::vec3(-1.f/width,d-m,0.f))
           -glm::cross(glm::vec3(-1.f/width,d-m,0.f),glm::vec3(0.f,a-m,-1.f/height));
    }
    no = glm::normalize(no);
    data[i*4  ] = no.x;
    data[i*4+1] = no.y;
    data[i*4+2] = no.z;
    data[i*4+3] = inData[i];
  }
  glActiveTexture(textureType);
  glBindTexture(GL_TEXTURE_2D,textureNumber);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data);
}

void Texture::load()
{
  glActiveTexture(textureType);
  glBindTexture(GL_TEXTURE_2D,textureNumber);
}

glm::vec4 Texture::getAt(float x,float y)
{
  return glm::vec4(data[(int(height*y*width)+int(width*x))*4],
                   data[(int(height*y*width)+int(width*x))*4+1],
                   data[(int(height*y*width)+int(width*x))*4+2],
                   data[(int(height*y*width)+int(width*x))*4+3]);
}

void Texture::add(Texture* other)
{
  for (int i = 0;i<width;i++)
    for (int j = 0;j<height;j++)
    {
      data[(i*height+j)*4]   += other->getAt(float(i)/width,float(j)/height).r*other->getAt(float(i)/width,float(j)/height).a;
      data[(i*height+j)*4+1] += other->getAt(float(i)/width,float(j)/height).g*other->getAt(float(i)/width,float(j)/height).a;
      data[(i*height+j)*4+2] += other->getAt(float(i)/width,float(j)/height).b*other->getAt(float(i)/width,float(j)/height).a;
      data[(i*height+j)*4]   /=2;
      data[(i*height+j)*4+1]   /=2;
      data[(i*height+j)*4+2]   /=2;
    }
    loadData(data);
}

void Texture::mask(Texture* other)
{
  for (int i = 0;i<width;i++)
    for (int j = 0;j<height;j++)
    {
      data[(i*height+j)*4]   *= other->getAt(float(i)/width,float(j)/height).a;
      data[(i*height+j)*4+1] *= other->getAt(float(i)/width,float(j)/height).a;
      data[(i*height+j)*4+2] *= other->getAt(float(i)/width,float(j)/height).a;
    }
    loadData(data);
}


void makeTextures()
{
  int width = 1000;
  int height =1000;
  Texture stone(COLOUR_TEXTURE,width,height);
  float* data = new float [4*width*height];
  for (int i = 0;i<height;i++)
    for (int j = 0;j<width;j++)
    {
      data[(i*width+j)*4+0] = 0/256.0;
      data[(i*width+j)*4+1] = 0/256.0;
      data[(i*width+j)*4+2] = 256/256.0;
      data[(i*width+j)*4+3] = 1;
    }

  Texture moss(COLOUR_TEXTURE,width,height);
  float* mdata = new float [4*width*height];
  for (int i = 0;i<height;i++)
    for (int j = 0;j<width;j++)
    {
      mdata[(i*width+j)*4+0] = 0/256.0;
      mdata[(i*width+j)*4+1] = 256/256.0;
      mdata[(i*width+j)*4+2] = 0/256.0;
      mdata[(i*width+j)*4+3] = 0.3*(1+perlinNoise->getAt(float(j)/width,float(i)/height).a);
    }
  moss.loadData(mdata);
  stone.loadData(data);
  stone.add(&moss);
  stone.toTGA("stone.tga");
  moss.toTGA("moss.tga");
}


#ifdef TEST_TEXTURE
int main()
{
  Texture texture(HEIGHTMAP_TEXTURE,8,8);
  texture.toTGA("test.tga");
}
#endif

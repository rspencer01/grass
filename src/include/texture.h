#ifndef TEXTURE_H
#define TEXTURE_H

#include <magrathea.h>

#define HEIGHTMAP_TEXTURE GL_TEXTURE0
#define COLOUR_TEXTURE GL_TEXTURE1
#define NOISE_TEXTURE GL_TEXTURE2
#define GRASS_DATA_TEXTURE GL_TEXTURE3


class Texture
{
  private:
    int textureType;
    int width;
    int height;
    float* data;
    GLuint textureNumber;
  public:
    Texture(int type,int width, int height);
    void toTGA(const char* file);
    void loadData(float* data);
    void loadBumpData(float* data);
    void load();
    glm::vec4 getAt(float x,float y);
    void add(Texture* other);
    void mask(Texture* other);
};

void makeTextures();

#endif


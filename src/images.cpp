#include <stdio.h>
#include <images.h>
#include <magrathea.h>

/// Genertate a new texture from scratch
/// @param smoothTexture Should the texture be smoothly interpolated?
GLuint newTexture(bool smoothTexture)
{
  GLuint returnValue;
  glGenTextures(1,&returnValue);
	glBindTexture(GL_TEXTURE_2D,returnValue);

  // Set the samplers for the smooth and solid versions
  if (smoothTexture)
  {
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  }
  else
  {
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  }

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  return returnValue;
}

Texture* textureFromTGA(const char* filePath, bool smoothTexture)
{
	logi.clog(LOG_COLOR_PURPLE,"Loading texture %s",filePath);
	FILE* fp = fopen(filePath,"rb");
  if (fp==NULL)
    DIE2("Texture path doesn't exist:",filePath);
  unsigned char inp;
  fseek(fp,2,0);
  fread(&inp,1,1,fp);
  unsigned short width;
  unsigned short height;
  fseek(fp,12,0);
  fread(&inp,1,1,fp);
  width = inp;
  fread(&inp,1,1,fp);
  width += inp<<8;
  fread(&inp,1,1,fp);
  height = inp;
  fread(&inp,1,1,fp);
  height += inp<<8;

  fread(&inp,1,1,fp);
  if (inp!=32)
  {
    DIE("Error loading texture:  All TGA files must be 32 bit.\n");
  }
  fread(&inp,1,1,fp);

  unsigned char* rgbaData = new unsigned char[width*height*4];
  fread(rgbaData,4,height*width,fp);
  for (int i = 0;i<width*height;i++)
  {
    unsigned char t = rgbaData[i*4 ];
    rgbaData[i*4     ] = rgbaData[i*4 + 2];
    rgbaData[i*4 + 2 ] = t;
  }
  float* floatData = new float[width*height*4];
  for (int i = 0;i<width*height*4;i++)
    floatData[i] = rgbaData[i]/256.f;
  Texture* texture = new Texture(COLOUR_TEXTURE,width,height);
  texture->loadData(floatData);

  delete rgbaData;
  delete floatData;
	fclose(fp);

	return texture;

}

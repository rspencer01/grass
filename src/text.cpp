#include <magrathea.h>
#include <images.h>
#include <log.h>
#include <text.h>
#include <stdarg.h>

GLuint fontTextureNumber;
float charSize = 0.075f;

void initTextEngine()
{
  logi.log("Initialising text engine");

  //fontTextureNumber = textureFromTGA("assets/font.tga",true);
}

void printString(int x, int y, const char* format, ... )
{
  char buffer [80];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);
  int i = 0;
  while (buffer[i])
  {
    printLetter(buffer[i],x,y);
    x++;
    i++;
  }
}

void printLetter(char c,int x,int y)
{
  float TposX = (c%16)/16.f;
  float TposY = 1-(c/16 +1)/16.f;
  float s = 1.f/16.f;
  float posX = (x * charSize - 1);
  float posY = 1.f-(y+1) * charSize;

  glEnable( GL_TEXTURE_2D );
  glBindTexture( GL_TEXTURE_2D, fontTextureNumber );
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glBegin(GL_QUADS);
  glColor3f(1.f, 1.f, 1.f);
  glTexCoord2d(TposX,TposY);glVertex3f(posX, posY, 0.f);
  glTexCoord2d(TposX+s,TposY);glVertex3f(posX+charSize, posY, 0.f);
  glTexCoord2d(TposX+s,TposY+s);glVertex3f(posX+charSize, posY+charSize, 0.f);
  glTexCoord2d(TposX,TposY+s);glVertex3f(posX, posY+charSize, 0.f);  
  glEnd();
}

#include <magrathea.h>
#include <texture.h>
#include <grassDatamap.h>

Texture* dm;
int w = 1000, h = 1000;
float * data;

void initGrassDataMap()
{
  dm = new Texture(GRASS_DATA_TEXTURE,w,h);
  data = new float [4*w*h];
  for (int i = 0;i<4*w*h;i++)
  {
    data[i] = 1;
  }
  dm->loadData(data);
}

void updateGrassDataMap(float x,float y,float v)
{
  if ((v<0) || (v>1)) return;
  int xx = (w*x)/10;
  int yy = (h*y)/10;
  if (xx>w-1) return;
  if (xx<0) return;
  if (yy>h-1) return;
  if (yy<0) return;
  //logi.log("%d %d",xx,yy);
  data[4*(yy*w+xx)] = 
    data[4*(yy*w+xx)+1] = 
    data[4*(yy*w+xx)+2] = 
    data[4*(yy*w+xx)+3] =
    min(v,data[4*(yy*w+xx)]);
}

void loadGrassDataMap()
{
  dm->loadData(data);
  dm->load();
}
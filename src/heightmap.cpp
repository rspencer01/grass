#include <heightmap.h>
#include <string.h>
#include <math.h>
#include <log.h>
#include <stdlib.h>

int width,height;
float scale;
float* heights = NULL;

float randf()
{
  return rand()%10000/10000.f-0.5f;
}

void doErosion(int width, int height, float* values)
{
  logi.clog(LOG_COLOR_LIGHT_BLUE,"Eroding");
  float* nv = new float[width*height];
  for (int i = 0;i<height*width;i++)
    nv[i] = values[i];
  for (int time = 10;time>0;time--)
  {
    for (int i = 1;i<height-1;i++)
      for (int j = 1;j<width-1;j++)
      {
        int ci=i,cj=j;
        for (int k = 1000;k>0;k--)
        {
          if (ci==0||cj==0||ci==height-1||cj==width-1)break;

          float a = values[ci*width+cj-1];
          float b = values[ci*width+cj+1];
          float c = values[(ci+1)*width+cj];
          float d = values[(ci-1)*width+cj];
          float m = values[(ci)*width+cj];
          if (m<=a &&m<=b&&m<=c&&m<=d) break;
          if (m>a &&m>b&&m>c&&m>d) nv[(ci)*width+cj] =values[(ci)*width+cj]*0.95;
          else nv[(ci)*width+cj] =values[(ci)*width+cj]*0.99;
          if (a<b&&a<c&&a<d){cj--; continue;}
          if (b<a&&b<c&&b<d){cj++; continue;}
          if (d<a&&d<c&&d<b){ci--; continue;}
          {ci++; continue;}
        }
      }
    for (int i = 0;i<height*width;i++)
      values[i] = nv[i];
  }
}

float* getHeightmapData(int w, int h)
{
  width =w;height = h;
  scale = w/8000.f;
  int seed = 1;
  srand(seed);
  logi.clog(LOG_COLOR_LIGHT_BLUE,"Constructing heightmap, seed %d",seed);
  if (heights!=NULL)
    delete[] heights;
  heights = new float[width*height];

  for (int i=0;i<(width)*(height);i++) heights[i] = 0;
  heights[0] = 0.1;
  heights[width-1] = 0;
  heights[(height-1)*(width)] = 0.0;
  heights[(height-1)*(width)+height-1] = 0;

  int sz = width-1;
  int count = 1;while(1<<count<sz)count++;count--;
  float H = 0.9;
  float scaler = pow(2.f,H);
  float rng = 1.f;
  while (sz>1)
  {
    for (int i = 0;i<height-1;i+=sz)
      for (int j = 0;j<width-1;j+=sz)
      {
        float a = heights[i*width+j];
        float b = heights[i*width+j+sz];
        float c = heights[(i+sz)*width+j];
        float d = heights[(i+sz)*width+j+sz];

        float r = rng;
        if (width/sz<4)
          r = 0;
        heights[(i+sz/2)*width+(j+sz/2)] = (a+b+c+d)/4 + r * randf();
        float e = heights[(i+sz/2)*width+(j+sz/2)];
        heights[(i+sz/2)*width+(j)] = (a+c+e)/3 + r * randf();
        heights[(i)*width+(j+sz/2)] = (a+b+e)/3 + r * randf();
        heights[(i+sz/2)*width+(j+sz)] = (b+d+e)/3 + r * randf();
        heights[(i+sz)*width+(j+sz/2)] = (c+d+e)/3 + r * randf();
      }
    sz /= 2;
    count--;
    rng/=scaler;
  }
  for (int i = 0;i<width;i++)
    for (int j = 0; j<height;j++)
      heights[i*width+j] = 8*pow(abs((double)heights[i*width+j]),1.5)*0.7;
  doErosion(width,height,heights);

  return heights;
}

float getHeight(float x, float y)
{
  return 0;
  if (x<1) x = 1;
  if (y<1) y = 1;
  if (x>7999) x = 7999;
  if (y>7999) y = 7999;
  x*=scale;
  y*=scale;
  x-=0.5;
  y-=0.5;
  float a = heights[int(y)*width+int(x)]*1000;
  float b = heights[int(y)*width+int(x)+1]*1000;
  float c = heights[(int(y)+1)*width+int(x)]*1000;
  float d = heights[(int(y)+1)*width+int(x)+1]*1000;
  float fx = x-int(x);
  float fy = y-int(y);
  float e = a*(1-fx)+b*fx;
  float f = c*(1-fx)+d*fx;
  return e*(1-fy)+f*fy;
}
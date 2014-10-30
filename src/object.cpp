#include <GL/glew.h>
#include <GL/glut.h>

#include <fstream>
#include <string>
#include <vector>
#include <sstream>


#include <stdio.h>
#include <object.h>
#include <game.h>


/// @param pos The position of this object in gamespace
/// @param g   The game to which this object belongs
Object::Object(glm::vec3 pos,Game* g)
{
  position = pos;
  game = g;
  buffersInitialised = false;
  vertexData = NULL;
  triDat = NULL;
  billboard = false;
  forward = glm::vec3(1,0,0);
  up = glm::vec3(0,1,0);
  right = glm::vec3(0,0,1);
  xySlew = 0;
  glGenBuffers(1, &objectBO);
  VAO = (GLuint)-1;
  updateMatrix();
}

/// Frees the data used by this object (esp the buffers in the GPU)
Object::~Object()
{
  if (vertexData!=NULL)
    delete vertexData;
  if (triDat!=NULL)
    delete triDat;
  if (buffersInitialised)
  {
  	glDeleteBuffersARB(1,&vertexVBO);
  	glDeleteBuffersARB(1,&indexVBO);
    glDeleteBuffersARB(1,&objectBO);
    glDeleteVertexArrays(1,&VAO);
  }
}

/// Changes the position of the object.
void Object::setPosition(glm::vec3 pos)
{
  position = pos;
  updateMatrix();
}

/// Renders this object to the screen, using the VBOs that were 
/// initialised using the addPoint, addTriangle and pushTriangleData
/// functions
/// @param refreshTime Number of milliseconds since the last rendering
/// @param cameraPos   Position of the camera in 3-space
void Object::Render()
{
  // Rotate the object if it is a billboard
  if (billboard)
  {
  }
  // Only do something if we have data
	if (buffersInitialised)
	{
    game->shaderManager->loadShader(shaderID);
    // Load our transformation matrix etc
    game->shaderManager->getCurrentShader()->setObjectData(objectBO);
    // Select this object's texture
//    glBindTexture(GL_TEXTURE_2D,textureNumber);
    // Use our data

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,numberOfTriangles*3,GL_UNSIGNED_INT,0);
  }
}

/// Reset the data.  Clear the internal arrays, and the GPU arrays.
/// Reserve space for p point and t triangles
/// @param p Number of points
/// @param t Number of triangles
void Object::clearTriangleData(int p, int t)
{
  // Clear all the data and the buffers (if required)
  if (triDat!=NULL)
  { 
    delete triDat;
    triDat = NULL;
  }
  if (vertexData!=NULL)
  {
    delete vertexData;
    vertexData = NULL;
  }
  triDat = new int[t*3];
  vertexData = new VertexDatum[p];
  if (buffersInitialised)
  {
  	glDeleteBuffersARB(1,&vertexVBO);
	  glDeleteBuffersARB(1,&indexVBO);
  }
  numberOfTriangles = t;
  numberOfPoints = p;
  // Now make buffers
	glGenBuffersARB(1,&indexVBO);
  glGenBuffersARB(1,&vertexVBO);
}
/// Add a new point to the object.  All points are taken relative to the origin of the object
/// @param i The index of the vertex to change
/// @param point The position of this point, relative to the object origin
/// @param r The red component of the colour
/// @param b The blue component of the colour
/// @param g The green component of the colour
void Object::addPoint(int i,glm::vec3 point,glm::vec3 normal, glm::vec4 col)
{
  // Add it to the internal array
	vertexData[i].position = point;
	vertexData[i].normal = normal;
	vertexData[i].colour = col;
  vertexData[i].texMix = glm::vec4(-1);
}


void Object::setTextureMix(int point,float a, float b, float c, float d)
{
  vertexData[point].texMix = glm::vec4(a,b,c,d);
}

/// Adds a new triangle to the object.  Indexes are the same as the order the points were added
/// @param i The index of the triangle to edit
/// @param a The index of the first point
/// @param b The index of the second point
/// @param c The index of the third point
void Object::addTriangle(int i, int a,int b, int c)
{
  // Add it to the internal array
	triDat[i*3] = a;
	triDat[i*3+1] = b;
	triDat[i*3+2] = c;
}

/// Constructs new VBOs and pushes all the data to the GPU
void Object::pushTriangleData()
{
  // Make our VAO.  Not sure why leaving this out stops the first region from rendering... does the vao need to know about glBufferData?
  if (VAO==(GLuint)-1)
    glGenVertexArrays(1,&VAO);
  glBindVertexArray(VAO);
	// set it as the current one,
	glBindBufferARB(GL_ARRAY_BUFFER, vertexVBO);
	// ... and blit the data in.
	glBufferDataARB(GL_ARRAY_BUFFER, numberOfPoints*sizeof(VertexDatum),vertexData,GL_STATIC_DRAW);

	// set it as the current one,
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	// ... and blit the data in.
  glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER, numberOfTriangles*3*sizeof(int),triDat,GL_STATIC_DRAW);
  
  updateVAO();
	// Finally set the variables that need setting
	buffersInitialised = true;
}

void Object::updateVAO()
{
  if (VAO==(GLuint)-1)
    glGenVertexArrays(1,&VAO);
  glBindVertexArray(VAO);
  glBindBufferARB(GL_ARRAY_BUFFER,vertexVBO);
  glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointerARB(0,3,GL_FLOAT,GL_FALSE,sizeof(VertexDatum),0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointerARB(1,4,GL_FLOAT,GL_FALSE,sizeof(VertexDatum),(void*)(3*sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointerARB(2,2,GL_FLOAT,GL_FALSE,sizeof(VertexDatum),(void*)(10*sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointerARB(3,3,GL_FLOAT,GL_FALSE,sizeof(VertexDatum),(void*)(7*sizeof(float)));
  glEnableVertexAttribArray(4);
  glVertexAttribPointerARB(4,4,GL_FLOAT,GL_FALSE,sizeof(VertexDatum),(void*)(12*sizeof(float)));
  glBindVertexArray(0);
}

void Object::updateTriangleData()
{

  glDeleteBuffersARB(1,&vertexVBO);
  glGenBuffersARB(1,&vertexVBO);
  glBindBufferARB(GL_ARRAY_BUFFER,vertexVBO);
  glBufferDataARB(GL_ARRAY_BUFFER, numberOfPoints*sizeof(VertexDatum),vertexData,GL_STATIC_DRAW);

  updateVAO();
}

void Object::editTextureCoord(int i, float u, float v)
{
	vertexData[i].texture = glm::vec2(u,v);
}

/// Rotates the object to match the given new axis
/// @param basisX The new X axis
/// @param basisY The new Y axis
void Object::rotate(glm::vec3 basisX,glm::vec3 basisY)
{
  forward = glm::normalize(basisX);
  up = glm::normalize(basisY);
  right = glm::normalize(glm::cross(up,forward));
  updateMatrix();
}

/// Updates the object translation and rotation matrix
void Object::updateMatrix()
{
  // This works.  You can check it yourself.
  objectData.transformMatrix = glm::mat4(glm::vec4(forward,0),
                                         glm::vec4(up,0) + glm::vec4(xySlew,0,0,0),
                                         glm::vec4(right,0),
                                         glm::vec4(position,1));
  updateObjectBO();
}

/// WARNING: Only call this function if you are sure that you will never 
/// edit the vertices again.  That will cause a seg fault
/// Only to be called after the triangle data has been set of an object
/// that is either massive, or very common and that will never animate
/// (eg trees)
void Object::freeze()
{
  if (triDat!=NULL)
  {
    delete[] triDat;
    triDat = NULL;
  }
  if (vertexData!=NULL)
  {
    delete[] vertexData;
    vertexData = NULL;
  }
}

void Object::updateObjectBO()
{
  glBindBuffer(GL_UNIFORM_BUFFER, objectBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(objectData), &objectData, GL_DYNAMIC_DRAW);
}

glm::vec3 Object::getPosition()
{
  return position;
}
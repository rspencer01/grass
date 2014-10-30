#include <magrathea.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <shaders.h>
#include <assert.h>
#include <fstream>
#include <string>
#include <cstring>

GLuint gSampler;
ShaderManager::ShaderManager()
{
  numShaders = 0;
  currentShader=-1;
  glGenBuffers(1, &frameDataBO);
}

int ShaderManager::newShader(const char* name,int types)
{
  if (shaderNames.count(std::string(name))!=0)
    return shaderNames[std::string(name)];
  shaders[numShaders] = new ShaderProgram(name);
  stypes[numShaders] = types;
  char path[80]={0};
  if (types&VERTEX_SHADER)
  {
    strcpy(path,"./shaders/");strcat(path,name);strcat(path,"/vertexShader.shd");
    shaders[numShaders]->LoadShader(path,GL_VERTEX_SHADER);
  }
  if (types&GEOMETRY_SHADER)
  {
    strcpy(path,"./shaders/");strcat(path,name);strcat(path,"/geometryShader.shd");
    shaders[numShaders]->LoadShader(path,GL_GEOMETRY_SHADER);
  }
  if (types&TESSELATION_SHADER)
  {
    strcpy(path,"./shaders/");strcat(path,name);strcat(path,"/tesselationControlShader.shd");
    shaders[numShaders]->LoadShader(path,GL_TESS_CONTROL_SHADER);
    strcpy(path,"./shaders/");strcat(path,name);strcat(path,"/tesselationEvaluationShader.shd");
    shaders[numShaders]->LoadShader(path,GL_TESS_EVALUATION_SHADER);
  }
  if (types&FRAGMENT_SHADER)
  {
    strcpy(path,"./shaders/");strcat(path,name);strcat(path,"/fragmentShader.shd");
    shaders[numShaders]->LoadShader(path,GL_FRAGMENT_SHADER);
  }
  shaders[numShaders]->CompileAll();
  shaders[numShaders]->Load();
  shaderNames[std::string(name)] = numShaders;
  numShaders++;
  return numShaders-1;
}
int ShaderManager::addShader(ShaderProgram* s,const char* name)
{
  if (shaderNames.count(std::string(name))!=0)
    return shaderNames[std::string(name)];
  logi.log("New Shader \"%s\"",name);
  shaders[numShaders] = s;
  shaderNames[std::string(name)] = numShaders;
  numShaders++;
  return numShaders-1;
}

ShaderProgram* ShaderManager::getShaderById(int id)
{
  return shaders[id];
}
void ShaderManager::loadShader(int id)
{
  if (id!=currentShader)
  {
    shaders[id]->Load();
    shaders[id]->setFrameData(frameDataBO);
    currentShader = id;
  }
}
void ShaderManager::setFrameData()
{
  glBindBuffer(GL_UNIFORM_BUFFER, frameDataBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(frameData), &frameData, GL_DYNAMIC_DRAW);
}

ShaderProgram* ShaderManager::getCurrentShader()
{
  return shaders[currentShader];
}

void ShaderManager::reloadAll()
{
  // TODO: redo this completely.
  logw.clog(LOG_COLOUR_RED,"Reloading shaders...");
  for (int i = 0;i<numShaders;i++)
  {
    const char* name = shaders[i]->name;
    delete shaders[i];
    shaders[i] = new ShaderProgram(name);
    char path[80]={0};
    if (stypes[i]&VERTEX_SHADER)
    {
      strcpy(path,"./shaders/");strcat(path,name);strcat(path,"/vertexShader.shd");
      shaders[i]->LoadShader(path,GL_VERTEX_SHADER);
    }
    if (stypes[i]&GEOMETRY_SHADER)
    {
      strcpy(path,"./shaders/");strcat(path,name);strcat(path,"/geometryShader.shd");
      shaders[i]->LoadShader(path,GL_GEOMETRY_SHADER);
    }
    if (stypes[i]&TESSELATION_SHADER)
    {
      strcpy(path,"./shaders/");strcat(path,name);strcat(path,"/tesselationControlShader.shd");
      shaders[i]->LoadShader(path,GL_TESS_CONTROL_SHADER);
      strcpy(path,"./shaders/");strcat(path,name);strcat(path,"/tesselationEvaluationShader.shd");
      shaders[i]->LoadShader(path,GL_TESS_EVALUATION_SHADER);
    }
    if (stypes[i]&FRAGMENT_SHADER)
    {
      strcpy(path,"./shaders/");strcat(path,name);strcat(path,"/fragmentShader.shd");
      shaders[i]->LoadShader(path,GL_FRAGMENT_SHADER);
    }
    shaders[i]->CompileAll();
    shaders[i]->Load();
  }
}


/// Gets rid of all the 0x0D values in a file (kills the GLSL compiler)
/// @param program The shader program to sanatise
/// @param size    The size of said program
void sanatiseShader(std::string program)
{
  // Loop through the file, replacing '\r' with newlines
  for (unsigned int i = 0;i<program.size();i++)
     if (program[i]==0x0D)
        program[i] = 0x0A;
}

/// Ask OpenGL to create a new shader program, and kill the program if there was an error
ShaderProgram::ShaderProgram(const char* nm)
{
  // Store the name
  name = nm;
  logi.clog(LOG_COLOR_BLUE,"New Shader \"%s\"",name);
  // Construct the program we are going to use
  ShaderProgramID = glCreateProgram();
  // If there was an error, let us know
  if (ShaderProgramID == 0) 
    DIE("Error creating new shader program");
  objectDataPosition = (GLuint)-1;
  frameDataPosition = (GLuint)-1;
}

/// Load a shader program from a source file.
/// @param shaderPath The path to the source file
/// @param shaderType The type of shader object to load (fragment or vertex)
void ShaderProgram::LoadShader(const char* shaderPath, GLenum shaderType)
{
  // Create us a new shader
  GLuint ShaderObj = glCreateShader(shaderType);
  // If something went wrong, tell us about it
  if (ShaderObj == 0) 
    DIE("Error creating shader object");
  // Now open the shader source.
  FILE* fp = fopen(shaderPath,"rb");
  if (fp==NULL)
    DIE2("Cannot open shader source",shaderPath);
  // Read in the entire file
  std::string progSource;
  std::ifstream infile;
  infile.open(shaderPath);
  std::string line;
  while(std::getline(infile,line))
  {
    progSource+=line+'\n';
  }
  // Sanatise the shader
  sanatiseShader(progSource);
  // And tell opengl that it is the source code
/*  if (shaderType==GL_TESS_CONTROL_SHADER)
  {
    std::ofstream ofile;
    ofile.open("out2");
    ofile.write(progSource,fileSize);
    ofile.close();
  }*/
  /*logi.log("Shader content:\n%s",progSource);
  logi.log("Filesize %d",fileSize);*/
  int fileSize = progSource.size();
  const char* s = progSource.c_str();
  glShaderSource(ShaderObj, 1, &s, NULL);
  // Now we are done with these, get rid of them
  fclose(fp);
  // Attempt to compile the shader
  glCompileShader(ShaderObj);
  // If there is an error, tell us about it
  GLint success;
  glGetObjectParameterivARB(ShaderObj, GL_OBJECT_COMPILE_STATUS_ARB, &success);
  if (success==0) 
  {
    GLchar InfoLog[1024];
    glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
    DIE3("Error compiling shader",shaderPath, InfoLog);
  }
  // Attach the compiled object to the program
  glAttachShader(ShaderProgramID, ShaderObj);
}

/// Compile the loaded shaders into the program
void ShaderProgram::CompileAll()
{
  // Bind the attributes to the right locations
  glBindAttribLocation(ShaderProgramID,0,"inPosition");
  glBindAttribLocation(ShaderProgramID,1,"inColour");
  glBindAttribLocation(ShaderProgramID,2,"inTex");
  glBindAttribLocation(ShaderProgramID,3,"inNorm");
  glBindAttribLocation(ShaderProgramID,4,"inTexShades");
  // Link the program
  glLinkProgram(ShaderProgramID);
  // If there is an error...
  GLint Success = 0;
  glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) 
  {
    GLchar ErrorLog[1024] = { 0 };
		glGetProgramInfoLog(ShaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
    DIE2("Error linking shader program:", ErrorLog);
	}
  // Confirm that this is valid
  glValidateProgram(ShaderProgramID);
  // Again witht the error
  glGetProgramiv(ShaderProgramID, GL_VALIDATE_STATUS, &Success);
  if (!Success) 
  {
    GLchar ErrorLog[1024] = { 0 };
    glGetProgramInfoLog(ShaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
    DIE2("Invalid shader program:", ErrorLog);
  }
}

/// Load the program into GPU memory, ready to be run
void ShaderProgram::Load()
{
  // Now load this program
  glUseProgram(ShaderProgramID);
  // Set the uniform samplers
  setInt("heightmap",0);
  setInt("colourTexture",1);
  setInt("noiseTexture",2);
  setInt("grassDataTexture",3);
}

GLuint ShaderProgram::getVariablePosition(const char* name)
{
  // A std::string for hasing purposes
  std::string vName (name);
  // If we don't know this variable, find out its location
  if (variableLocations.count(vName)==0)
    variableLocations[vName] = glGetUniformLocation(ShaderProgramID, name);
  // Die nicely if we need to
  //if (variableLocations[vName]==0xFFFFFFFF)
  //  DIE2("Cannot find shader variable",name);
  // Return the answer
  return variableLocations[vName];
}

void ShaderProgram::setInt(const char* varName, unsigned int value)
{
  glUniform1i(getVariablePosition(varName),value);
}

void ShaderProgram::setObjectData(GLuint bo)
{
  if (objectDataPosition == (GLuint) -1)
  {
    objectDataPosition = glGetUniformBlockIndex(ShaderProgramID,"objectData");
    glUniformBlockBinding(ShaderProgramID, objectDataPosition, 0);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER,0,bo);
}

void ShaderProgram::setFrameData(int frameDataBO)
{
  if (frameDataPosition == (GLuint) -1)
  {
    frameDataPosition = glGetUniformBlockIndex(ShaderProgramID,"frameData");
    glUniformBlockBinding(ShaderProgramID, frameDataPosition, 1);
  }
  glBindBufferBase(GL_UNIFORM_BUFFER,1,frameDataBO);
} 

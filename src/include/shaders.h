/**
 * @file    shaders.h
 * @author  Robert Spencer <robert.spencer94@gmail.com>
 * @section DESCRIPTION
 *
 * This file contains the shader class, which is used to load shaders and set their uniform variables
 */
#ifndef SHADERS_H
#define SHADERS_H

//#include <magrathea.h>

#include <map>
#include <string>

#define VERTEX_SHADER 0x1
#define GEOMETRY_SHADER 0x2
#define TESSELATION_SHADER 0x4
#define FRAGMENT_SHADER 0x8

typedef struct
{
  glm::mat4 cameraMatrix;
  glm::vec4 cameraPosition;
  glm::mat4 projectionMatrix;
} FrameData;

static_assert(sizeof(FrameData) == 36*sizeof(GLfloat),"FrameData wrong size");

class ShaderProgram;

class ShaderManager
{
  private:
    int currentShader;
    int numShaders;
    ShaderProgram* shaders[100];
    int stypes[100];
    std::map<std::string,int> shaderNames;
    /// The position of the frame information
    GLuint frameDataPosition;
    GLuint frameDataBO;
  public:
    ShaderManager();
    int newShader(const char* name,int);
    void loadShader(int);
    ShaderProgram* getCurrentShader();
    ShaderProgram* getShaderById(int);
    FrameData frameData;
    void setFrameData();
    int addShader(ShaderProgram*,const char*);
    void reloadAll();
};

/// A shader program object handles the loading, compiling and executing of shaders (both vertex and fragment)
/// on the GPU.
class ShaderProgram
{
  private:
    /// The number OpenGL assigned to this program
    GLuint ShaderProgramID;
    /// Finds the location of a variable by name
    GLuint getVariablePosition(const char* name);
    /// A map to hold the positions in GPU memory of variables (so we don't look them up each time)
    std::map<std::string,GLuint> variableLocations;
    /// The position of the object transform matrix (changed often)
    GLuint objectDataPosition;
    /// The position of the frame information
    GLuint frameDataPosition;
  public:
    /// Construct the program (empty and unloaded)
    ShaderProgram(const char* nm);
    /// Add an object
    void LoadShader(const char*,GLenum);
    /// Link all the pieces together
    void CompileAll();
    /// Actually use this shader
    void Load();
    /// Set some uniform matrix variable
    void setInt(const char* varName, unsigned int value);
    /// Function to set the object transformation matrix etc.
    void setObjectData(GLuint);
    /// Function to set the camera transformation matrix etc.
    void setFrameData(int);
    /// An accessor to get the buffer index
    GLuint getFrameDataBufferNumber();
    /// The name of the shader.  Used for reloading purposes
    const char* name;
};

#endif 
#include <magrathea.h>
#include <graphics.h>
#include <game.h>
#include <window.h>

Window::Window(Game* parent)
{
  width = height = 600;
  game = parent;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  windowHandle = glfwCreateWindow(width, height, "Grass", NULL, NULL);
  glfwSetWindowPos(windowHandle,800,30);
  if (!windowHandle)
    DIE("Failed to create window");
  glfwMakeContextCurrent(windowHandle);
}

Window::~Window()
{
  glfwDestroyWindow(windowHandle);
}

void Window::setContext()
{
  glfwGetWindowSize(windowHandle,&width,&height);
  makePerspectiveMatrix(&game->shaderManager->frameData.projectionMatrix,float(width)/height);
  glfwMakeContextCurrent(windowHandle);
  glViewport(0,0,width,height);
}

int Window::shouldClose()
{
  return glfwWindowShouldClose(windowHandle);
}

void Window::swapBuffers()
{
  glfwSwapBuffers(windowHandle);
}

void Window::setKeyCallBack(GLFWkeyfun func)
{
  glfwSetKeyCallback(windowHandle,func);
}

void Window::setScrollCallBack(GLFWscrollfun func)
{
  glfwSetScrollCallback(windowHandle,func);
}

void Window::setTitle(const char* s)
{
  glfwSetWindowTitle(windowHandle,s);
}

glm::vec2 Window::getSize()
{
  return glm::vec2(width,height);
}

glm::vec2 Window::getMousePos()
{
  double x,y;
  glfwGetCursorPos(windowHandle,&x,&y);
  return glm::vec2((float)x,(float)y);
}

glm::vec2 Window::getMouseCentreOffset()
{
  return getMousePos() - glm::vec2(width/2.f,height/2.f);
}


void Window::setMousePos(glm::vec2 p)
{
  glfwSetCursorPos(windowHandle,p.x,p.y);
}

void Window::setMouseCentre()
{
  setMousePos(glm::vec2(width/2,height/2));
}

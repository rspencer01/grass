#ifndef WINDOW_H
#define WINDOW_H

#include <magrathea.h>

class Window
{
  private:
    GLFWwindow* windowHandle;
    Game* game;
    int width;
    int height;
  public:
    Window(Game* game);
    ~Window();
    int shouldClose();
    void setContext();
    void swapBuffers();
    void setKeyCallBack(GLFWkeyfun func);
    void setTitle(const char* s);
    glm::vec2 getSize();
    glm::vec2 getMousePos();
    glm::vec2 getMouseCentreOffset();
    void setMousePos(glm::vec2);
    void setMouseCentre();
    void setScrollCallBack(GLFWscrollfun func);
};
#endif
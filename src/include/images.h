#ifndef IMAGES_H
#define IMAGES_H

#include <magrathea.h>
#include <texture.h>

/// Constructs a virgin texture
GLuint newTexture(bool smoothTexture);
/// Constructs an opengl texture from the given file
Texture* textureFromTGA(const char*,bool);
#endif

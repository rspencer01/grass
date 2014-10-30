/**
 * @file noise.h
 * @author  Robert Spencer <robert.spencer94@gmail.com>
 * @section DESCRIPTION
 *
 * This file contains a number of functions related to the generation of noise (mainly Perlin) noise.
 */
#ifndef NOISE_H
#define NOISE_H
#include <texture.h>
// Constructs the noisy textures and loads them
void initNoise();

extern Texture* perlinNoise;
#endif

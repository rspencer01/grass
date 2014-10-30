/**
 * @file heightmap.h
 * @author  Robert Spencer <robert.spencer94@gmail.com>
 * @section DESCRIPTION
 *
 * This file describes the heightmap functions.  These functions determine the height of a certain point
 */

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

/// Returns an array of heightmap data
float* getHeightmapData(int w, int h);
float getHeight(float,float);
#endif

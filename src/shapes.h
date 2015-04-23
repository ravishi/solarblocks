//==========================================================================
// shapes.h
//--------------------------------------------------------------------------
// As formas dos tetraminos.
//==========================================================================


#ifndef SHAPES_H
#define SHAPES_H

#include "tonc.h"

#define NUM_SHAPES 7
#define NUM_ROTATIONS 4

#define SHAPE_DATA(shape, rot)  ((u8*)(&shapes[shape][rot][0][0]))

extern const u8 shapes[NUM_SHAPES][NUM_ROTATIONS][4][4];

#endif // SHAPES_H

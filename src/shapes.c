//==========================================================================
// shapes.c
//--------------------------------------------------------------------------
// As formas dos tetraminos.
//==========================================================================


#include "shapes.h"
#include "tonc.h"

const u8 shapes[NUM_SHAPES][NUM_ROTATIONS][4][4] = {
  { // L shape
    { {0,0,0,0},
	  {0,0,1,0},
	  {1,1,1,0},
	  {0,0,0,0} },
	
	{ {0,1,0,0},
	  {0,1,0,0},
	  {0,1,1,0},
	  {0,0,0,0} },

	{ {0,0,0,0},
	  {0,1,1,1},
	  {0,1,0,0},
	  {0,0,0,0} },
	
	{ {0,0,0,0},
  	  {0,1,1,0},
	  {0,0,1,0},
	  {0,0,1,0} }
   },

  { // J shape
    { {0,0,0,0},
	  {0,1,0,0},
	  {0,1,1,1},
	  {0,0,0,0} },
	
    { {0,0,1,0},
	  {0,0,1,0},
	  {0,1,1,0},
	  {0,0,0,0} },
	
    { {0,0,0,0},
	  {1,1,1,0},
	  {0,0,1,0},
	  {0,0,0,0} },
	
    { {0,0,0,0},
	  {0,1,1,0},
	  {0,1,0,0},
	  {0,1,0,0} }
   },
	
  { //O shape
	{ {0,0,0,0},
	  {0,1,1,0},
	  {0,1,1,0},
	  {0,0,0,0} },
	
	{ {0,0,0,0},
	  {0,1,1,0},
	  {0,1,1,0},
	  {0,0,0,0} },
	
	{ {0,0,0,0},
	  {0,1,1,0},
	  {0,1,1,0},
	  {0,0,0,0} },
	
	{ {0,0,0,0},
	  {0,1,1,0},
	  {0,1,1,0},
	  {0,0,0,0} }
   },
	
  { // I shape
	{ {0,0,1,0},
	  {0,0,1,0},
	  {0,0,1,0},
	  {0,0,1,0} },
	
	{ {0,0,0,0},
	  {0,0,0,0},
	  {1,1,1,1},
	  {0,0,0,0} },

    { {0,1,0,0},
	  {0,1,0,0},
	  {0,1,0,0},
	  {0,1,0,0} },

    { {0,0,0,0},
	  {1,1,1,1},
	  {0,0,0,0},
	  {0,0,0,0} }
   },
	
  { // T shape
    { {0,0,0,0},
      {0,1,1,1},
      {0,0,1,0},
      {0,0,0,0} },

    { {0,0,0,0},
	  {0,0,1,0},
	  {0,1,1,0},
	  {0,0,1,0} },

    { {0,0,0,0},
	  {0,1,0,0},
	  {1,1,1,0},
	  {0,0,0,0} },

    { {0,1,0,0},
	  {0,1,1,0},
	  {0,1,0,0},
	  {0,0,0,0} }
   },

  { // Z shape
    { {0,0,0,0},
	  {0,1,1,0},
	  {0,0,1,1},
	  {0,0,0,0} },

    { {0,0,0,0},
	  {0,0,1,0},
	  {0,1,1,0},
	  {0,1,0,0} },

    { {0,0,0,0},
	  {0,1,1,0},
	  {0,0,1,1},
	  {0,0,0,0} },

    { {0,0,0,0},
	  {0,0,1,0},
	  {0,1,1,0},
	  {0,1,0,0} }
   },
	
  { // S shape
    { {0,0,0,0},
	  {0,0,1,1},
	  {0,1,1,0},
	  {0,0,0,0} },

    { {0,0,0,0},
	  {0,1,0,0},
	  {0,1,1,0},
	  {0,0,1,0} },

    { {0,0,0,0},
	  {0,0,1,1},
	  {0,1,1,0},
	  {0,0,0,0} },
	
    { {0,0,0,0},
	  {0,1,0,0},
	  {0,1,1,0},
	  {0,0,1,0} }
   }
};
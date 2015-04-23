//==========================================================================
// area.h
//--------------------------------------------------------------------------
// A área onde os tetraminos vão caindo e se empilhando.
//==========================================================================


#ifndef AREA_H
#define AREA_H

#include "tonc.h"
#include "tetramino.h"



// Attribute 1, regular sprites
#define ATTR1_BUILD_R(x, size, hflip, vflip)         \
( ((x)&511) | (((hflip)&1)<<12) | (((vflip)&1)<<13) | (((size)&3)<<14) )

typedef SCR_ENTRY AREA_ENTRY;



extern struct tag__area
{
    int         x, y, width, height;
    AREA_ENTRY  buffer[20*30];
    u16         ctrl;
    u8          mod;
} __area;

extern struct tag__actt
{
    int         x, y;
    TTetra      tetra;
    OBJ_ATTR    *obuff;
} __actt;



// TODO: checagem de limites nestas macros (PONDER: torná-las funções?)

#define area_plot(_x, _y, _block)   \
    if (_y >= 0 && _x >= 0)           \
        *(__area.buffer + (_y) * (__area.width) + (_x)) = _block

#define area_get_block(_x, _y)  \
    (_x >= 0 && _y >= 0           \
     ?  *(__area.buffer + (_y) * (__area.width) + (_x))  \
     :  0)

void area_init(int bgnr, int bgcnt);

void area_set_area(int x, int y, int width, int height);

void area_blit(int x, int y, int width, int height, void *buffer, int fun);

void area_update();

void area_get_start_pos(int *x, int *y);

int area_get_full_row();

void area_remove_row(int row);




// === Active Tetramino Stuff ==============================================

// TODO: fazer isso aqui de outro jeito. set/get functions?
#define active_tetra()  (__actt)


void active_tetra_init(void *obj_buffer, int obi);

void active_tetra_set_tetra(TTetra t);

void active_tetra_set_pos(int x, int y);

void active_tetra_update();

void active_tetra_rotate(int factor);

void active_tetra_fix();


TTetra active_tetra_get_tetra();

void active_tetra_get_pos(int *x, int *y);


// -- colisões

#define BLK_NONE        0
#define BLK_LEFT        1
#define BLK_RIGHT       2
#define BLK_TOP         4
#define BLK_BOTTOM      8

#define BLK_HORZ        3
#define BLK_VERT        12

int active_tetra_collide();

#endif // AREA_H

//==========================================================================
// area.c
//--------------------------------------------------------------------------
// A área onde os tetraminos vão caindo e se empilhando.
//==========================================================================


#include "tonc.h"
#include "area.h"
#include "shapes.h"
#include "posprintf.h"

struct tag__area __area;
struct tag__actt __actt;

void area_init(int bgnr, int bgcnt)
{
    __area.width = 30;
    __area.height = 20;
    __area.x = __area.y = 0;
    __area.mod = true;

    // preenche o buffer com zeros
    toncset32(__area.buffer, 0, sizeof(__area.buffer)/4);

    __area.ctrl = bgcnt;
    REG_BGCNT[bgnr] = bgcnt;
}

void area_set_area(int x, int y, int width, int height)
{
    __area.x = x;
    __area.y = y;
    __area.width = width;
    __area.height = height;
    __area.mod = true;
}

void area_blit(int x, int y, int width, int height, void *buffer, int fun)
{
    int xx, yy;
    AREA_ENTRY b;
    for (yy = 0; yy < height; ++yy)
        for (xx = 0; xx < width; ++xx)
        {
            b = *(((AREA_ENTRY*) buffer) + yy * width + xx);
            if (fun == 1 || b != 0)
                area_plot(x + xx, y + yy, b);
        }
    __area.mod = true;
}

void area_update()
{
    if (__area.mod == false)
        return;

    int yy;
    for (yy = 0; yy < __area.height; ++yy)
    {
        memcpy32
            (
                se_mem[BFN_GET(__area.ctrl, BG_SBB)] + 32 * yy + __area.x,
                __area.buffer + yy * __area.width,
                __area.width * sizeof(AREA_ENTRY) / 4
            );
    }

    __area.mod = false;
}

void area_get_start_pos(int *x, int *y)
{
    *x = (__area.width - 4)/2;
    *y = 0;
}

int area_get_full_row()
{
    int x, y, full;
    for (y = __area.height; y >= 0; --y)
    {
        full = true;
        for (x=0; x < __area.width; ++x)
        {
            if (area_get_block(x, y) <= 0) full = false;
        }
        if (full == true)
            return y;
    }
    return -1;
}

void area_remove_row(int row)
{
    int y;
    
    // ir copiando para a linha y a linha de cima
    for (y = row; y >= 1; --y)
    {
        memcpy32
            (
                __area.buffer + y * __area.width,
                __area.buffer + (y-1) * __area.width,
                sizeof(__area.buffer[0]) * __area.width / 4
            );
    }

    // limpar a primeira linha
    memset32(__area.buffer, 0x0, sizeof(__area.buffer[0]) * __area.width / 4);
    __area.mod = true;
}



// === Active Tetramino Stuff ==============================================

void active_tetra_init(void *obj_buffer, int obi)
{
    int i;
    OBJ_ATTR *obuff = (OBJ_ATTR*)obj_buffer + obi;
    for (i=0; i<16; ++i)
    {
        obj_set_attr(&obuff[i],
                ATTR0_8BPP | ATTR0_SQUARE | ATTR0_HIDE,    // 8bpp Regular Square
                0,                          // size = 1x1t
                ATTR2_PALBANK(0) | 0        // palbank 0, tile 2
            );
    }
    __actt.obuff = obuff;
}

void active_tetra_set_tetra(TTetra t)
{
    __actt.tetra = t;
}

void active_tetra_set_pos(int x, int y)
{
    __actt.x = x;
    __actt.y = y;
}

void active_tetra_update()
{
    int i;
    TTetra t = __actt.tetra;
    for (i=0; i<16; ++i)
    {
        // TODO: código independente da bitdepth dos sprites
        __actt.obuff[i].attr2 = ATTR2_PALBANK(0) | (t.color) * 2;

        // atualizar a posição de cada bloquinho
        obj_set_pos(&__actt.obuff[i],
                (__area.x + __actt.x)*8 + (i%4) * 8,
                (__area.y + __actt.y)*8 + (i/4) * 8 );

        // ligar/desligar os sprites
        __actt.obuff[i].attr0 &= ~(3 << 8);
        __actt.obuff[i].attr0 |=
                *(&shapes[t.shape][t.rot][0][0] + i) == 1
                ? ATTR0_REG
                : ATTR0_HIDE;
    }
}

/**
 * Sistema inteligente de rotação. Incrementa ou decrementa x e/ou y
 * se necessário e possível.
 *
 * (PONDER: usar recursividade melhoraria esta função?)
 */
void active_tetra_rotate(int factor)
{
    int newrot = __actt.tetra.rot + factor;
    if (newrot == NUM_ROTATIONS)
        newrot = 0;
    else if (newrot < 0)
        newrot = NUM_ROTATIONS - 1;

    int x, y;
    const u8 *s = SHAPE_DATA(__actt.tetra.shape, newrot);
    int rl, rr, rt, rb;
    rl = rr = rt = rb = -1;
    for (y=0; y<4; ++y)
        for (x=0; x<4; ++x)
        {
            if (*(s + y*4 + x) == 1)
            {
                if (rt == -1) rt = y;
                else if (y < rt) rt = y;
                else if (y > rb) rb = y;
                if (rl == -1) rl = x;
                else if (x < rl) rl = x;
                else if (x > rr) rr = x;

                // TODO: sistema de rotação inteligente (aumentar/
                // reduzir x e/ou y automaticamente)
                if (area_get_block(__actt.x + x, __actt.y + y) > 0)
                    return;
            }
        }

    // erro nas bordas
    while (__actt.x + rl < 0)
        ++ __actt.x;
    while (__actt.x + rr >= __area.width)
        -- __actt.x;
    while (__actt.y + rt < 0)
        ++__actt.y;
    while (__actt.y + rb >= __area.height)
        --__actt.y;

    __actt.tetra.rot = newrot;
}

void active_tetra_fix()
{
    SCR_ENTRY data[16];
    tetra_get_se_data(__actt.tetra, data);

    int i;
    for (i=0; i<16; ++i)
        if (data[i] > 0)
        {
            if (data[i] > 8)
                data[i] += 1;
            else
                data[i] += 7;
        }

    area_blit(__actt.x, __actt.y, 4, 4, data, 0);
}

TTetra active_tetra_get_tetra()
{
    return __actt.tetra;
}

void active_tetra_get_pos(int *x, int *y)
{
    *x = __actt.x;
    *y = __actt.y;
}


//-------------------------------------------------
// colisões
//-------------------------------------------------

int active_tetra_collide()
{
    int col = 0;

    // Dois coelhos com um loop só.
    // Neste loop, calculamos a real área ocupada por um
    // tetramino e ao mesmo tempo, verificamos colisões com
    // a construção.
    int x, y;
    const u8 *s = SHAPE_DATA(__actt.tetra.shape, __actt.tetra.rot);
    int rl, rr, rt, rb;
    rl = rr = rt = rb = -1;
    for (y=0; y<4; ++y)
        for (x=0; x<4; ++x)
        {
            if (*(s + y*4 + x) == 1)
            {
                if (rt == -1) rt = y;
                else if (y < rt) rt = y;
                else if (y > rb) rb = y;
                if (rl == -1) rl = x;
                else if (x < rl) rl = x;
                else if (x > rr) rr = x;

                // colisão com a construção
                if (area_get_block(__actt.x + x + 1, __actt.y + y) > 0)
                    col |= BLK_RIGHT;
                if (area_get_block(__actt.x + x - 1, __actt.y + y) > 0)
                    col |= BLK_LEFT;
                if (area_get_block(__actt.x + x, __actt.y + y + 1) > 0)
                    col |= BLK_BOTTOM;
                if (area_get_block(__actt.x + x, __actt.y + y - 1) > 0)
                    col |= BLK_TOP;
            }
        }

    // --- colisões com as bordas -----------------------------------
    if (__actt.x + rl - 1 < 0)
        col |= BLK_LEFT;
    else if (__actt.x + rr + 1 >= __area.width)
        col |= BLK_RIGHT;

    if (__actt.y + rt - 1 < 0)
        col |= BLK_TOP;
    else if (__actt.y + rb + 1 >= __area.height)
        col |= BLK_BOTTOM;

    return col;
}

//==========================================================================
// tetramino.c
//--------------------------------------------------------------------------
// Funções para desenhar os tetraminhos e talz.
//==========================================================================

#include "tonc.h"
#include "shapes.h"
#include "tetramino.h"


TTetra mktetra(int shape, int clr, int rot)
{
    TTetra tetra = {shape, clr, rot};
    return tetra;
}

TTetra mk_rand_tetra()
{
    TTetra t;
    int count = 0;

    t.shape = qran_range(0, 6);
    t.color = qran_range(1, 7);
    t.rot = qran_range(0, 3);
    return t;
}

int tetra_get_se_data(TTetra tetra, void *retbuf)
{
    int i;

    for (i=0; i<16; ++i)
    {
        *( ((SCR_ENTRY*) retbuf) + i) = *(&shapes[tetra.shape][tetra.rot][0][0] + i) == 1
            ? (SCR_ENTRY) tetra.color
            : 0;
    }

    return 1;
}

void tetra_get_real_area(TTetra tetra, int *rt, int *rb, int *rl, int *rr)
{
    int x, y;
    const u8 *s = SHAPE_DATA(tetra.shape, tetra.rot);
    *rl = *rr = *rt = *rb = -1;
    for (y=0; y<4; ++y)
        for (x=0; x<4; ++x)
        {
            if (*(s + y*4 + x) == 1)
            {
                if (*rt == -1) *rt = y;
                else if (y < *rt) *rt = y;
                else if (y > *rb) *rb = y;
                if (*rl == -1) *rl = x;
                else if (x < *rl) *rl = x;
                else if (x > *rr) *rr = x;
            }
        }
}

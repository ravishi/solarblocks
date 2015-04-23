#ifndef TETRAMINOS_H
#define TETRAMINOS_H



enum
{
    TETRA_SHAPE_L = 0,
    TETRA_SHAPE_J,
    TETRA_SHAPE_O,
    TETRA_SHAPE_I,
    TETRA_SHAPE_T,
    TETRA_SHAPE_Z,
    TETRA_SHAPE_S
};

typedef struct tagTTetra
{
    u8  shape, color, rot;
} TTetra;


TTetra mktetra(int shape, int clr, int rot);
TTetra mk_rand_tetra();
int tetra_get_se_data(TTetra tetra, void *retbuf);
void tetra_get_real_area(TTetra tetra, int *rt, int *rb, int *rl, int *rr);

#endif // TETRAMINOS_H

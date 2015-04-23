#include "tonc.h"
#include "scene_title.h"
#include "scene_stage.h"
#include "blocks.h"

void scene_title()
{
    RegisterRamReset(RESET_GFX | RESET_REG_MASK);
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    REG_DISPCNT= DCNT_MODE0 | DCNT_BG0;
    REG_BG0CNT = BG_CBB(0) | BG_SBB(31);

    //memset16(pal_bg_mem+1, CLR_WHITE, 15);
    pal_bg_bank[0][15] = pal_bg_bank[0][1] = CLR_WHITE;

    // --- (1) Base TTE init for tilemaps ---
    tte_init_se(
        0,                      // Background number (BG 0)
        BG_CBB(0)|BG_SBB(31),   // BG control (for REG_BGxCNT)
        0,                      // Tile offset (special cattr)
        CLR_WHITE,              // Ink color
        14,                     // BitUnpack offset (on-pixel = 15)
        NULL,                   // Default font (sys8) 
        NULL);                  // Default renderer (se_drawg_s)

    tte_set_pos(72, 72);
    tte_write("PRESS START");

    profile_start();
    key_wait_till_hit(KEY_START);

    // gerar uma seed única
    sqran(profile_stop());

    // cair fora daqui
    scene_stage();
}

//==========================================================================
// scene_stage.c
//--------------------------------------------------------------------------
// Enquanto o jogador empilha os blocos...
//==========================================================================
// === NOTAS ===
// TODO: centralizar os tetraminos na janelinha de NEXT 


#include "tonc.h"
#include "blocks.h"
#include "tetramino.h"
#include "area.h"
#include "scene_stage.h"
#include "shapes.h"
#include "posprintf.h"
#include <string.h>


#define XSPD_BASE   38
#define YSPD_BASE   64
#define SPR_COUNT   17



OBJ_ATTR obj_buffer[128];

TTetra next;

int sort_keys[SPR_COUNT];      // sort keys
u8 sort_ids[SPR_COUNT];        // sorted OAM indices


// --- funções que não deveriam estar aqui ---------------------------------

// gerar um tetramino sempre diferente
TTetra rand_tetra_ex(int pseudoseed)
{
    TTetra t = mk_rand_tetra();

    if (t.shape == active_tetra().tetra.shape)
    {
        sqran(pseudoseed);
        t = mk_rand_tetra();
    }

    return t;
}

// --- Funções bacanas -----------------------------------------------

// novo tetramino
void scene_stage_new_tetra(int frame)
{
    int sx, sy;
    int rt, rb, rl, rr;

    area_get_start_pos(&sx, &sy);

    tetra_get_real_area(next, &rt, &rb, &rl, &rr);

    active_tetra_set_tetra(next);
    active_tetra_set_pos(sx, sy - rb);

    // --- update the next indicator
    next = rand_tetra_ex(frame);

    int i;
    const u8 *s = SHAPE_DATA(next.shape, next.rot);
    for (i=0; i<16; ++i)
    {
        if (s[i] == 1)
            memcpy32(tile8_mem[4] + 17 + i,
                    ((const u8*)blocksTiles) + 0x40 * next.color,
                    0x10);
        else
            memcpy32(tile8_mem[4] + 17 + i,
                    blocksTiles,
                    0x10);
    }
}

void scene_stage_gfx_init()
{
    RegisterRamReset(RESET_GFX | RESET_REG_MASK);
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);

    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG1CNT = BG_CBB(0) | BG_SBB(18) | BG_8BPP | BG_REG_32x32;
    oam_init(obj_buffer, 128);

    tte_init_chr4c(
        0,
        BG_CBB(1) | BG_SBB(28) | BG_4BPP,
        0,
        bytes2word(3,1,1,1),
        3,
        &verdana9Font,
        (fnDrawg) chr4c_drawg_b1cts_fast);
    tte_init_con();

    // iniciar os bloquinhos no fundo
    memcpy32(pal_bg_mem, blocksPal, blocksPalLen/4);
    memcpy32(tile_mem[0], blocksTiles, blocksTilesLen/4);

    // iniciar os bloquinhos na memória dos objetos
    memcpy32(tile_mem[4], blocksTiles, blocksTilesLen/4);
    memcpy32(pal_obj_mem, blocksPal, blocksPalLen/4);
}


void scene_stage_draw_gui()
{
    int x, y;
    for (x=0; x<32; ++x)
        for (y=0; y<32; ++y)
        {
            se_mem[28][y*32 + x] |= 15 << 0xc;
        }
    pal_bg_bank[0][0] = CLR_BLACK;
    pal_bg_bank[15][0] = CLR_BLACK;
    pal_bg_bank[15][1] = RGB15(9, 9, 14);
    pal_bg_bank[15][2] = CLR_BLACK;
    pal_bg_bank[15][3] = CLR_WHITE;

    TSurface *gui_srf = tte_get_surface();
    schr4c_rect(gui_srf,    0, 0, 6 *8, 160, 1);
    schr4c_rect(gui_srf, 18*8, 0,  240, 160, 1);

    tte_set_pos(12, 8);
    tte_write("NEXT");
    schr4c_rect(gui_srf,   8,  24,  40,  56, 0);

    // "NEXT" image
    obj_set_attr(&obj_buffer[0x11],
            ATTR0_8BPP | ATTR0_SQUARE | ATTR0_REG,
            2 << 0xe,
            ATTR2_PALBANK(0) | 17*2);
    obj_set_pos(&obj_buffer[0x11], 8, 24);
}


// Um efeito bacana de fade-in para preto quando
// o cara aperta START.
void scene_stage_pause()
{
    // salvar o estado anterior do TTE
    TTC ctx;
    TSurface srf;
    memcpy(&srf, tte_get_surface(), sizeof(srf));
    memcpy(&ctx, tte_get_context(), sizeof(ctx));

    // TODO: escrever PAUSE

    REG_BLDCNT= BLD_BUILD(
        BLD_BG1 | BLD_BG0 | BLD_OBJ,    // Top layers
        BLD_BG3,                        // Bottom layers
        3);                             // Mode
    REG_BLDY = BLDY_BUILD(5);

    // esperar até que cara aperte Start novamente
    key_wait_till_hit(KEY_START);

    // voltar ao estado anterior
    REG_BLDCNT = BLD_BUILD(0, 0, 0);
    memcpy(tte_get_surface(), &srf, sizeof(srf));
    memcpy(tte_get_context(), &ctx, sizeof(ctx));
}


void scene_stage()
{
    scene_stage_gfx_init();

    area_init(1, REG_BG1CNT);
    area_set_area(6, 0, 12, 20);

    active_tetra_init(obj_buffer, 1);

    scene_stage_draw_gui();

    // -----------------------
    next = rand_tetra_ex(qran());
    scene_stage_new_tetra(qran());
    int frame = 0;
    float vx, vy;
    int lv = 1;
    int lines = 0;
    char strbuf[80];
    int xSpeedThreshold, xSpeedCounter = 0;
    int ySpeedThreshold, ySpeedCounter = 0;
    while (1)
    {
        VBlankIntrWait();

        // --- atualização da velocidade -----
        vx = vy = 0;

        if (xSpeedThreshold == 0)
        {
            xSpeedThreshold = XSPD_BASE;
        }
        else
        {
            xSpeedThreshold = (int)(xSpeedThreshold * 0.97);
        }

        if (ySpeedThreshold == 0)
        {
            ySpeedThreshold = YSPD_BASE - lv;
        }
        else if (ySpeedThreshold > 5 * (lv > 10 ? 1 : 0.7))
        {
            ySpeedThreshold = (int)(ySpeedThreshold * 0.8);
        }


        // --- teclas -----

        key_poll();
       
        // --- Left/Right -----
        if (key_hit(KEY_LEFT))
                vx --;
        else if (key_hit(KEY_RIGHT))
                vx ++;
        else if (key_is_down(KEY_LEFT))
        {
            xSpeedCounter++;
            if (xSpeedCounter > xSpeedThreshold)
            {
                xSpeedCounter = 0;
                vx --;
            }
        }
        else if (key_is_down(KEY_RIGHT))
        {
            xSpeedCounter++;
            if (xSpeedCounter > xSpeedThreshold)
            {
                xSpeedCounter = 0;
                vx ++;
            }
        }
        else
            xSpeedThreshold = xSpeedCounter = 0;

        // --- Down ------        
        if (key_is_down(KEY_DOWN))
        {
            ySpeedCounter++;
            if (ySpeedCounter > ySpeedThreshold)
            {
                ySpeedCounter = 0;
                vy++;
            }
        }
        else
        {
            ySpeedCounter++;
            if (ySpeedCounter > ySpeedThreshold)
            {
                ySpeedCounter = 0;
                vy++;
            }
            ySpeedThreshold = 0;
        }

        // --- START -------
        if (key_hit(KEY_START))
        {
            scene_stage_pause();
        }

        if (key_hit(KEY_A))
            active_tetra_rotate(1);
        else if (key_hit(KEY_B))
            active_tetra_rotate(-1);

        // --- teste de colisões ------------

        // TODO: corrigir bug com as colisões diagonais
        int col = 0;
        if (vx != 0 || vy != 0)
            col = active_tetra_collide();

        if (vy > 0)
        {
            if ((col & BLK_BOTTOM) == 0)
                active_tetra().y += vy;
            else
            {
                // verificar a posição do tetramino atual
                // se for negativa, já era
                if (active_tetra().y < 0)
                {
                    int rt, rb, rl, rr;
                    tetra_get_real_area(active_tetra().tetra, &rt, &rb, &rl, &rr);
                    if (active_tetra().y + rt + 1< 0)
                    {
                        VBlankIntrDelay(10);
                        // fade-out para preto
                        scene_title();
                    }
                }
                active_tetra_fix();
                VBlankIntrDelay(15);
                scene_stage_new_tetra(frame);

                // --- linhas completas -----------
        
                int fr;
                while ((fr = area_get_full_row()) != -1)
                {
                    lines++;
                    area_remove_row(fr);
                    VBlankIntrDelay(5);
                    area_update();
                }
            }
        }

        if (vx > 0)
        {
            if ((col & BLK_RIGHT) == 0)
                active_tetra().x += vx;
        }
        else if (vx < 0)
            if ((col & BLK_LEFT) == 0)
                active_tetra().x += vx;



        // --- placar -----------

        if (lines*12 > lv*100) lv++;

        posprintf(strbuf, "LEVEL:  %03d\nSCORE: %04d\nLINES: %04d", lv, lines*12, lines);
        tte_set_margins(19*8, 0, 240, 160);
        chr4c_erase(19*8, 0, 240, 160);
        tte_set_pos(19*8, 0);
        tte_write(strbuf);

        // --- o resto das coisas ---

        area_update();
        active_tetra_update();

        oam_copy(oam_mem, obj_buffer, 0x12);

        frame++;
    }
}

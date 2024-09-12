#include "gametank.h"
#include "drawing_funcs.h"
#include "input.h"
#include "persist.h"
#include "music.h"
#include "banking.h"
#include "gen/assets/font.h"
#include "gen/assets/shotgun.h"

#pragma data-name (push, "SAVE")
char saved_pos[4] = {30, 40, 1, 1};
#pragma data-name (pop)

unsigned char padding[8] = { 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55 };
unsigned char tilemap[256] = {
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,  0,  0,255,  0,  0,  0,  0,  0,  0,  0,  0,255,  0,  0,255,
  255,  0,  0,255,  0,  8,255,  0,  0,255,  9,  0,255,  0,  0,255,
  255,  0,  0,  0,  0,255,255,  0,  0,255,255,  0,  0,  0,  0,255,
  255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,
  255,  0,255,255,255,255,  0,  0,  0,  0,255,255,255,255,  0,255,
  255,  0,  0,  0,  0,255,  0,  0,  0,  0,255,  0,  0,  0,  0,255,
  255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,
  255,  0,  0,  0,  0,255,  0,  0,  0,  0,255,  0,  0,  0,  0,255,
  255,  0,255,255,255,255,  0,  0,  0,  0,255,255,255,255,  0,255,
  255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,
  255,  0,  0,  0,  0,255,255,  0,  0,255,255,  0,  0,  0,  0,255,
  255,  0,  0,255,  0, 10,255,  0,  0,255, 11,  0,255,  0,  0,255,
  255,  0,  0,255,  0,  0,  0,  0,  0,  0,  0,  0,255,  0,  0,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

unsigned char player_ptr_default[4] = {17, 222, 30, 209};
unsigned char player_x_default[4] = { 8, 112, 112, 8 }, player_y_default[4] = { 8, 104, 8, 104};
unsigned char player_tile_default[4] = { 8, 96, 64, 40 };
unsigned char player_score[4] = { 0, 0, 0, 0 };
unsigned char player_score_tile[4] = { 0, 0, 0, 0 };

unsigned char player_ptr[4] = {17, 222, 30, 209};
unsigned char player_x[4] = { 8, 112, 112, 8 }, player_y[4] = { 8, 104, 8, 104};
unsigned char player_tile[4] = { 8, 96, 64, 40 };
unsigned char bullet_x[4] = {0, 0, 0, 0}, bullet_y[4] = {0, 0, 0, 0};
unsigned char bullet_ptr[4] = {0, 0, 0, 0};

unsigned char bullets[4] = { 0, 0, 0, 0 };
unsigned char dir_x[4] = { 0, 0, 0, 0 };
unsigned char dir_y[4] = { 0, 0, 0, 0 };
unsigned char dir[4] = { 0, 0, 0, 0 };

unsigned char bullet_dir_x[4] = { 0, 0, 0, 0 };
unsigned char bullet_dir_y[4] = { 0, 0, 0, 0 };

unsigned char gun[4] = { 0, 0, 0, 0 };  // 0 means there is a gun
unsigned char *tile;
unsigned char tmp, tmp2;
unsigned char player_tilemap;

#define MOVE_PLAYER(nb, flag, mask) \
        if (dir_x[nb] != 0) {    \
            if (dir_x[nb] == 1) {    \
                if (((player_x[nb] & 7) == 0) && (tilemap[player_ptr[nb]-1] == 0xFF)) {   \
                    dir_x[nb] = 0;   \
                } else {    \
                    player_x[nb]--;  \
                    tmp = player_x[nb] & 7;  \
                    if (tmp == 4) { \
                        tilemap[player_ptr[nb]] &= mask;     \
                        player_ptr[nb]--;    \
                        tmp2 = tilemap[player_ptr[nb]];  \
                        tmp = tmp2 & 0xF;       \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tmp2 &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        tilemap[player_ptr[nb]] = tmp2 | flag;   \
                    } else if (tmp == 0) {  \
                        dir_x[nb] = 0;   \
                    }   \
                }   \
            } else {    \
               if (((player_x[nb] & 7) == 0) && (tilemap[player_ptr[nb]+1] == 0xFF)) {    \
                    dir_x[nb] = 0;   \
                } else {    \
                    player_x[nb]++;  \
                    tmp = player_x[nb] & 7;  \
                    if (tmp == 4) { \
                        tilemap[player_ptr[nb]] &= mask; \
                        player_ptr[nb]++;    \
                        tmp2 = tilemap[player_ptr[nb]];  \
                        tmp = tmp2 & 0xF;   \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tmp2 &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        tilemap[player_ptr[nb]] = tmp2 | flag;   \
                    } else if (tmp == 0) {  \
                        dir_x[nb] = 0;   \
                    }   \
                }   \
            }   \
        } else if (dir_y[nb] != 0) { \
            if (dir_y[nb] == 1) {    \
                if (((player_y[nb] & 7) == 0) && (tilemap[player_ptr[nb]-16] == 0xFF)) {  \
                    dir_y[nb] = 0;   \
                } else {    \
                    player_y[nb]--;  \
                    tmp = player_y[nb] & 7;  \
                    if (tmp == 4) {     \
                        tilemap[player_ptr[nb]] &= mask;     \
                        player_ptr[nb]-=16;  \
                        tmp2 = tilemap[player_ptr[nb]];  \
                        tmp = tmp2 & 0xF;   \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tmp2 &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        tilemap[player_ptr[nb]] = tmp2 | flag;       \
                    } else if (tmp == 0) {  \
                        dir_y[nb] = 0;   \
                    }   \
                }   \
            } else {    \
               if (((player_y[nb] & 7) == 0) && (tilemap[player_ptr[nb]+16] == 0xFF)) {   \
                    dir_y[nb] = 0;   \
                } else {    \
                    player_y[nb]++;  \
                    tmp = player_y[nb] & 7;  \
                    if (tmp == 4) {         \
                        tilemap[player_ptr[nb]] &= mask;    \
                        player_ptr[nb]+=16;  \
                        tmp2 = tilemap[player_ptr[nb]];  \
                        tmp = tmp2 & 0xF;   \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tmp2 &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        tilemap[player_ptr[nb]] = tmp2 | flag;   \
                    } else if (tmp == nb) {  \
                        dir_y[nb] = 0;   \
                    }   \
                }   \
            }   \
        }

void breakpoint() {}
void hit();

int main () {
    init_graphics();
    init_dynawave();
    init_music();

    flip_pages();
    clear_border(0);
    await_draw_queue();
    flip_pages();
    await_draw_queue();
    clear_border(0);

    load_spritesheet(&ASSET__font__bios8_bmp, 0);
    load_spritesheet(&ASSET__shotgun__tiles_bmp, 2);
    load_spritesheet(&ASSET__shotgun__tilemap_bmp, 1);

    change_rom_bank(SAVE_BANK_NUM);

    __asm__("_loop: nop");
    while (1) {                                     //  Run forever
        clear_screen(0);
        clear_border(0);

        draw_sprite(0, 0, 127, 127, 0, 0, 1);

        draw_sprite(player_x[0], player_y[0]-1, 8, 9, player_tile[0], 8, 2);
        draw_sprite(player_x[1], player_y[1]-1, 8, 9, player_tile[1], 8, 2);
        draw_sprite(player_x[2], player_y[2]-1, 8, 9, player_tile[2], 8, 2);
        draw_sprite(player_x[3], player_y[3]-1, 8, 9, player_tile[3], 8, 2);
        draw_sprite(16, 120, 8, 7, player_score_tile[0], 24, 2);
        draw_sprite(48, 120, 8, 7, player_score_tile[1], 24, 2);
        draw_sprite(80, 120, 8, 7, player_score_tile[2], 24, 2);
        draw_sprite(112, 120, 8, 7, player_score_tile[3], 24, 2);
        if (!gun[0]) { draw_sprite(40, 16, 8, 8, 56, 0, 2); } else {
            gun[0]--;
            if (gun[0] == 0) {
                tmp = tilemap[37] & 0xF0;
                tilemap[37] = 8 | tmp;
            }
        }
        if (!gun[1]) { draw_sprite(80, 16, 8, 8, 56, 0, 2); } else {
            gun[1]--;
            if (gun[1] == 0) {
                tmp = tilemap[42] & 0xF0;;
                tilemap[42] = 9 | tmp;
            }
        }
        if (!gun[2]) { draw_sprite(40, 96, 8, 8, 56, 0, 2); } else {
            gun[2]--;
            if (gun[2] == 0) {
                tmp = tilemap[197] & 0xF0;;
                tilemap[197] = 10 | tmp;
            }
        }
        if (!gun[3]) { draw_sprite(80, 96, 8, 8, 56, 0, 2); } else {
            gun[3]--;
            if (gun[3] == 0) {
                tmp = tilemap[202] & 0xF0;;
                tilemap[202] = 11 | tmp;
            }
        }

        if (bullets[0]) {
            draw_sprite(player_x[0]+4, player_y[0]+1, 8, 8, 56, 0, 2);
        }
        if (bullets[1]) {
            draw_sprite(player_x[1]+4, player_y[1]+1, 8, 8, 56, 0, 2);
        }

        MOVE_PLAYER(0, 0x10, 0xEF)
        MOVE_PLAYER(1, 0x20, 0xCF)
        MOVE_PLAYER(2, 0x40, 0xBF)
        MOVE_PLAYER(3, 0x80, 0x7F)

        //Animating bullets
        if (bullet_dir_x[0] != 0) {
            // Going left
            if (bullet_dir_x[0] == 1) {
                if (((bullet_x[0] & 7) == 0) && (tilemap[bullet_ptr[0]-1] == 0xFF)) {
                    bullet_dir_x[0] = 0;
                    bullet_x[0] = 0;
                } else {
                    bullet_x[0]--;
                    bullet_x[0]--;
                    tmp = bullet_x[0] & 7;
                    if (tmp == 6) {
                        bullet_ptr[0]--;
                    }
                }
            // Going right
            } else {
                tmp = tilemap[bullet_ptr[0]];
                tmp2 = tmp & 0xE0;
                // We hit someone!
                if (tmp == 0xFF) {
//
//                if (((bullet_x[0] & 7) == 6) && (tilemap[bullet_ptr[0]+1] == 0xFF)) {
                    bullet_dir_x[0] = 0;
                    bullet_x[0] = 0;
                } else if (tmp2) {
                    hit();
                } else {
                    bullet_x[0]++;
                    bullet_x[0]++;
                    tmp = bullet_x[0] & 7;
                    if (tmp == 0) {
                        bullet_ptr[0]++;
                    }
                }
            }
            draw_box(bullet_x[0], bullet_y[0], 1, 1, 7);
        } else if (bullet_dir_y[0] != 0) {
            // Going up
            if (bullet_dir_y[0] == 1) {
                if (((bullet_y[0] & 7) == 0) && (tilemap[bullet_ptr[0]-16] == 0xFF)) {
                    bullet_dir_y[0] = 0;
                    bullet_x[0] = 0;
                } else {
                    bullet_y[0]--;
                    bullet_y[0]--;
                    tmp = bullet_y[0] & 7;
                    if (tmp == 6) {
                        bullet_ptr[0]-= 16;
                    }
                }
            // Going down
            } else {
               if (((bullet_y[0] & 7) == 6) && (tilemap[bullet_ptr[0]+16] == 0xFF)) {
                    bullet_dir_y[0] = 0;
                    bullet_x[0] = 0;
                } else {
                    bullet_y[0]++;
                    bullet_y[0]++;
                    tmp = bullet_y[0] & 7;
                    if (tmp == 0) {
                        bullet_ptr[0]+=16;
                    }
                }
            }
            draw_box(bullet_x[0], bullet_y[0], 1, 1, 7);
        }

        cursorX = 4;
        cursorY = 4;
        print_hex_num(player_ptr[0]);

        await_draw_queue();
        sleep(1);
        flip_pages();
        update_inputs();
        if(player1_buttons & INPUT_MASK_LEFT && dir_y[0] == 0) {
            dir_x[0] = 1;
            dir[0] = 1;
            player_tile[0] = 0;
        } else if (player1_buttons & INPUT_MASK_RIGHT && dir_y[0] == 0) {
            dir_x[0] = 2;
            dir[0] = 0;
            player_tile[0] = 8;
        } else if (player1_buttons & INPUT_MASK_UP && dir_x[0] == 0) {
            dir_y[0] = 1;
            dir[0] = 2;
            player_tile[0] = 24;
        } else if (player1_buttons & INPUT_MASK_DOWN && dir_x[0] == 0) {
            dir_y[0] = 2;
            dir[0] = 3;
            player_tile[0] = 16;
        }

        if(player2_buttons & INPUT_MASK_LEFT && dir_y[1] == 0) {
            dir_x[1] = 1;
            dir[1] = 1;
            player_tile[1] = 96;
        } else if (player2_buttons & INPUT_MASK_RIGHT && dir_y[1] == 0) {
            dir_x[1] = 2;
            dir[1] = 0;
            player_tile[1] = 104;
        } else if (player2_buttons & INPUT_MASK_UP && dir_x[1] == 0) {
            dir_y[1] = 1;
            dir[1] = 2;
            player_tile[1] = 120;
        } else if (player2_buttons & INPUT_MASK_DOWN && dir_x[1] == 0) {
            dir_y[1] = 2;
            dir[1] = 3;
            player_tile[1] = 112;
        }

        if (bullets[0] != 0 && bullet_x[0] == 0 && (player1_buttons & ~player1_old_buttons & INPUT_MASK_A)) {
            bullets[0]--;
            breakpoint();
            switch(dir[0]) {
                // Shoot right
                case 0:
                    bullet_x[0] = player_x[0]+8;
                    bullet_y[0] = player_y[0]+4;
                    bullet_ptr[0] = player_ptr[0] + 1;
                    bullet_dir_x[0] = 2;
                    bullet_dir_y[0] = 0;
                    break;
                // Shoot left
                case 1:
                    bullet_x[0] = player_x[0]-2;
                    bullet_y[0] = player_y[0]+4;
                    bullet_ptr[0] = player_ptr[0] - 1;
                    bullet_dir_x[0] = 1;
                    bullet_dir_y[0] = 0;
                    break;
                // Shoot up
                case 2:
                    bullet_x[0] = player_x[0]+4;
                    bullet_y[0] = player_y[0]-2;
                    bullet_ptr[0] = player_ptr[0] - 16;
                    bullet_dir_x[0] = 0;
                    bullet_dir_y[0] = 1;
                    break;
                // Shoot down
                case 3:
                    bullet_x[0] = player_x[0]+4;
                    bullet_y[0] = player_y[0]+8;
                    bullet_ptr[0] = player_ptr[0] + 16;
                    bullet_dir_x[0] = 0;
                    bullet_dir_y[0] = 2;
                    break;
            }
            breakpoint();
            if (tilemap[bullet_ptr[0]] == 0xFF) {
                bullet_x[0] = 0;
                bullet_dir_x[0] = 0;
                bullet_dir_y[0] = 0;
            }
        }
    }

  return (0);                                     //  We should never get here!
}

void hit() {
    if (tmp2 & 0x10) {
        tmp = 0;
    }
    tilemap[bullet_ptr[0]] &= 0xDF;
    player_ptr[1] = player_ptr_default[1];
    player_x[1] = player_x_default[1];
    player_y[1] = player_y_default[1];
    player_tile[1] = player_tile_default[1];
    bullet_dir_x[0] = 0;
    bullet_x[0] = 0;
    player_score[0]++;
    player_score_tile[0]+=8;
}

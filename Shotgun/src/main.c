#include "gametank.h"
#include "drawing_funcs.h"
#include "input.h"
#include "persist.h"
#include "music.h"
#include "banking.h"
#include "gen/assets/shotgun.h"
#include "macros.h"

#pragma data-name (push, "SAVE")
#pragma data-name (pop)

#pragma code-name (push, "DATA")
// So that the tilemap starts at an 0x..10 address (easier for debugging)
unsigned char padding[8] = { 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55 };
/*
255: obstacle
0: floor
8-11: gun
Players are represented by the top 4 bits, as a tile could have all 4 players plus a gun:
0x10: player 1
0x20: player 2
0x40: player 3
0x80: player 4
*/
unsigned char tilemap[256] = {
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255, 16,  0,255,  0,  0,  0,  0,  0,  0,  0,  0,255,  0, 64,255,
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
  255,128,  0,255,  0,  0,  0,  0,  0,  0,  0,  0,255,  0, 32,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

unsigned char tilemap_dir[256] = {
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0, 10,  6,  0, 10, 14, 12, 14, 14, 12, 14,  6,  0, 10,  6,  0,
  0, 11,  7,  0, 11,  5,  0, 11,  7,  0,  9,  7,  0, 11,  7,  0,
  0, 11, 15, 14,  7,  0,  0, 11,  7,  0,  0, 11, 14, 15,  7,  0,
  0, 11, 13, 13, 13, 12, 14, 15, 15, 14, 12, 13, 13, 13,  7,  0,
  0,  3,  0,  0,  0,  0, 11, 15, 15,  7,  0,  0,  0,  0,  3,  0,
  0, 11, 14, 14,  6,  0, 11, 15, 15,  7,  0, 10, 14, 14,  7,  0,
  0, 11, 15, 15, 15, 12, 15, 15, 15, 15, 12, 15, 15, 15,  7,  0,
  0, 11, 13, 13,  5,  0, 11, 15, 15,  7,  0,  9, 13, 13,  7,  0,
  0,  3,  0,  0,  0,  0, 11, 15, 15,  7,  0,  0,  0,  0,  3,  0,
  0, 11, 14, 14, 14, 12, 13, 15, 15, 13, 12, 14, 14, 14,  7,  0,
  0, 11, 15, 13,  7,  0,  0, 11,  7,  0,  0, 11, 13, 15,  7,  0,
  0, 11,  7,  0, 11,  6,  0, 11,  7,  0, 10,  7,  0, 11,  7,  0,
  0,  9,  5,  0,  9, 13, 12, 13, 13, 12, 13,  5,  0,  9,  5,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};
#pragma code-name (pop)

unsigned char player_ptr_default[4] = {17, 222, 30, 209};
unsigned char player_x_default[4] = { 8, 112, 112, 8 };
unsigned char player_y_default[4] = { 8, 104, 8, 104};
unsigned char player_tile_default[4] = { 8, 96, 64, 40 };
unsigned char player_score[4] = { 0, 0, 0, 0 };
unsigned char player_score_tile[4] = { 0, 0, 0, 0 };

// Offset of the players on the timemap
unsigned char player_ptr[4] = {17, 222, 30, 209};
// Screen position of the players
unsigned char player_x[4] = { 8, 112, 112, 8 };
unsigned char player_y[4] = { 8, 104, 8, 104};
// Direction the players are going (1=negative, 2=positive) when in movement
unsigned char dir_x[4] = { 0, 0, 0, 0 };
unsigned char dir_y[4] = { 0, 0, 0, 0 };
// AI: direction compressed (1=up, 2=down, 4=left, 8=right)
unsigned char dir_bit[4] = { 0, 0, 4, 0 };
unsigned char dir_bit_not[4] = { 0, 0, 8, 0 };
// AI: where the CPU players want to go
unsigned char target_x[2] = { 10, 0 };
unsigned char target_y[2] = { 12, 0 };
unsigned char moving[2] = { 0, 0 };
unsigned char steps[2] = { 0, 0 };
// Direction the players are facing (used when shooting)
unsigned char dir[4] = { 0, 0, 0, 0 };
// Tile to use to display the players
unsigned char player_tile[4] = { 8, 96, 64, 40 };
unsigned char player_invincible[4] = {0, 0, 0, 0};

// Number of bullets each player has left
unsigned char bullets[4] = { 0, 0, 0, 0 };
// Offset of the bullets on the tilemap
unsigned char bullet_ptr[4] = {0, 0, 0, 0};
// Scree position of the bullets
unsigned char bullet_x[4] = {0, 0, 0, 0};
unsigned char bullet_y[4] = {0, 0, 0, 0};
// Direction the bullets are going
unsigned char bullet_dir_x[4] = { 0, 0, 0, 0 };
unsigned char bullet_dir_y[4] = { 0, 0, 0, 0 };

// Whether there is a gun at the 4 locations
unsigned char gun[4] = { 0, 0, 0, 0 };  // 0 means there is a gun

// Temp variables
unsigned char *tile;
unsigned char tmp, tile_val;
int long_val1, long_val2;
unsigned char player_tilemap;
unsigned char player_idx;

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

    load_spritesheet((char*)&ASSET__shotgun__tiles_bmp, 0);
    load_spritesheet((char*)&ASSET__shotgun__tilemap_bmp, 1);

    change_rom_bank(SAVE_BANK_NUM);

    __asm__("_loop: nop");
    while (1) {                                     //  Run forever
        clear_screen(0);
        clear_border(0);

        // Draw the playfield
        draw_sprite(0, 0, 127, 127, 0, 0, 1);

        // Draw the players
        DRAW_PLAYER(0);
        DRAW_PLAYER(1);
        DRAW_PLAYER(2);
        DRAW_PLAYER(3);

        // Draw the scores
        draw_sprite(16, 120, 8, 7, player_score_tile[0], 24, 0);
        draw_sprite(48, 120, 8, 7, player_score_tile[1], 24, 0);
        draw_sprite(80, 120, 8, 7, player_score_tile[2], 24, 0);
        draw_sprite(112, 120, 8, 7, player_score_tile[3], 24, 0);

        // Draw the guns
        if (!gun[0]) { draw_sprite(40, 16, 8, 8, 56, 0, 0); } else {
            gun[0]--;
            if (gun[0] == 0) {
                tmp = tilemap[37] & 0xF0;
                tilemap[37] = 8 | tmp;
            }
        }
        if (!gun[1]) { draw_sprite(80, 16, 8, 8, 56, 0, 0); } else {
            gun[1]--;
            if (gun[1] == 0) {
                tmp = tilemap[42] & 0xF0;;
                tilemap[42] = 9 | tmp;
            }
        }
        if (!gun[2]) { draw_sprite(40, 96, 8, 8, 56, 0, 0); } else {
            gun[2]--;
            if (gun[2] == 0) {
                tmp = tilemap[197] & 0xF0;;
                tilemap[197] = 10 | tmp;
            }
        }
        if (!gun[3]) { draw_sprite(80, 96, 8, 8, 56, 0, 0); } else {
            gun[3]--;
            if (gun[3] == 0) {
                tmp = tilemap[202] & 0xF0;;
                tilemap[202] = 11 | tmp;
            }
        }

        // Draw the guns held by the players
        if (bullets[0]) {
            draw_sprite(player_x[0]+4, player_y[0]+1, 8, 8, 56, 0, 0);
        }
        if (bullets[1]) {
            draw_sprite(player_x[1]+4, player_y[1]+1, 8, 8, 56, 0, 0);
        }

        // Compute the players next movement
        MOVE_PLAYER(0, 0x10, 0xEF);
        MOVE_PLAYER(1, 0x20, 0xDF);
        MOVE_PLAYER(2, 0x40, 0xBF);
        MOVE_PLAYER(3, 0x80, 0x7F);

        // Animating bullets
        MOVE_BULLET(0, 0xEF);
        MOVE_BULLET(1, 0xDF);
        MOVE_BULLET(2, 0xBF);
        MOVE_BULLET(3, 0x7F);

        await_draw_queue();
        PROFILER_END(0);
        sleep(1);
        flip_pages();
        PROFILER_START(0);

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

        CHECK_PLAYER_FIRE(0, player1_buttons, player1_old_buttons);
//        CHECK_PLAYER_FIRE(1, player2_buttons, player2_old_buttons);

        // AI for player 3
        // If it's "not moving" i.e. not at an intersection
        if (moving[0] == 0) {
            breakpoint();
            tile_val = tilemap_dir[player_ptr[2]];   // We get the possible directions
            tile_val ^= dir_bit_not[0];              // We remove the reverse direction
            long_val1 = 4000;
            // Up
            if (tile_val & 0x1) {
                tmp = target_x[0] >= player_x[2] ? target_x[0] - player_x[2] : player_x[2] - target_x[0];
                long_val1 = tmp*tmp;
                tmp = target_y[0] >= player_y[2]-1 ? target_y[0] - player_y[2] + 1 : player_y[2] - 1 - target_y[0];
                long_val1 += tmp*tmp;
                dir_x[2] = 0;
                dir_y[2] = 1;
                player_ptr[2] -= 16;
            }
            // Down
            if (tile_val & 0x2) {
                tmp = target_x[0] >= player_x[2] ? target_x[0] - player_x[2] : player_x[2] - target_x[0];
                long_val2 = tmp*tmp;
                tmp = target_y[0] >= player_y[2]+1 ? target_y[0] - player_y[2] - 1 : player_y[2] + 1 - target_y[0];
                long_val2 += tmp*tmp;
                if (long_val2 < long_val1) {
                    long_val1 = long_val2;
                    dir_x[2] = 0;
                    dir_y[2] = 2;
                    player_ptr[2] += 16;
                }
            }
            // Left
            if (tile_val & 0x4) {
                tmp = target_x[0] >= player_x[2]-1 ? target_x[0] - player_x[2]+1 : player_x[2]-1 - target_x[0];
                long_val2 = tmp*tmp;
                tmp = target_y[0] >= player_y[2] ? target_y[0] - player_y[2] : player_y[2] - target_y[0];
                long_val2 += tmp*tmp;
                if (long_val2 < long_val1) {
                    long_val1 = long_val2;
                    dir_x[2] = 1;
                    dir_y[2] = 0;
                    player_ptr[2]--;
                }
            }
            // Right
            if (tile_val & 0x8) {
                tmp = target_x[0] >= player_x[2]+1 ? target_x[0] - player_x[2]-1 : player_x[2]+1 - target_x[0];
                long_val2 = tmp*tmp;
                tmp = target_y[0] >= player_y[2] ? target_y[0] - player_y[2] : player_y[2] - target_y[0];
                long_val2 += tmp*tmp;
                if (long_val2 < long_val1) {
                    long_val1 = long_val2;
                    dir_x[2] = 2;
                    dir_y[2] = 0;
                    player_ptr[2]++;
                }
            }
            moving[0] = 1;
            steps[0] = 0;
        // Player in movement, so keep going
        } else {
            if (dir_x[2] == 1) {
                player_x[2]++;
            } else if (dir_x[2] == 2) {
                player_x[2]--;
            }
            if (dir_y[2] == 1) {
                player_y[2]++;
            } else if (dir_y[2] == 2) {
                player_y[2]--;
            }
            steps[0]++;
            if (steps[0] == 8) {
                moving[0] = 0;
            }
        }
    }

  return (0);
}

#pragma data-name (push, "PROG0")
// Reset a player to its original position
// player_idx: player who shot
// tmp: player who got shot
void reset_player() {
    player_ptr[tmp] = player_ptr_default[tmp];
    player_x[tmp] = player_x_default[tmp];
    player_y[tmp] = player_y_default[tmp];
    player_tile[tmp] = player_tile_default[tmp];
    player_invincible[tmp] = 255;

    player_score[player_idx]++;
    player_score_tile[player_idx] += 8;
}

// When a bullet hits a player!
// player_idx: the player who shot
// tile_val: the value of the tile (player_idx bit removed)
void hit() {
    if (tile_val & 0x10) {
        tmp = 0;
        tilemap[bullet_ptr[0]] &= 0xEF;
        reset_player();
    }
    if (tile_val & 0x20) {
        tmp = 1;
        tilemap[bullet_ptr[0]] &= 0xDF;
        reset_player();
    }
    if (tile_val & 0x40) {
        tmp = 2;
        tilemap[bullet_ptr[0]] &= 0xBF;
        reset_player();
    }
    if (tile_val & 0x80) {
        tmp = 3;
        tilemap[bullet_ptr[0]] &= 0x7F;
        reset_player();
    }

    bullet_dir_x[player_idx] = 0;
    bullet_dir_y[player_idx] = 0;
    bullet_x[player_idx] = 0;
}
#pragma data-name (pop)

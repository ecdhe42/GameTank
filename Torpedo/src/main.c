#include "gametank.h"
#include "drawing_funcs.h"
#include "input.h"
#include "persist.h"
#include "music.h"
#include "banking.h"
#include "gen/assets/torpedo.h"
#include "gen/assets/font.h"
#include "gen/assets/sfx.h"

#pragma data-name (push, "SAVE")
char saved_pos[4] = {30, 40, 1, 1};
#pragma data-name (pop)

unsigned int score;
unsigned int fish_counter;
unsigned char nb_torpedoes, game_over;
int x1_wide, width_wide, idx;
unsigned char x1, x2, y, width, x_offset;
unsigned char crosshair_x, torpedo_drawn;

const unsigned char torpedo_speed[121] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4
};

struct Ship {
    unsigned char width;
    unsigned char height;
    unsigned char gx_x;
    unsigned char gx_y;
    unsigned char solidity;
    unsigned char solidity_default;
    int speed;
    int speed_default;
    int screen_x;
    int screen_x_default;
    unsigned char screen_y;
    unsigned char display;
    unsigned char exploding;
    unsigned char sinking;
    int score;
};

struct Torpedo {
    unsigned char screen_x;
    unsigned char screen_y;
    int screen_y_wide;
    unsigned char frame;
    unsigned char counter;
};

#define NB_SHIPS    4

struct Ship available_ships[] = {
/*  width height gx_x gx_y sol sol_def speed speed_def, scr_x scr_x_def scr_y display expl sink score */
    { 40,    16,  88,  16,  2,      2,    2,        2,     0,     -160,   40,      0,   0,   0,   500 },
    { 16,     8,  72,  24,  1,      1,   -4,       -4,     0,      508,   55,      0,   0,   0,  1000 },
    { 40,    16,  80,   0,  1,      1,    2,        2,     0,     -160,   70,      0,   0,   0,    20 },
    { 40,    16,  40,   0,  1,      1,   -5,       -5,     0,      508,   80,      0,   0,   0,     1 },
};

struct Ship *sprites_to_draw[10];

/************************************************************/
/* SHIPS                                                    */
/************************************************************/
void draw_ship(struct Ship *ship, struct Torpedo *torpedo) {
    if (!ship->display) {
        return;
    }
    x1_wide = ship->screen_x >> 2;
    if (x1_wide < 0) {
        x_offset = -x1_wide;
        x1 = 0;
        x1_wide += ship->width;
        width = x1_wide;
    } else {
        x_offset = 0;
        x1 = x1_wide;
        width = ship->width;
    }
    x2 = x1 + width;
    if (x2 >= 127) { x2 = 127; }
    width = x2-x1;
    if (width == 0) return;
    draw_sprite(x1, ship->screen_y+ship->sinking, width, ship->height-ship->sinking, ship->gx_x+x_offset, ship->gx_y, 0);
    if (ship->exploding) {
        if (ship->screen_y > 60) {
            x1 = ((ship->exploding >> 2) - 1) << 5;
            if (x1 >= 128) {
                x1 -= 128;
                x2 = 56;
            } else {
                x2 = 40;
            }
            draw_sprite(torpedo->screen_x-16, ship->screen_y+ship->height-16, 32, 16, x1, x2, 0);
        } else {
            x1 = ((ship->exploding >> 2) - 1) << 4;
            if (x1 >= 128) {
                x1 -= 128;
                x2 = 80;
            } else {
                x2 = 72;
            }
            draw_sprite(torpedo->screen_x-8, ship->screen_y+ship->height-8, 16, 8, x1, x2, 0);
        }
        ship->exploding++;
/*        if (ship->exploding == 24) {
            while (1) {}
        }*/
        if (ship->exploding == 32) {
            ship->exploding = 0;
            ship->solidity--;
            if (ship->solidity == 0) {
                ship->sinking = 1;
            } else {
                ship->speed--;
            }
        }
    }
}

void move_ship(struct Ship *ship) {
    if (!ship->display || ship->exploding) {
        return;
    }

    if (ship->sinking) {
        ship->sinking++;
        if (ship->sinking == ship->height) {
            ship->display = 0;
        }
    }

    ship->screen_x += ship->speed;
    width_wide = -ship->width;
    width_wide = width_wide << 2;
    if (ship->screen_x >= 512 || ship->screen_x <= width_wide) {
        ship->display = 0;
    }
}

void reset_ship(struct Ship *ship) {
    ship->screen_x = ship->screen_x_default;
    ship->solidity = ship->solidity_default;
    ship->speed = ship->speed_default;
    ship->display = 1;
    ship->sinking = 0;
    ship->exploding = 0;
}

/************************************************************/
/* TORPEDOES                                                */
/************************************************************/
void move_torpedo(struct Torpedo *torpedo) {
    if (torpedo->screen_y == 255) {
        return;
    }
    torpedo->screen_y_wide = torpedo->screen_y_wide - torpedo_speed[torpedo->screen_y];
    torpedo->screen_y = torpedo->screen_y_wide >> 3;
    if (!(torpedo->counter & 31)) {
        play_sound_effect((char*)&ASSET__sfx__ping_bin, 1);
    }
    torpedo->counter++;
    if (torpedo->screen_y == 54) {
        torpedo->screen_y = 255;
    }
}

void draw_torpedo(struct Torpedo *torpedo) {
    draw_sprite(torpedo->screen_x-4, torpedo->screen_y, 8, 8, torpedo->frame*8, torpedo_speed[torpedo->screen_y]*8-8, 0);
    torpedo->frame++;
    if (torpedo->frame == 4) {
        torpedo->frame = 0;
    }
}

void reset_torpedo(struct Torpedo *torpedo) {
    torpedo->screen_y = 255;
    torpedo->frame = 0;
    torpedo->counter = 0;
}

void launch_torpedo(struct Torpedo *torpedo) {
    torpedo->screen_x = crosshair_x + 8;
    torpedo->screen_y_wide = 960;
    torpedo->screen_y = 120;
}

/************************************************************/
/* Misc                                                     */
/************************************************************/
void print_score() {
    int N = score;
    unsigned char screen_x = 110;
    int r;

    while (N != 0) {
        r = N % 10;
        draw_sprite(screen_x, 8, 8, 8, r*8, 88, 2);
        screen_x -= 8;
        N = N / 10;
    }
}

void draw_fish() {
    unsigned char offset;
    if (fish_counter < 44) {
        offset = 11 - (fish_counter >> 2);
        draw_sprite(110, 108+offset, 8, 12-offset, 64, 16, 0);
    } else if (fish_counter < 88) {
        offset = (fish_counter >> 2) -11;
        draw_sprite(110, 108+offset, 8, 12-offset, 64, 16, 0);
    } else if (fish_counter == 800) {
        fish_counter = 0;
    }
    fish_counter++;
}

void breakpoint() {}

/************************************************************/
/* MAIN                                                     */
/************************************************************/
int main () {
    struct Ship *ship;
    struct Torpedo torpedo;
    unsigned char counter;

    crosshair_x = 56;
    nb_torpedoes = 16;
    counter = 0;
    fish_counter = 0;

    init_graphics();
    init_dynawave();
    init_music();

    flip_pages();
    clear_border(0);
    await_draw_queue();
    flip_pages();
    await_draw_queue();
    clear_border(0);

    load_spritesheet((char *)&ASSET__torpedo__ships_bmp, 0);
    load_spritesheet((char *)&ASSET__torpedo__background_bmp, 1);
    load_spritesheet((char *)&ASSET__font__bios8_bmp, 2);
    
    change_rom_bank(SAVE_BANK_NUM);

    for (idx=0; idx<NB_SHIPS; idx++) {
        ship = &available_ships[idx];
        reset_ship(ship);
    }

    reset_torpedo(&torpedo);

    while (1) {                                     //  Run forever
        clear_screen(0);
        clear_border(0);

        draw_sprite(0, 0, 127, 127, 0, 0, 1);       // Draw the background
        print_score();
        if (nb_torpedoes) {
            draw_sprite(0, 8, nb_torpedoes*4, 8, 0, 32, 0);
        }

        torpedo_drawn = 0;
        for (idx=0; idx<NB_SHIPS; idx++) {
            ship = &available_ships[idx];
            if (!torpedo_drawn && torpedo.screen_y < ship->screen_y + ship->height) {
                draw_torpedo(&torpedo);
                torpedo_drawn = 1;
            }
            if (torpedo.screen_y == ship->screen_y + ship->height) {
                x1 = ship->screen_x >> 2;
                if (torpedo.screen_x > x1+4 && torpedo.screen_x < x1+ship->width-4) {
                    play_sound_effect((char*)&ASSET__sfx__success_bin, 1);
                    ship->exploding = 1;
                    score += ship->score;
                    reset_torpedo(&torpedo);
                }
            }
            draw_ship(ship, &torpedo);
        }
        if (!torpedo_drawn && torpedo.screen_y != 255) {
            draw_torpedo(&torpedo);
        }
        draw_fish();
        draw_sprite(crosshair_x, 111, 16, 16, 40, 16, 0);       // Draw the crosshair

        await_draw_queue();
        sleep(1);
        flip_pages();
        update_inputs();
        if(player1_buttons & INPUT_MASK_LEFT) {
            crosshair_x--;
        } else if (player1_buttons & INPUT_MASK_RIGHT) {
            crosshair_x++;
        }
    
        if (nb_torpedoes && (player1_buttons & ~player1_old_buttons & INPUT_MASK_A) && torpedo.screen_y == 255) {
            launch_torpedo(&torpedo);
            nb_torpedoes--;
        }

        move_torpedo(&torpedo);

        for (idx=0; idx<NB_SHIPS; idx++) {
            ship = &available_ships[idx];
            move_ship(ship);
            if (ship->display == 0) {
                reset_ship(ship);
            }
        }

        if (!nb_torpedoes && torpedo.screen_y == 255) {
            game_over = 1;
            for (idx=0; idx<NB_SHIPS; idx++) {
                ship = &available_ships[idx];
                if (ship->sinking || ship->exploding) {
                    game_over = 0;
                }
            }
            if (game_over) {
                while (1) {}
            }
        }

        if (torpedo.screen_y == 255 && (counter == 64)) {
            play_sound_effect(&ASSET__sfx__sonar_bin, 1);
            counter = 0;
        }
        counter++;

        tick_music();
    }

    return (0);                                     //  We should never get here!
}

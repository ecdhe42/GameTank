#define DRAW_PLAYER(nb)  \
        tmp = player_invincible[nb];     \
        if (!tmp) {     \
            draw_sprite(player_x[nb], player_y[nb]-1, 8, 9, player_tile[nb], 8, 0);    \
        } else {    \
            if (tmp & 0x4) {    \
                draw_sprite(player_x[nb], player_y[nb]-1, 8, 9, player_tile[nb], 8, 0);    \
            }   \
            player_invincible[nb]--; \
        }

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
                        tile_val = tilemap[player_ptr[nb]];  \
                        tmp = tile_val & 0xF;       \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tile_val &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        if (player_invincible[nb]) {    \
                            tilemap[player_ptr[nb]] = tile_val; \
                        } else {    \
                            tilemap[player_ptr[nb]] = tile_val | flag;   \
                        }   \
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
                        tile_val = tilemap[player_ptr[nb]];  \
                        tmp = tile_val & 0xF;   \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tile_val &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        if (player_invincible[nb]) {    \
                            tilemap[player_ptr[nb]] = tile_val; \
                        } else {    \
                            tilemap[player_ptr[nb]] = tile_val | flag;   \
                        }   \
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
                        tile_val = tilemap[player_ptr[nb]];  \
                        tmp = tile_val & 0xF;   \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tile_val &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        if (player_invincible[nb]) {    \
                            tilemap[player_ptr[nb]] = tile_val; \
                        } else {    \
                            tilemap[player_ptr[nb]] = tile_val | flag;   \
                        }   \
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
                        tile_val = tilemap[player_ptr[nb]];  \
                        tmp = tile_val & 0xF;   \
                        if (tmp >= 8 && bullets[nb] == 0) {  \
                            gun[tmp-8] = 0xFF;  \
                            tmp = 0;    \
                            tile_val &= 0xF0;   \
                            bullets[nb] = 3; \
                        }   \
                        if (player_invincible[nb]) {    \
                            tilemap[player_ptr[nb]] = tile_val; \
                        } else {    \
                            tilemap[player_ptr[nb]] = tile_val | flag;   \
                        }   \
                    } else if (tmp == nb) {  \
                        dir_y[nb] = 0;   \
                    }   \
                }   \
            }   \
        }

#define MOVE_BULLET(nb, mask) \
        if (bullet_x[nb]) {  \
            breakpoint();   \
            if (bullet_dir_x[nb] != 0) { \
                if (bullet_dir_x[nb] == 1) { \
                    bullet_x[nb]--;  \
                    bullet_x[nb]--;  \
                    tmp = bullet_x[nb] & 7;  \
                    if (tmp == 6) { \
                        bullet_ptr[nb]--;    \
                    }  \
                } else {    \
                    bullet_x[nb]++;  \
                    bullet_x[nb]++;  \
                    tmp = bullet_x[nb] & 7;  \
                    if (tmp == 0) { \
                        bullet_ptr[nb]++;    \
                    }   \
                }   \
            } else {    \
                if (bullet_dir_y[nb] == 1) { \
                    bullet_y[nb]--; \
                    bullet_y[nb]--;  \
                    tmp = bullet_y[nb] & 7;  \
                    if (tmp == 6) { \
                        bullet_ptr[nb]-= 16; \
                    }   \
                } else {    \
                    bullet_y[nb]++;  \
                    bullet_y[nb]++;  \
                    tmp = bullet_y[nb] & 7;  \
                    if (tmp == 0) { \
                        bullet_ptr[nb]+=16;  \
                    }   \
                }   \
            }   \
            tile_val = tilemap[bullet_ptr[nb]];  \
            tmp = tile_val & mask;  \
            if (tile_val == 0xFF) { \
                bullet_dir_x[nb] = 0;    \
                bullet_x[nb] = 0;    \
            } else if (tmp) {   \
                player_idx = nb; \
                hit();  \
            } else {    \
                draw_box(bullet_x[nb], bullet_y[nb], 1, 1, 7);    \
            }   \
        }

#define CHECK_PLAYER_FIRE(nb, player_buttons, player_old_buttons)  \
        if (bullets[nb] != 0 && bullet_x[nb] == 0 && (player_buttons & ~player_old_buttons & INPUT_MASK_A)) {   \
            bullets[nb]--;   \
            bullet_ptr[nb] = player_ptr[nb];  \
            switch(dir[nb]) {    \
                case 0: \
                    bullet_x[nb] = (player_x[nb]+6) & 0xFE;   \
                    bullet_y[nb] = player_y[nb]+4;    \
                    bullet_dir_x[nb] = 2;    \
                    bullet_dir_y[nb] = 0;    \
                    break;  \
                case 1: \
                    bullet_x[nb] = player_x[nb] & 0xFE;   \
                    bullet_y[nb] = player_y[nb]+4;    \
                    bullet_dir_x[nb] = 1;    \
                    bullet_dir_y[nb] = 0;    \
                    break;  \
                case 2: \
                    bullet_x[nb] = player_x[nb]+4;    \
                    bullet_y[nb] = player_y[nb] & 0xFE;   \
                    bullet_dir_x[nb] = 0;    \
                    bullet_dir_y[nb] = 1;    \
                    break;  \
                case 3: \
                    bullet_x[nb] = player_x[nb]+4;    \
                    bullet_y[nb] = (player_y[nb]+6) & 0xFE;   \
                    bullet_dir_x[nb] = 0;    \
                    bullet_dir_y[nb] = 2;    \
                    break;  \
            }   \
        }

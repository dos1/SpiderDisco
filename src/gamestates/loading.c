/*! \file loading.c
 *  \brief Loading screen.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "../common.h"
#include <libsuperderpy.h>

/*! \brief Resources used by Loading state. */
struct LoadingResources {
		ALLEGRO_BITMAP *loading_bitmap; /*!< Rendered loading bitmap. */
		struct Character *pajonczek;
};

void Progress(struct Game *game, struct LoadingResources *data, float p) {
	al_set_target_bitmap(al_get_backbuffer(game->display));
	al_clear_to_color(al_map_rgb(255,255,255));
	al_draw_bitmap(data->loading_bitmap,game->viewport.width * 0.075, game->viewport.height*0.92,0);
	AnimateCharacter(game, data->pajonczek, 1.5);
	DrawCharacter(game, data->pajonczek, al_map_rgb(255,255,255), 0);
	al_draw_filled_rectangle(0, game->viewport.height * 0.99, game->viewport.width,
	                         game->viewport.height, al_map_rgba(0,0,0,32));
	al_draw_filled_rectangle(0, game->viewport.height * 0.99, p*game->viewport.width,
	                         game->viewport.height, al_map_rgba(0,0,0,255));
}

void Draw(struct Game *game, struct LoadingResources *data, float p) {
	al_draw_bitmap(data->loading_bitmap,0,0,0);
	DrawCharacter(game, data->pajonczek, al_map_rgb(255,255,255), 0);
	Progress(game, data, p);
}

void* Load(struct Game *game) {
	struct LoadingResources *data = malloc(sizeof(struct LoadingResources));
	int flags = al_get_new_bitmap_flags();
	al_add_new_bitmap_flag(ALLEGRO_MAG_LINEAR | ALLEGRO_MIN_LINEAR);
	al_clear_to_color(al_map_rgb(255,255,255));

	data->loading_bitmap = al_load_bitmap(GetDataFilePath(game, "loading.png"));

	al_draw_filled_rectangle(0, game->viewport.height * 0.99, game->viewport.width,
	                         game->viewport.height, al_map_rgba(0,0,0,32));
	al_set_target_bitmap(al_get_backbuffer(game->display));

	data->pajonczek = CreateCharacter(game, "pajonczek");
	RegisterSpritesheet(game, data->pajonczek, "stand");
	LoadSpritesheets(game, data->pajonczek);
	SelectSpritesheet(game, data->pajonczek, "stand");
	SetCharacterPositionF(game, data->pajonczek, 0.02, 0.9, 0);
	al_set_new_bitmap_flags(flags);

	return data;
}
void Unload(struct Game *game, struct LoadingResources *data) {
	al_destroy_bitmap(data->loading_bitmap);
	DestroyCharacter(game, data->pajonczek);
	free(data);
}

void Start(struct Game *game, struct LoadingResources *data) {}
void Stop(struct Game *game, struct LoadingResources *data) {}

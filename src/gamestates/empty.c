/*! \file empty.c
 *  \brief Empty gamestate.
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
#include <math.h>
#include <libsuperderpy.h>

#define NUMBER_OF_PAJONKS 50
#define FAST_MODE false

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_FONT *font;
		int blink_counter;

		float wind;

		struct Character *pajonczek;
		struct Character *pajonczki[NUMBER_OF_PAJONKS];

		ALLEGRO_AUDIO_STREAM *music, *elevator;

		ALLEGRO_BITMAP *bg, *web;
		ALLEGRO_BITMAP *disco[6], *matryca;
		ALLEGRO_BITMAP *pola[20][6], *duzepole;
		ALLEGRO_BITMAP *listek03, *roslinka04, *wp05, *listek1, *listek2, *listek3, *cien;

		ALLEGRO_BITMAP *tmp, *mask;

		float discocount;

		float pole;
};

struct PajonkData {
		float angle;
		float angle_mod;
		float angle_range;
		float speed;
		int r;
		bool right;
		float sin;
};

int Gamestate_ProgressCount = 892; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->blink_counter++;

	data->wind += 0.0125;
	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		struct PajonkData *d = data->pajonczki[i]->data;
		d->angle_mod += d->speed * (d->right ? 1 : -1);
		if (fabs(d->angle_mod) >= d->angle_range) {
			d->right = !d->right;
		}
		d->sin += d->speed * 10.0;
		AnimateCharacter(game, data->pajonczki[i], 1);
	}
	data->discocount += 0.0318;
	if (data->discocount >= 6) {
		data->discocount -= 5;
	}
	data->pole += 0.05;
	if (data->pole >= 20*6) {
		data->pole = 0;
	}
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_draw_bitmap(data->bg, -240 + sin(data->wind) * 4, -160,0);
	al_draw_bitmap(data->disco[(int)data->discocount], 480, 158 + sin(data->wind) * 4, 0);

	int next = (int)data->discocount;
	next--;
	next += 6;
	next++;
	next++;
	next = next%6;
	next++;

	int prev = (int)(data->blink_counter/15.0) % 6 + 1;

	al_set_target_bitmap(data->tmp);
	al_clear_to_color(al_map_rgba(0,0,0,0));

	al_draw_bitmap(data->disco[prev], 480, 158 + sin(data->wind) * 4, 0);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ZERO, ALLEGRO_ALPHA); // now as a mask
	al_draw_bitmap(data->duzepole, 480, 158 + sin(data->wind) * 4, 0);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

	al_set_target_backbuffer(game->display);
	al_draw_bitmap(data->tmp, 0, 0, 0);


	al_set_target_bitmap(data->tmp);
	al_clear_to_color(al_map_rgba(0,0,0,0));

	al_draw_bitmap(data->disco[next], 480, 158 + sin(data->wind) * 4, 0);

	//int p = (int)data->pole;
	//al_draw_bitmap(data->pola[p/6][p%6], 480, 158 + sin(data->wind) * 4, 0);
	al_set_target_bitmap(data->mask);
	al_clear_to_color(al_map_rgba(0,0,0,0));

	int p = (int)(data->blink_counter/4.0) % 20;
	for (int i=0; i<6; i++) {
		al_draw_bitmap(data->pola[p][i], 480, 158 + sin(data->wind) * 4, 0);
	}
	al_set_target_bitmap(data->tmp);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ZERO, ALLEGRO_ALPHA); // now as a mask
	al_draw_bitmap(data->mask, 0, 0, 0);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

	al_set_target_backbuffer(game->display);
	al_draw_bitmap(data->tmp, 0, 0, 0);


	al_draw_bitmap(data->web, -38, -160 + sin(data->wind) * 4, 0);

	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		struct PajonkData *d = data->pajonczki[i]->data;
		int centerx = 900, centery = 525;
		//d->angle = data->wind; //(rand() / (float)RAND_MAX) * 2*ALLEGRO_PI;

		int r = d->r + sin(d->sin) * 20;
		int a = sin(d->angle + d->angle_mod) * r;
		int b = cos(d->angle + d->angle_mod) * r;
		SetCharacterPosition(game, data->pajonczki[i], centerx + b, centery + a, 0);

		//SetCharacterPositionF(game, data->pajonczki[i], (rand() / (float)RAND_MAX) / 2.5 + 0.3 - 0.05, (rand() / (float)RAND_MAX) / 1.5 + 0.165, 0);
		DrawCharacter(game, data->pajonczki[i], al_map_rgb(255,255,255), 0);
	}

	al_draw_bitmap(data->listek03, 566, 598,0);
	al_draw_rotated_bitmap(data->roslinka04, 512, 1390, 1221 + 512, -100 + 1390, sin(data->wind / 2.0 + 2.34) / 50.0, 0);
	al_draw_bitmap(data->wp05, -240, -160,0);

	al_draw_bitmap(data->listek1, 1065, 644,0);
	//al_draw_rotated_bitmap(data->listek1, 200, 200, 1065, 644, data->wind, 0);

	al_draw_rotated_bitmap(data->listek2, 0, 588, -94, 534 + 588, sin(data->wind / 1.5 + 5.298) / 20.0, 0);

	//al_draw_bitmap(data->listek2, -94, 534,0);
	al_draw_bitmap(data->listek3, -94, -123,0);
	al_draw_tinted_bitmap(data->cien, al_map_rgba_f(0.1,0.1,0.1,0.4), 1282, -363,0);
}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->font = al_create_builtin_font();

	data->elevator = al_load_audio_stream(GetDataFilePath(game, "elevator.flac"), 4, 1024);
	al_set_audio_stream_playing(data->elevator, true);
	al_attach_audio_stream_to_mixer(data->elevator, game->audio.music);
	al_set_audio_stream_playmode(data->elevator, ALLEGRO_PLAYMODE_LOOP);


	data->pajonczek = CreateCharacter(game, "pajonczek");
	RegisterSpritesheet(game, data->pajonczek, "stand");
	LoadSpritesheets(game, data->pajonczek);
	progress(game);

	data->bg = al_load_bitmap(GetDataFilePath(game, "bg.png"));
	progress(game);
	data->web = al_load_bitmap(GetDataFilePath(game, "web.png"));
	progress(game);
	data->listek03 = al_load_bitmap(GetDataFilePath(game, "03listek.png"));
	progress(game);
	data->roslinka04 = al_load_bitmap(GetDataFilePath(game, "04_roslinka.png"));
	progress(game);
	data->wp05 = al_load_bitmap(GetDataFilePath(game, "05_warstwa_posrednia.png"));
	progress(game);
	data->listek1 = al_load_bitmap(GetDataFilePath(game, "06_lisc_zielony.png"));
	progress(game);
	data->listek2 = al_load_bitmap(GetDataFilePath(game, "06_lisc_zielony2.png"));
	progress(game);
	data->listek3 = al_load_bitmap(GetDataFilePath(game, "06_zolty_lisc.png"));
	progress(game);
	data->cien = al_load_bitmap(GetDataFilePath(game, "07_cien.png"));
	progress(game);

	data->tmp = al_create_bitmap(1920, 1080);
	data->mask = al_create_bitmap(1920, 1080);

	data->matryca = al_load_bitmap(GetDataFilePath(game, "matryca.png"));
	progress(game);

	al_lock_bitmap(data->matryca, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	for (int i=0; i<20; i++) {
		for (int j=0; j<6; j++) {
			data->pola[i][j] = al_create_bitmap(872, 792);
			al_lock_bitmap(data->pola[i][j], ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		}
	}
	data->duzepole = al_create_bitmap(872, 792);
	al_lock_bitmap(data->duzepole, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
	ALLEGRO_COLOR transparent = al_map_rgba(0, 0, 0, 0);
	ALLEGRO_COLOR black = al_map_rgba(0, 0, 0, 255);
	for (int x=0; x<872; x++) {
		for (int y=0; y<792; y++) {
			ALLEGRO_COLOR color = al_get_pixel(data->matryca, x, y);
			for (int i=0; i< (FAST_MODE ? 1 : 20); i++) {
				for (int j=0; j<6; j++) {
					al_set_target_bitmap(data->pola[i][j]);
					al_put_pixel(x, y, transparent);
					//PrintConsole(game, "r: %d, g: %d", color.r, color.g);
					if (color.r*255 == 60 + i*10) {
						if (color.g*255 == 100 + j*30) {
							al_put_pixel(x, y, black);
						}
					}
				}
			}
			al_set_target_bitmap(data->duzepole);
			if (color.r*255 == 50) {
				if (color.g*255 == 70) {
					al_put_pixel(x, y, black);
				}
			}
		}
		progress(game);
	}
	for (int i=0; i<20; i++) {
		for (int j=0; j<6; j++) {
			al_unlock_bitmap(data->pola[i][j]);
		}
	}
	al_unlock_bitmap(data->duzepole);
	al_unlock_bitmap(data->matryca);
	progress(game);

	data->disco[0] = al_load_bitmap(GetDataFilePath(game, "01disko00.png"));
	progress(game);
	data->disco[1] = al_load_bitmap(GetDataFilePath(game, "01disko01.png"));
	progress(game);
	data->disco[2] = al_load_bitmap(GetDataFilePath(game, "01disko02.png"));
	progress(game);
	data->disco[3] = al_load_bitmap(GetDataFilePath(game, "01disko03.png"));
	progress(game);
	data->disco[4] = al_load_bitmap(GetDataFilePath(game, "01disko04.png"));
	progress(game);
	data->disco[5] = al_load_bitmap(GetDataFilePath(game, "01disko05.png"));
	progress(game);

	data->music = al_load_audio_stream(GetDataFilePath(game, "startrek.flac"), 4, 1024);
	al_set_audio_stream_playing(data->music, false);
	al_attach_audio_stream_to_mixer(data->music, game->audio.music);
	al_set_audio_stream_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);
	progress(game);

	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		data->pajonczki[i] = CreateCharacter(game, "pajonczek");
		data->pajonczki[i]->shared = true;
		data->pajonczki[i]->spritesheets = data->pajonczek->spritesheets;
		data->pajonczki[i]->data = malloc(sizeof(struct PajonkData));
	}

	al_set_audio_stream_playing(data->elevator, false);
	al_destroy_audio_stream(data->elevator);

	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);

	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		DestroyCharacter(game, data->pajonczki[i]);
	}
	DestroyCharacter(game, data->pajonczek);
	al_destroy_audio_stream(data->music);

	al_destroy_bitmap(data->bg);
	al_destroy_bitmap(data->web);
	al_destroy_bitmap(data->listek03);
	al_destroy_bitmap(data->roslinka04);
	al_destroy_bitmap(data->wp05);
	al_destroy_bitmap(data->listek1);
	al_destroy_bitmap(data->listek2);
	al_destroy_bitmap(data->listek3);
	al_destroy_bitmap(data->cien);

	al_destroy_bitmap(data->disco[0]);
	al_destroy_bitmap(data->disco[1]);
	al_destroy_bitmap(data->disco[2]);
	al_destroy_bitmap(data->disco[3]);
	al_destroy_bitmap(data->disco[4]);
	al_destroy_bitmap(data->disco[5]);

	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->blink_counter = 0;
	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		SelectSpritesheet(game, data->pajonczki[i], "stand");
		//SetCharacterPositionF(game, data->pajonczki[i], (rand() / (float)RAND_MAX) / 2.5 + 0.3 - 0.05, (rand() / (float)RAND_MAX) / 1.5 + 0.165, 0);
		data->pajonczki[i]->pos = rand() % 3;
		struct PajonkData *d = data->pajonczki[i]->data;
		d->angle = (rand() / (float)RAND_MAX) * 2*ALLEGRO_PI;
		d->angle_mod = 0;
		d->right = rand() % 2;
		d->r = rand() % 225 + 125;
		d->angle_range = (rand() / (float)RAND_MAX) * 0.33 + 0.1;
		d->speed = (rand() / (float)RAND_MAX) * 0.005 + 0.0066;
		d->sin = (rand() / (float)RAND_MAX);
		data->pajonczki[i]->pos_tmp= rand() % 5;
	}

	data->wind = 0;
	al_set_audio_stream_playing(data->music, true);
	data->discocount = 0.5;
	data->pole = 0;
}

void Gamestate_Stop(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
	al_set_audio_stream_playing(data->music, false);
}

void Gamestate_Pause(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic not ProcessEvent)
	// Pause your timers here.
}

void Gamestate_Resume(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers here.
}

// Ignore this for now.
// TODO: Check, comment, refine and/or remove:
void Gamestate_Reload(struct Game *game, struct GamestateResources* data) {}

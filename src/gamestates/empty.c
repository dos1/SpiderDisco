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

#define NUMBER_OF_PAJONKS 80

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_FONT *font;
		int blink_counter;

		float wind;

		struct Character *pajonczek;
		struct Character *pajonczki[NUMBER_OF_PAJONKS];

		ALLEGRO_AUDIO_STREAM *music;

		ALLEGRO_BITMAP *bg, *web;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->blink_counter++;
	if (data->blink_counter >= 60) {
		data->blink_counter = 0;
	}
	data->wind += 0.0125;
	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		AnimateCharacter(game, data->pajonczki[i], 1);
	}
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_draw_bitmap(data->bg, 0,0,0);
	al_draw_bitmap(data->web, -44, -160 + sin(data->wind) * 4, 0);
	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		DrawCharacter(game, data->pajonczki[i], al_map_rgb(255,255,255), 0);
	}
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

	data->pajonczek = CreateCharacter(game, "pajonczek");
	RegisterSpritesheet(game, data->pajonczek, "stand");
	LoadSpritesheets(game, data->pajonczek);

	data->bg = al_load_bitmap(GetDataFilePath(game, "bg.png"));
	data->web = al_load_bitmap(GetDataFilePath(game, "web.png"));

	data->music = al_load_audio_stream(GetDataFilePath(game, "startrek.flac"), 4, 1024);
	al_set_audio_stream_playing(data->music, false);
	al_attach_audio_stream_to_mixer(data->music, game->audio.music);
	al_set_audio_stream_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);

	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		data->pajonczki[i] = CreateCharacter(game, "pajonczek");
		data->pajonczki[i]->shared = true;
		data->pajonczki[i]->spritesheets = data->pajonczek->spritesheets;
	}

	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->blink_counter = 0;
	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		SelectSpritesheet(game, data->pajonczki[i], "stand");
		SetCharacterPositionF(game, data->pajonczki[i], (rand() / (float)RAND_MAX) / 2.0 + 0.25, (rand() / (float)RAND_MAX) / 1.5 + 0.165, 0);
		data->pajonczki[i]->pos = rand() % 3;
	}

	data->wind = 0;
	al_set_audio_stream_playing(data->music, true);
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

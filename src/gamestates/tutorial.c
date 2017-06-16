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

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_BITMAP *bmp, *fg, *left, *right, *anykey;
		ALLEGRO_AUDIO_STREAM *elevator;

		int counter;
};

int Gamestate_ProgressCount = 5; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->counter++;
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_draw_bitmap(data->bmp, 0, 0,0);
	al_draw_bitmap(data->fg, -240 + sin(data->counter / 1.5) * 2, -160 + cos(data->counter / 4.0) * 1.5,0);

	al_draw_rotated_bitmap(data->left, 1160-1081, 525-161, 1160, 525, sin(data->counter / 12.0) / 32.0, 0);
	al_draw_rotated_bitmap(data->right, 1160-1343, 525-266, 1160, 525, -sin(data->counter / 12.0) / 32.0, 0);

	if (data->counter % 80 < 65) {
		al_draw_bitmap(data->anykey, 1230, 970, 0);
	}
}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode != ALLEGRO_KEY_TILDE)) {
		SwitchCurrentGamestate(game, "disco");
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->bmp = al_load_bitmap(GetDataFilePath(game, "tutorial.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	data->fg = al_load_bitmap(GetDataFilePath(game, "tutorialfg.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	data->left = al_load_bitmap(GetDataFilePath(game, "tutorialleft.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	data->right = al_load_bitmap(GetDataFilePath(game, "tutorialright.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	data->anykey = al_load_bitmap(GetDataFilePath(game, "anykey.png"));
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->elevator = al_load_audio_stream(GetDataFilePath(game, "elevator.flac"), 4, 1024);
	al_set_audio_stream_playing(data->elevator, false);
	al_attach_audio_stream_to_mixer(data->elevator, game->audio.music);
	al_set_audio_stream_gain(data->elevator, 0.8);
	al_set_audio_stream_playmode(data->elevator, ALLEGRO_PLAYMODE_LOOP);

	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_bitmap(data->bmp);
	al_destroy_bitmap(data->fg);
	al_destroy_bitmap(data->left);
	al_destroy_bitmap(data->right);
	al_destroy_bitmap(data->anykey);
	al_destroy_audio_stream(data->elevator);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	al_set_audio_stream_playing(data->elevator, true);
	data->counter = 0;
}

void Gamestate_Stop(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
	al_set_audio_stream_playing(data->elevator, false);
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

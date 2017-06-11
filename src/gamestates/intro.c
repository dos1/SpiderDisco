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
#include <libsuperderpy.h>

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		struct Timeline *timeline;

		ALLEGRO_BITMAP *bitmap;

		ALLEGRO_SAMPLE *music_sample;
		ALLEGRO_SAMPLE_INSTANCE *music;

		ALLEGRO_SAMPLE *sample;
		ALLEGRO_SAMPLE_INSTANCE *sample_instance;

};

int Gamestate_ProgressCount = 3; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	TM_Process(data->timeline);
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_clear_to_color(al_map_rgb(255,255,255));
	if (data->bitmap) {
		al_draw_scaled_bitmap(data->bitmap, 0, 0, al_get_bitmap_width(data->bitmap),
		                      al_get_bitmap_height(data->bitmap), 0, 0, 1920, 1080, 0);
	}

TM_DrawDebug(game, data->timeline, 0);
}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	TM_HandleEvent(data->timeline, ev);
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		SwitchCurrentGamestate(game, "empty");
		// When there are no active gamestates, the engine will quit.
	}
}



bool Finish(struct Game *game, struct TM_Action *action, enum TM_ActionState state) {
	struct GamestateResources *data = TM_GetArg(action->arguments, 0);
	if (state == TM_ACTIONSTATE_RUNNING) {
		al_stop_sample_instance(data->music);

		int x, y;
		al_get_mouse_cursor_position(&x, &y);

		SwitchCurrentGamestate(game, "empty");

	}
	return true;
}

bool Show(struct Game *game, struct TM_Action *action, enum TM_ActionState state) {
	struct GamestateResources *data = TM_GetArg(action->arguments, 0);
	if (state == TM_ACTIONSTATE_RUNNING) {
		data->bitmap = TM_GetArg(action->arguments, 1);

	}
	return true;
}

bool Speak(struct Game *game, struct TM_Action *action, enum TM_ActionState state) {
//	struct GamestateResources *data = TM_GetArg(action->arguments, 0);
	ALLEGRO_AUDIO_STREAM *stream = TM_GetArg(action->arguments, 1);

	if (state == TM_ACTIONSTATE_INIT) {
		al_set_audio_stream_playing(stream, false);
		al_set_audio_stream_playmode(stream, ALLEGRO_PLAYMODE_ONCE);
	}

	if (state == TM_ACTIONSTATE_START) {
		//al_rewind_audio_stream(stream);
		al_attach_audio_stream_to_mixer(stream, game->audio.voice);
		al_set_audio_stream_playing(stream, true);

	}

	if (state == TM_ACTIONSTATE_RUNNING) {
		return !al_get_audio_stream_playing(stream);
	}

	if (state == TM_ACTIONSTATE_DESTROY) {
		al_destroy_audio_stream(stream);
	}
	return false;
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->timeline = TM_Init(game, "intro");
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->music_sample = al_load_sample(GetDataFilePath(game, "intro/music.flac"));
	data->music = al_create_sample_instance(data->music_sample);
	al_attach_sample_instance_to_mixer(data->music, game->audio.music);
	al_set_sample_instance_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);

	al_play_sample_instance(data->music);

	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar


	TM_AddDelay(data->timeline, 1000);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/1.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/1.flac"), 4, 1024)), "speak");

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/2.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/2.flac"), 4, 1024)), "speak");

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/3.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/3.flac"), 4, 1024)), "speak");

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/4.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/4.flac"), 4, 1024)), "speak");

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/5.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/5.flac"), 4, 1024)), "speak");

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/6.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/6.flac"), 4, 1024)), "speak");

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/7.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/7.flac"), 4, 1024)), "speak");

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/8.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/8.flac"), 4, 1024)), "speak");

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/9.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/9.flac"), 4, 1024)), "speak");

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/10.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/10.flac"), 4, 1024)), "speak");

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 2, data, al_load_bitmap(GetDataFilePath(game, "intro/11.png"))), "show");
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, data, al_load_audio_stream(GetDataFilePath(game, "intro/11.flac"), 4, 1024)), "speak");


TM_AddAction(data->timeline, Finish, TM_AddToArgs(NULL, 1, data), "finish");

  return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_sample_instance(data->music);
	al_destroy_sample(data->music_sample);
	TM_Destroy(data->timeline);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->bitmap = NULL;
}

void Gamestate_Stop(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
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

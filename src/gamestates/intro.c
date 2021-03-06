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
	struct Timeline* timeline;

	ALLEGRO_BITMAP* bitmap;

	ALLEGRO_AUDIO_STREAM* music;

	ALLEGRO_FONT* font;

	bool skip;
	char* text;
};

int Gamestate_ProgressCount = 11; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	// Called 60 times per second. Here you should do all your game logic.
	TM_Process(data->timeline, delta);
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_clear_to_color(al_map_rgb(255, 255, 255));
	if (data->bitmap) {
		//	al_draw_scaled_bitmap(data->bitmap, 0, 0, al_get_bitmap_width(data->bitmap),
		//	                      al_get_bitmap_height(data->bitmap), 0, 0, 1920, 1080, 0);

		al_draw_bitmap(data->bitmap, -240, -160, 0);
	}

	if (data->text) {
		al_draw_text(data->font, al_map_rgb(0, 0, 0), 1920 / 2, 1000, ALLEGRO_ALIGN_CENTER, data->text);
	}

	//TM_DrawDebug(game, data->timeline, 0);
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if (((ev->type == ALLEGRO_EVENT_KEY_DOWN) && ((ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE) || (ev->keyboard.keycode == ALLEGRO_KEY_BACK))) ||
		(ev->type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN)) {
		SwitchCurrentGamestate(game, "tutorial");
		LoadGamestate(game, "disco");
		// When there are no active gamestates, the engine will quit.
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP)) {
		data->skip = true;
	}
}

static TM_ACTION(Finish) {
	if (action->state == TM_ACTIONSTATE_RUNNING) {
		SwitchCurrentGamestate(game, "tutorial");
		LoadGamestate(game, "disco");
	}
	return true;
}

static TM_ACTION(Show) {
	if (action->state == TM_ACTIONSTATE_RUNNING) {
		data->bitmap = TM_GetArg(action->arguments, 0);
	}
	return true;
}

static TM_ACTION(Speak) {
	ALLEGRO_AUDIO_STREAM* stream = TM_GetArg(action->arguments, 0);

	if (action->state == TM_ACTIONSTATE_INIT) {
		al_set_audio_stream_playing(stream, false);
		al_set_audio_stream_playmode(stream, ALLEGRO_PLAYMODE_ONCE);
	}

	if (action->state == TM_ACTIONSTATE_START) {
		//al_rewind_audio_stream(stream);
		al_attach_audio_stream_to_mixer(stream, game->audio.voice);
		al_set_audio_stream_playing(stream, true);
		//al_set_audio_stream_gain(stream, 1.1);
		data->text = TM_GetArg(action->arguments, 1);
	}

	if (action->state == TM_ACTIONSTATE_RUNNING) {
		return !al_get_audio_stream_playing(stream) || data->skip;
	}

	if (action->state == TM_ACTIONSTATE_DESTROY) {
		data->skip = false;
		data->text = NULL;
		al_destroy_audio_stream(stream);
	}
	return false;
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources* data = malloc(sizeof(struct GamestateResources));
	data->timeline = TM_Init(game, data, "intro");

	data->music = al_load_audio_stream(GetDataFilePath(game, "intro/music.flac"), 4, 1024);
	al_set_audio_stream_playing(data->music, false);
	al_set_audio_stream_gain(data->music, 1.2);

	al_attach_audio_stream_to_mixer(data->music, game->audio.music);

	data->font = al_load_ttf_font(GetDataFilePath(game, "fonts/belligerent.ttf"), 48, 0);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	TM_AddDelay(data->timeline, 0.6);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/1.png"))));
	TM_AddDelay(data->timeline, 0.4);
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/1.flac"), 4, 1024), "Once upon a time there was a little drone named Bobby."));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/1a.flac"), 4, 1024), "Bobby had a human owner, who was a reckless boy."));
	progress(game);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/2.png"))));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/2.flac"), 4, 1024), "One day he crashed Bobby into the trees and ran away."));
	progress(game);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/3.png"))));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/3.flac"), 4, 1024), "Fortunately, the drone was rescued by a huge family of overprotective spiders."));
	progress(game);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/4.png"))));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/4.flac"), 4, 1024), "They lived happily for some time."));
	progress(game);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/5.png"))));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/5.flac"), 4, 1024), "He grew up with them, learned their ways: playing typical spider sports"));
	progress(game);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/6.png"))));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/6.flac"), 4, 1024), "and traditional spider dinner parties."));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/6a.flac"), 4, 1024), "They accepted him."));
	progress(game);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/7.png"))));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/7.flac"), 4, 1024), "But he still felt quite out of place."));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/7a.flac"), 4, 1024), "Perhaps due to the fact that he constantly kept squishing his new family,"));
	progress(game);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/8.png"))));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/8.flac"), 4, 1024), "Perhaps due to the fact that he constantly kept squishing his new family,"));
	progress(game);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/9.png"))));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/9.flac"), 4, 1024), "which led him to a personality crisis."));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/9a.flac"), 4, 1024), "Spiders might be very forgiving, but he's a very emotional fella."));
	progress(game);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/10.png"))));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/10.flac"), 4, 1024), "Now Bobby wants to learn how to move like a spider."));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/10a.flac"), 4, 1024), "So he gathered his friends and went to the..."));
	progress(game);

	TM_AddAction(data->timeline, Show, TM_AddToArgs(NULL, 1, al_load_bitmap(GetDataFilePath(game, "intro/11.png"))));
	TM_AddAction(data->timeline, Speak, TM_AddToArgs(NULL, 2, al_load_audio_stream(GetDataFilePath(game, "intro/11.flac"), 4, 1024), NULL));

	TM_AddDelay(data->timeline, 1.0);

	TM_AddAction(data->timeline, Finish, NULL);

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_audio_stream(data->music);
	TM_Destroy(data->timeline);
	al_destroy_bitmap(data->bitmap);
	al_destroy_font(data->font);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->bitmap = NULL;
	data->skip = false;
	data->text = NULL;
	al_set_audio_stream_playing(data->music, true);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
	al_set_audio_stream_playing(data->music, false);
}

void Gamestate_Pause(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic not ProcessEvent)
	// Pause your timers here.
}

void Gamestate_Resume(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers here.
}

// Ignore this for now.
// TODO: Check, comment, refine and/or remove:
void Gamestate_Reload(struct Game* game, struct GamestateResources* data) {}

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
		ALLEGRO_FONT *font;
		int blink_counter;

		ALLEGRO_BITMAP *bmp;

		ALLEGRO_BITMAP *bg;
		ALLEGRO_AUDIO_STREAM *music;

		ALLEGRO_BITMAP *photo1, *photo2, *photogirl, *wstazka;

		float pos;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	if (data->blink_counter < 120) {
		data->blink_counter++;
	}

	if (data->pos > -(100 + 300*game->data->score + 250)) {
		data->pos -= 0.75;
	}
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_draw_bitmap(data->bg, -240, -160,0);
	al_draw_bitmap(data->bmp, 1920/2 - 100, data->pos, 0);

}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}
}

char *reasons_common[] = {
  "Brought you a beer last night",
  "Comforted you after your last breakup",
  "Was your best friend since high school",
  "Always helped you with cheating at exams",
  "Lent you $1000 when you really needed",
  "You had a good time hiking last summer",
  "Had awesome ideas at game jams",
  "Met you with your wife",
  "Quite good looking cousin",
  "Was always there for you",
  "You two had a nice time on a game jam"
};
char *reasons_male[] = {
  "Was the witness at your wedding",
  "This one funny guy at your workplace",
  "Your brother",
  "You can't remember his last name"
};
char *reasons_female[] = {
  "Went with you to your high school prom",
  "Had a crush on you",
  "You used to go out in the high school",
  "Your sister",
  "Your wife"
};
char *names_male[] = {
  "Rob", "John", "Zbyszek", "Frank", "Tom", "Sebastian", "David", "Mark", "Marty", "Marek",
  "Stefan", "Bob", "Paulo", "Julio", "Max", "Gandalf", "Otto"
};
char *names_female[] = {
  "Anne", "Mary", "Suzanne", "Ela", "Ola", "Marta", "Julie", "Stephanie"
};

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->font = al_load_ttf_font(GetDataFilePath(game, "fonts/belligerent.ttf"), 48, 0);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	//game->data->score = 3;

	data->bg = al_load_bitmap(GetDataFilePath(game, "cmentarz.png"));

	data->photo1 = al_load_bitmap(GetDataFilePath(game, "polaroid_chlopczyk.png"));
	data->photo2 = al_load_bitmap(GetDataFilePath(game, "polaroid_chlopczyk2.png"));
	data->photogirl = al_load_bitmap(GetDataFilePath(game, "polaroid_dziewczynka.png"));

	data->wstazka = al_load_bitmap(GetDataFilePath(game, "polaroid_kokardka.png"));

	data->bmp = al_create_bitmap(1920/2 + 200, 100 + 300*game->data->score + 550);
	al_set_target_bitmap(data->bmp);
	al_clear_to_color(al_map_rgba(0,0,0,0));

	al_draw_text(data->font, al_map_rgb(0,0,0), 1920/4, 10, ALLEGRO_ALIGN_CENTER, "IN MEMORY OF");

	for (int i=0; i<game->data->score; i++) {
		bool girl = false;
		if ((rand() / (float)RAND_MAX) <= 0.33) {
			girl = true;
		}
		char* name = names_male[rand() % (sizeof(names_male) / sizeof(char*))];
		if (girl) {
			name = names_female[rand() % (sizeof(names_female) / sizeof(char*))];
		}
		char *reason = reasons_common[rand() % (sizeof(reasons_common) / sizeof(char*))];

		if ((rand() / (float)RAND_MAX) <= 0.2) {
			reason = reasons_male[rand() % (sizeof(reasons_male) / sizeof(char*))];

			if (girl) {
				reason = reasons_female[rand() % (sizeof(reasons_female) / sizeof(char*))];
			}
		}

		ALLEGRO_BITMAP *photo = data->photo1;
		if ((rand() / (float)RAND_MAX) <= 0.5) {
			photo = data->photo2;
		}
		if (girl) {
			if ((rand() / (float)RAND_MAX) <= 0.2) {
				photo = data->photogirl;
			}
		}


		bool left = true;
		if (i%2) left = false;


		if (left) {
			//al_draw_text(data->font, al_map_rgb(0,0,0), 10, 100+300*i, ALLEGRO_ALIGN_LEFT, name);
			DrawWrappedText(data->font, al_map_rgb(0,0,0), 280, 100+300*i+20 + 80, 1920/3, ALLEGRO_ALIGN_LEFT, reason);

			al_draw_scaled_bitmap(photo, 0, 0, al_get_bitmap_width(photo), al_get_bitmap_height(photo),
			               10, 100+300*i+20, 250, 268, 0);

			al_draw_text(data->font, al_map_rgb(0,0,0), 25, 100+300*i+20 + 215, ALLEGRO_ALIGN_LEFT, name);


			al_draw_bitmap(data->wstazka, 10 + 185 - (rand() / (float)RAND_MAX)*10, 100+300*i+20 + 177 + (rand() / (float)RAND_MAX)*10, 0);
		} else {
			DrawWrappedText(data->font, al_map_rgb(0,0,0), 10, 100+300*i+20 + 80, 1920/3, ALLEGRO_ALIGN_LEFT, reason);
			al_draw_scaled_bitmap(photo, 0, 0, al_get_bitmap_width(photo), al_get_bitmap_height(photo),
			                      650, 100+300*i+20, 250, 268, 0);

			al_draw_text(data->font, al_map_rgb(0,0,0), 665, 100+300*i+20 + 215, ALLEGRO_ALIGN_LEFT, name);

			al_draw_bitmap(data->wstazka, 650 + 185 - (rand() / (float)RAND_MAX)*10, 100+300*i+20 + 177 + (rand() / (float)RAND_MAX)*10, 0);

		}


	}

	al_draw_text(data->font, al_map_rgb(0,0,0), 1920/2, 100+300*game->data->score + 480, ALLEGRO_ALIGN_CENTER, "Fin.");


	data->music = al_load_audio_stream(GetDataFilePath(game, "cannonfodder.ogg"), 4, 1024);
	al_attach_audio_stream_to_mixer(data->music, game->audio.music);
	al_set_audio_stream_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);

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
	data->pos = 1080;
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

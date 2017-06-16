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

static const char *reasons_common[] = {
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
  "You two had a nice time on a game jam",
  "Never forgot about your birthday",
  "You used to go out for karaoke",
  "Taught you how to play ukulele",
  "Showed you your favourite band",
  "You were going to the cinema this weekend",
  "Trusted you the most",
  "Just became a parent",
  "Always smiling",
  "Liked the same music as you do",
  "Shared the Netflix account with you",
  "Gave the best WALL-E impressions",
  "Was able to perform perfect moonwalk",
  "Left WoW for some dancing for the first time",
  "You met on the Web"
};
static const char *reasons_male[] = {
  "Was the witness at your wedding",
  "This one funny guy at your workplace",
  "Your brother",
  "You couldn't remember his last name",
  "Your favourite wingman",
  "You won't ever forget his vegan lasagne"
};
static const char *reasons_female[] = {
  "Went with you to your high school prom",
  "Had a crush on you",
  "You used to go out in the high school",
  "Your sister",
  "Your wife",
  "Your ex-girlfriend"
};
static const char *names_male[] = {
  "Rob", "John", "Zbyszek", "Frank", "Tom", "Sebastian", "David", "Mark", "Marty", "Marek",
  "Stefan", "Bob", "Paulo", "Julio", "Max", "Gandalf", "Otto", "Steve", "Justine", "Phil",
  "Donald", "Marcin", "Jackson", "Lucas", "Oliver", "Michael", "James", "Alex", "Mickey", "Istvan"
};
static const char *names_female[] = {
  "Anne", "Mary", "Suzanne", "Ela", "Ola", "Marta", "Julie", "Stephanie", "Agata", "Claudia",
  "Laura", "Sophia", "Emma", "Olivia", "Zoe", "Lily", "Amelia", "Emily", "Chloe", "Shakira",
  "Elaine", "Jasmine", "Angie", "April", "Kate", "Kasia", "Basia", "Zosia", "Eszter", "Hania"
};

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_FONT *font;
		int blink_counter;
		float counter;

		ALLEGRO_BITMAP *bmp, *tmp;

		ALLEGRO_BITMAP *bg, *bg2;
		ALLEGRO_AUDIO_STREAM *music;

		ALLEGRO_BITMAP *photo1, *photo2, *photogirl, *wstazka;

		float pos;

		bool used_common[sizeof(reasons_common) / sizeof(char*)];
		bool used_male[sizeof(reasons_male) / sizeof(char*)];
		bool used_female[sizeof(reasons_female) / sizeof(char*)];
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	if (data->blink_counter < 120) {
		data->blink_counter++;
	}
	data->counter += 0.0005;

	if (data->pos > -(100 + 300*game->data->score + 250)) {
		data->pos -= 0.75;

		ALLEGRO_KEYBOARD_STATE kbd;
		al_get_keyboard_state(&kbd);

		if (al_key_down(&kbd, ALLEGRO_KEY_SPACE)) {
			data->pos -= 9.25;
		}
	}
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_draw_bitmap(data->bg, -240 + sin(data->counter) * 200, -160,0);
	al_draw_bitmap(data->bg2, -240, -160,0);
	al_draw_bitmap(data->bmp, 1920/2 - 30, data->pos, 0);

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
	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->font = al_load_ttf_font(GetDataFilePath(game, "fonts/belligerent.ttf"), 48, 0);
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	//game->data->score = 30;

	for (int i=0; i<(sizeof(data->used_common)/sizeof(bool)); i++) {
		data->used_common[i] = false;
	}
	for (int i=0; i<(sizeof(data->used_male)/sizeof(bool)); i++) {
		data->used_male[i] = false;
	}
	for (int i=0; i<(sizeof(data->used_female)/sizeof(bool)); i++) {
		data->used_female[i] = false;
	}

	data->bg = al_load_bitmap(GetDataFilePath(game, "cmentarz_tyl.png"));
	data->bg2 = al_load_bitmap(GetDataFilePath(game, "cmentarz_przod.png"));

	data->photo1 = al_load_bitmap(GetDataFilePath(game, "polaroid_chlopczyk.png"));
	data->photo2 = al_load_bitmap(GetDataFilePath(game, "polaroid_chlopczyk2.png"));
	data->photogirl = al_load_bitmap(GetDataFilePath(game, "polaroid_dziewczynka.png"));

	data->wstazka = al_load_bitmap(GetDataFilePath(game, "polaroid_kokardka.png"));

	data->tmp = al_create_bitmap(300, 300);

	data->bmp = al_create_bitmap(1920/2 + 200, 100 + 300*game->data->score + 550);
	al_set_target_bitmap(data->bmp);
	al_clear_to_color(al_map_rgba(0,0,0,0));

	al_draw_text(data->font, al_map_rgb(0,0,0), 1920/4, 10, ALLEGRO_ALIGN_CENTER, "IN MEMORY OF");

	for (int i=0; i<game->data->score; i++) {


		bool left = false;
		for (int i=0; i<(sizeof(data->used_common)/sizeof(bool)); i++) {
			if (!data->used_common[i]) {
				left = true;
			}
		}
		if (!left) {
			for (int i=0; i<(sizeof(data->used_common)/sizeof(bool)); i++) {
				data->used_common[i] = false;
			}
		}
		left = false;

		for (int i=0; i<(sizeof(data->used_male)/sizeof(bool)); i++) {
			if (!data->used_male[i]) {
				left = true;
			}
		}
		if (!left) {
			for (int i=0; i<(sizeof(data->used_male)/sizeof(bool)); i++) {
				data->used_male[i] = false;
			}
		}
		left = false;

		for (int i=0; i<(sizeof(data->used_female)/sizeof(bool)); i++) {
			if (!data->used_female[i]) {
				left = true;
			}
		}
		if (!left) {
			for (int i=0; i<(sizeof(data->used_female)/sizeof(bool)); i++) {
				data->used_female[i] = false;
			}
		}



		bool girl = false;
		if ((rand() / (float)RAND_MAX) <= 0.4) {
			girl = true;
		}
		const char* name = names_male[rand() % (sizeof(names_male) / sizeof(char*))];
		if (girl) {
			name = names_female[rand() % (sizeof(names_female) / sizeof(char*))];
		}
		int num;
		do {
			num = rand() % (sizeof(reasons_common) / sizeof(char*));
		} while (data->used_common[num]);
		const char *reason = reasons_common[num];
		data->used_common[num] = true;

		if ((rand() / (float)RAND_MAX) <= 0.1) {
			do {
				num = rand() % (sizeof(reasons_male) / sizeof(char*));
			} while (data->used_male[num]);
			reason = reasons_male[num];
			data->used_male[num] = true;

			if (girl) {
				do {
					num = rand() % (sizeof(reasons_female) / sizeof(char*));
				} while (data->used_female[num]);
				reason = reasons_female[num];
				data->used_female[num] = true;
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


		bool right = false;
		if (i%2) right = true;


		al_set_target_bitmap(data->tmp);
		al_clear_to_color(al_map_rgba(0,0,0,0));
		al_draw_scaled_bitmap(photo, 0, 0, al_get_bitmap_width(photo), al_get_bitmap_height(photo),
		               10, 10, 250, 268, 0);
		al_draw_text(data->font, al_map_rgb(0,0,0), 25, 225, ALLEGRO_ALIGN_LEFT, name);

		al_draw_bitmap(data->wstazka, 10 + 185 + (rand() / (float)RAND_MAX)*10, 20 + 177 - (rand() / (float)RAND_MAX)*10, 0);
		al_set_target_bitmap(data->bmp);

		if (!right) {
			//al_draw_text(data->font, al_map_rgb(0,0,0), 10, 100+300*i, ALLEGRO_ALIGN_LEFT, name);
			DrawWrappedText(data->font, al_map_rgb(0,0,0), 280, 100+300*i+20 + 80, 1920/3, ALLEGRO_ALIGN_LEFT, reason);

			al_draw_rotated_bitmap(data->tmp, 150, 150, 10 + 150, 100+300*i+10 + 150, -1/24.0, 0);
		} else {
			DrawWrappedText(data->font, al_map_rgb(0,0,0), 10, 100+300*i+20 + 80, 1920/3, ALLEGRO_ALIGN_LEFT, reason);
			al_draw_rotated_bitmap(data->tmp, 150, 150, 640 + 150, 100+300*i+10 + 150, 1/24.0, 0);
		}


	}

	al_draw_text(data->font, al_map_rgb(0,0,0), 1920/2 - 100, 100+300*game->data->score + 480, ALLEGRO_ALIGN_CENTER, "Fin.");


	data->music = al_load_audio_stream(GetDataFilePath(game, "cannonfodder.ogg"), 4, 1024);
	al_attach_audio_stream_to_mixer(data->music, game->audio.music);
	al_set_audio_stream_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);

	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	al_destroy_bitmap(data->tmp);
	al_destroy_bitmap(data->bg);
	al_destroy_bitmap(data->bg2);
	al_destroy_bitmap(data->photo1);
	al_destroy_bitmap(data->photo2);
	al_destroy_bitmap(data->photogirl);
	al_destroy_bitmap(data->wstazka);
	al_destroy_audio_stream(data->music);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->blink_counter = 0;
	data->pos = 1080;
	data->counter = 0;
	PrintConsole(game, "score: %d", game->data->score);
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

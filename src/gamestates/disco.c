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
#include <stdio.h>
#include <libsuperderpy.h>

#define NUMBER_OF_PAJONKS 50

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_FONT *font;
		int blink_counter;

		float wind;

		struct Character *pajonczek, *dron, *kula;
		struct Character *pajonczki[NUMBER_OF_PAJONKS];

		ALLEGRO_AUDIO_STREAM *music;//, *elevator;

		ALLEGRO_SAMPLE *boom_sample, *death_sample;
		ALLEGRO_SAMPLE_INSTANCE *boom, *death;

		ALLEGRO_SAMPLE *oops_samples[17];
		ALLEGRO_SAMPLE_INSTANCE *oops[17];

		ALLEGRO_BITMAP *bg, *web;
		ALLEGRO_BITMAP *disco[6], *matryca;
		ALLEGRO_BITMAP *duzepole;

		struct {
				ALLEGRO_BITMAP *bmp;
				int x1, y1, x2, y2;
		} pola[20][6];

		ALLEGRO_BITMAP *listek03, *roslinka04, *wp05, *listek1, *listek2, *listek3, *cien;

		ALLEGRO_BITMAP *nozka1, *nozka2, *nozka3, *nozka4, *shadow;
		int nozka;
		float noga1, noga2, noga3, noga4;
		float noga1x, noga2x, noga3x, noga4x;
		float noga1y, noga2y, noga3y, noga4y;

		ALLEGRO_BITMAP *tmp, *mask, *chleb;

		float discocount;

		int shake;

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
		bool dead;
};

int Gamestate_ProgressCount = 260 + NUMBER_OF_PAJONKS; // number of loading steps as reported by Gamestate_Load

void CheckCollision(struct Game *game, struct GamestateResources* data, int x, int y) {
	bool dead = false;
	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		if (IsOnCharacter(game, data->pajonczki[i], x+10, y+10) || IsOnCharacter(game, data->pajonczki[i], x+36, y+10)
		    || IsOnCharacter(game, data->pajonczki[i], x+10, y+14) || IsOnCharacter(game, data->pajonczki[i], x+36, y+14)) {
			SelectSpritesheet(game, data->pajonczki[i], "dead");
			struct PajonkData *d = data->pajonczki[i]->data;
			if (!d->dead) {
				game->data->score++;
				d->dead = true;
				dead = true;
			}
		}
	}
	al_play_sample_instance(data->boom);
	data->shake = rand() % 10 + 25;
	if (dead) {
		al_play_sample_instance(data->death);
		al_play_sample_instance(data->oops[rand() % 17]);
	}
}

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->blink_counter++;

	if (data->shake) {
		    data->shake--;
	}

	AnimateCharacter(game, data->kula, 1);


float pos = data->blink_counter / 160.0;
if (pos > 1) {
	pos = 1;
}
  SetCharacterPosition(game, data->kula, 1200, -700 + 666*pos, 0);


	data->wind += 0.0125;
	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		struct PajonkData *d = data->pajonczki[i]->data;
		if (d->dead) continue;
		d->angle_mod += d->speed * (d->right ? 1 : -1);
		if (fabs(d->angle_mod) >= d->angle_range) {
			d->right = !d->right;
		}
		d->sin += d->speed * 10.0;
		AnimateCharacter(game, data->pajonczki[i], 1);

		if (rand() % 300 == 0) {
			d->angle += d->angle_mod;
			d->angle_mod = 0;
			//d->r = -d->r;
			d->right = rand() % 2;
			d->dead = false;
			d->angle_range = (rand() / (float)RAND_MAX) * 0.33 + 0.1;
			d->speed = (rand() / (float)RAND_MAX) * 0.005 + 0.005;
		}
	}
	AnimateCharacter(game, data->dron, 1);
	data->discocount += 0.0318;
	if (data->discocount >= 6) {
		data->discocount -= 5;
	}
	data->pole += 0.05;
	if (data->pole >= 20*6) {
		data->pole = 0;
	}

//	al_draw_bitmap(data->shadow, 845 + 116 + data->noga3x, 150 + 100 + data->noga3y, 0);
//	al_draw_bitmap(data->shadow, 887 + 195 + data->noga4x, 268 + 125 + data->noga4y, 0);
//	al_draw_bitmap(data->shadow, 589 + 0 + data->noga1x, 285 + 115 + data->noga1y, 0);
//	al_draw_bitmap(data->shadow, 683 + 11 + data->noga2x, 379 + 160 + data->noga2y, 0);

	if (data->nozka == 1) {
		data->noga1 += 0.05;
		if (data->noga1 > 2*ALLEGRO_PI) {
			data->nozka++;
			data->noga1 -= 2*ALLEGRO_PI;

			CheckCollision(game, data, 589 + 0 + data->noga1x, 285 + 115 + data->noga1y);
		}
	} else if (data->nozka == 2) {
		data->noga2 += 0.05;
		if (data->noga2 > 2*ALLEGRO_PI) {
			data->nozka++;
			data->noga2 -= 2*ALLEGRO_PI;

			CheckCollision(game, data, 683 + 11 + data->noga2x, 379 + 160 + data->noga2y);
		}
	} else if (data->nozka == 3) {
		data->noga3 += 0.05;
		if (data->noga3 > 2*ALLEGRO_PI) {
			data->nozka++;
			data->noga3 -= 2*ALLEGRO_PI;

			CheckCollision(game, data, 845 + 116 + data->noga3x, 150 + 100 + data->noga3y);
		}
	} else if (data->nozka == 4) {
		data->noga4 += 0.05;
		if (data->noga4 > 2*ALLEGRO_PI) {
			data->nozka=1;
			data->noga4 -= 2*ALLEGRO_PI;

			CheckCollision(game, data, 887 + 195 + data->noga4x, 268 + 125 + data->noga4y);
		}
	}

	if (!al_get_audio_stream_playing(data->music)) {
		SwitchCurrentGamestate(game, "outro");
	}
}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	al_draw_bitmap(data->bg, -240 + sin(data->wind) * 4, -160,0);

	int shake = data->shake ? rand() % 10 : 0;

	al_draw_bitmap(data->disco[(int)data->discocount], 480 + shake, 158 + sin(data->wind) * 4 + shake, 0);


	int next = (int)data->discocount;
	next--;
	next += 6;
	next++;
	next++;
	next = next%6;
	next++;

	int prev = (data->blink_counter/15) % 6 + 1;

	//PrintConsole(game, "disco: %d, prev: %d, next: %d", (int)data->discocount, prev, next);

	al_set_target_bitmap(data->tmp);
	al_clear_to_color(al_map_rgba(0,0,0,0));

	al_draw_bitmap(data->disco[prev], 480 + shake, 158 + shake + sin(data->wind) * 4, 0);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ZERO, ALLEGRO_ALPHA); // now as a mask
	al_draw_bitmap(data->duzepole, 480 + shake, 158 + shake + sin(data->wind) * 4 + 2, 0);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

	al_set_target_backbuffer(game->display);
	al_draw_bitmap(data->tmp, 0, 0, 0);

	al_set_target_bitmap(data->tmp);
	al_clear_to_color(al_map_rgba(0,0,0,0));

	al_draw_bitmap(data->disco[next], 480 + shake, 158 + shake + sin(data->wind) * 4, 0);

	//int p = (int)data->pole;
	//al_draw_bitmap(data->pola[p/6][p%6], 480, 158 + sin(data->wind) * 4, 0);
	al_set_target_bitmap(data->mask);
	al_clear_to_color(al_map_rgba(0,0,0,0));

	int blinkmode = (data->blink_counter/252) % 6;

	if (blinkmode == 0) {
		int p = (data->blink_counter/4) % 20;
		for (int i=0; i<6; i++) {
			al_draw_bitmap(data->pola[p][i].bmp, data->pola[p][i].x1 + 480 + shake, data->pola[p][i].y1 + 158 + shake + sin(data->wind) * 4, 0);
		}
	} else if (blinkmode == 1) {
		int p = (data->blink_counter/9) % 6;
		for (int i=0; i<20; i++) {
			al_draw_bitmap(data->pola[i][p].bmp, data->pola[i][p].x1 + 480 + shake, data->pola[i][p].y1 + 158 + shake + sin(data->wind) * 4, 0);
		}
	} else if (blinkmode == 2) {
		int k = 0;
		for (int i=0; i<6; i++) {
			for (int j=0; j<20; j++) {
				if (k%2==(data->blink_counter/25)%2) {
					al_draw_bitmap(data->pola[j][i].bmp, data->pola[j][i].x1 + 480 + shake, data->pola[j][i].y1 + 158 + shake + sin(data->wind) * 4, 0);
				}
				k++;
			}
		}
	} else if (blinkmode == 3) {
		int p = 5 - (data->blink_counter/9) % 6;
		for (int i=0; i<20; i++) {
			al_draw_bitmap(data->pola[i][p].bmp, data->pola[i][p].x1 + 480 + shake, data->pola[i][p].y1 + 158 + shake + sin(data->wind) * 4, 0);
		}
	} else if (blinkmode == 4) {
		int k = 0;
		for (int i=0; i<6; i++) {
			for (int j=0; j<20; j++) {
				if (k%3==(data->blink_counter/18)%3) {
					al_draw_bitmap(data->pola[j][i].bmp, data->pola[j][i].x1 + 480 + shake, data->pola[j][i].y1 + 158 + shake + sin(data->wind) * 4, 0);
				}
				k++;
			}
		}
	} else if (blinkmode == 5) {
		int k = 0;
		for (int i=0; i<6; i++) {
			for (int j=0; j<20; j++) {
				if (i%2) {
					al_draw_bitmap(data->pola[j][i].bmp, data->pola[j][i].x1 + 480 + shake, data->pola[j][i].y1 + 158 + shake + sin(data->wind) * 4, 0);
				}
				k++;
			}
		}
	}

/*	for (int i=0; i<6; i++) {
		for (int j=0; j<20; j++) {
				al_draw_bitmap(data->pola[j][i].bmp, data->pola[j][i].x1 + 480 + shake, data->pola[j][i].y1 + 158 + shake + sin(data->wind) * 4, 0);
		}
	}*/

	al_set_target_bitmap(data->tmp);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ZERO, ALLEGRO_ALPHA); // now as a mask
	al_draw_bitmap(data->mask, 0, 0, 0);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

	al_set_target_backbuffer(game->display);
	al_draw_bitmap(data->tmp, 0, 0, 0);


	al_draw_bitmap(data->web, -38 + shake, -160 + shake + sin(data->wind) * 4, 0);

	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		struct PajonkData *d = data->pajonczki[i]->data;
		if (!d->dead) continue;
		int centerx = 900, centery = 525;
		//d->angle = data->wind; //(rand() / (float)RAND_MAX) * 2*ALLEGRO_PI;

		int r = d->r + sin(d->sin) * 20;
		int a = sin(d->angle + d->angle_mod) * r;
		int b = cos(d->angle + d->angle_mod) * r;
		SetCharacterPosition(game, data->pajonczki[i], centerx + b + shake + 20, centery + a + shake + 20, 0);

		//SetCharacterPositionF(game, data->pajonczki[i], (rand() / (float)RAND_MAX) / 2.5 + 0.3 - 0.05, (rand() / (float)RAND_MAX) / 1.5 + 0.165, 0);
		DrawCharacter(game, data->pajonczki[i], al_map_rgb(255,255,255), 0);
	}


	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		struct PajonkData *d = data->pajonczki[i]->data;
		if (d->dead) continue;
		int centerx = 900, centery = 525;
		//d->angle = data->wind; //(rand() / (float)RAND_MAX) * 2*ALLEGRO_PI;

		int r = d->r + sin(d->sin) * 20;
		int a = sin(d->angle + d->angle_mod) * r;
		int b = cos(d->angle + d->angle_mod) * r;
		SetCharacterPosition(game, data->pajonczki[i], centerx + b, centery + a, 0);

		//SetCharacterPositionF(game, data->pajonczki[i], (rand() / (float)RAND_MAX) / 2.5 + 0.3 - 0.05, (rand() / (float)RAND_MAX) / 1.5 + 0.165, 0);
		DrawCharacter(game, data->pajonczki[i], al_map_rgb(255,255,255), 0);
	}


	SetCharacterPosition(game, data->dron, 775 + cos(data->wind * 5) * 3, 268, 0);

	al_draw_bitmap(data->shadow, 845 + 116 + data->noga3x, 150 + 100 + data->noga3y, 0);
	al_draw_bitmap(data->shadow, 887 + 195 + data->noga4x, 268 + 125 + data->noga4y, 0);
	al_draw_bitmap(data->shadow, 589 + 0 + data->noga1x, 285 + 115 + data->noga1y, 0);
	al_draw_bitmap(data->shadow, 683 + 11 + data->noga2x, 379 + 160 + data->noga2y, 0);

	al_draw_rotated_bitmap(data->nozka3, 12, 148, 845 + 12 + data->noga3x, 150 + 148 + data->noga3y, -(cos(data->noga3 + ALLEGRO_PI) + 1) / 5.0, 0);
	al_draw_rotated_bitmap(data->nozka4, 15, 108, 887 + 15 + data->noga4x, 268 + 108 + data->noga4y, -(cos(data->noga4 + ALLEGRO_PI) + 1) / 5.0, 0);
	DrawCharacter(game, data->dron, al_map_rgb(255,255,255), 0);
	al_draw_rotated_bitmap(data->nozka1, 234, 56, 589 + 234 + data->noga1x, 285 + 56 + data->noga1y, (cos(data->noga1 + ALLEGRO_PI) + 1) / 5.0, 0);
	al_draw_rotated_bitmap(data->nozka2, 175, 16, 683 + 175 + data->noga2x, 379 + 16 + data->noga2y, (cos(data->noga2 + ALLEGRO_PI) + 1) / 5.0, 0);
	al_draw_bitmap(data->chleb, 775 + cos(data->wind * 5) * 3, 268, 0);


	DrawScaledCharacter(game, data->kula, al_map_rgb(255,255,255), 0.75, 0.75, 0);


	al_draw_bitmap(data->listek03, 566, 598,0);
	al_draw_rotated_bitmap(data->roslinka04, 512, 1390, 1221 + 512, -100 + 1390, sin(data->wind / 2.0 + 2.34) / 50.0, 0);
	al_draw_bitmap(data->wp05, -240, -160,0);

	//al_draw_bitmap(data->listek1, 1065, 644,0);
	al_draw_rotated_bitmap(data->listek1, 920, 430, 1065+920, 644+430, cos(data->wind + 1) / 60.0, 0);

	al_draw_rotated_bitmap(data->listek2, 0, 588, -94, 534 + 588, sin(data->wind / 1.5 + 5.298) / 20.0, 0);

	//al_draw_bitmap(data->listek2, -94, 534,0);
	//al_draw_bitmap(data->listek3, -94, -123,0);
	al_draw_rotated_bitmap(data->listek3, 145, 40, -94 + 145, -123 + 40, sin(data->wind / 2.5 + 0.1234) / 30.0, 0);


	al_draw_tinted_bitmap(data->cien, al_map_rgba_f(0.1,0.1,0.1,0.4), 1282, -363,0);

}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		SwitchCurrentGamestate(game, "outro"); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}

	if (ev->type == ALLEGRO_EVENT_MOUSE_AXES) {
		  int mousex = ev->mouse.dx / (al_get_display_width(game->display) / 1920.0) / 10.0;
			int mousey = 0;//ev->mouse.dy / (al_get_display_width(game->display) / 1080.0) / 10.0;

			if (data->nozka == 1) {
				data->noga1x += mousex;
				data->noga1y += mousey;
			} else if (data->nozka == 2) {
				data->noga2x += mousex;
				data->noga2y += mousey;
			} else if (data->nozka == 3) {
				data->noga3x += mousex;
				data->noga3y += mousey;
			} else if (data->nozka == 4) {
				data->noga4x += mousex;
				data->noga4y += mousey;
			}

			int range = 25;
			if (data->noga1x < -range) {
				data->noga1x = -range;
			}
			if (data->noga1x > range) {
				data->noga1x = range;
			}
			if (data->noga2x < -range) {
				data->noga2x = -range;
			}
			if (data->noga2x > range) {
				data->noga2x = range;
			}
			if (data->noga3x < -range) {
				data->noga3x = -range;
			}
			if (data->noga3x > range) {
				data->noga3x = range;
			}
			if (data->noga4x < -range) {
				data->noga4x = -range;
			}
			if (data->noga4x > range) {
				data->noga4x = range;
			}
			if (data->noga1y < -range) {
				data->noga1y = -range;
			}
			if (data->noga1y > range) {
				data->noga1y = range;
			}
			if (data->noga2y < -range) {
				data->noga2y = -range;
			}
			if (data->noga2y > range) {
				data->noga2y = range;
			}
			if (data->noga3y < -range) {
				data->noga3y = -range;
			}
			if (data->noga3y > range) {
				data->noga3y = range;
			}
			if (data->noga4y < -range) {
				data->noga4y = -range;
			}
			if (data->noga4y > range) {
				data->noga4y = range;
			}

			al_set_mouse_xy(game->display, al_get_display_width(game->display) / 2, al_get_display_height(game->display) / 2);
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

	struct GamestateResources *data = malloc(sizeof(struct GamestateResources));
	data->font = al_create_builtin_font();

	/*data->elevator = al_load_audio_stream(GetDataFilePath(game, "elevator.flac"), 4, 1024);
	al_set_audio_stream_playing(data->elevator, true);
	al_attach_audio_stream_to_mixer(data->elevator, game->audio.music);
	al_set_audio_stream_gain(data->elevator, 0.8);
	al_set_audio_stream_playmode(data->elevator, ALLEGRO_PLAYMODE_LOOP);*/


	data->boom_sample = al_load_sample( GetDataFilePath(game, "boom.flac") );
	data->boom = al_create_sample_instance(data->boom_sample);
	al_attach_sample_instance_to_mixer(data->boom, game->audio.fx);
	al_set_sample_instance_playmode(data->boom, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(data->boom, 0.5);

	data->death_sample = al_load_sample( GetDataFilePath(game, "dead.flac") );
	data->death = al_create_sample_instance(data->death_sample);
	al_attach_sample_instance_to_mixer(data->death, game->audio.fx);
	al_set_sample_instance_playmode(data->death, ALLEGRO_PLAYMODE_ONCE);
	al_set_sample_instance_gain(data->death, 0.5);


	for (int i=0; i<17; i++) {
		char *filename = malloc(255 * sizeof(char));
		snprintf(filename, 255, "oops/%d.wav", i);

		data->oops_samples[i] = al_load_sample( GetDataFilePath(game, filename) );
		data->oops[i] = al_create_sample_instance(data->oops_samples[i]);
		al_attach_sample_instance_to_mixer(data->oops[i], game->audio.voice);
		al_set_sample_instance_playmode(data->oops[i], ALLEGRO_PLAYMODE_ONCE);
		free(filename);
	}

	data->pajonczek = CreateCharacter(game, "pajonczek");
	RegisterSpritesheet(game, data->pajonczek, "stand");
	RegisterSpritesheet(game, data->pajonczek, "dead");
	LoadSpritesheets(game, data->pajonczek);

	data->dron = CreateCharacter(game, "dron");
	RegisterSpritesheet(game, data->dron, "dance");
	LoadSpritesheets(game, data->dron);


	data->kula = CreateCharacter(game, "kula");
	RegisterSpritesheet(game, data->kula, "kula");
	LoadSpritesheets(game, data->kula);

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

	/*
	 // mask generation code
	al_lock_bitmap(data->matryca, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
	for (int i=0; i<20; i++) {
		for (int j=0; j<6; j++) {
			data->pola[i][j].bmp = al_create_bitmap(872, 792);
			data->pola[i][j].x1 = 1000;
			data->pola[i][j].y1 = 1000;
			data->pola[i][j].x2 = -1;
			data->pola[i][j].y2 = -1;
			al_lock_bitmap(data->pola[i][j].bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		}
	}
	data->duzepole.bmp = al_create_bitmap(872, 792);
	al_lock_bitmap(data->duzepole.bmp, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
	ALLEGRO_COLOR transparent = al_map_rgba(0, 0, 0, 0);
	ALLEGRO_COLOR black = al_map_rgba(0, 0, 0, 255);
	for (int x=0; x<872; x++) {
		for (int y=0; y<792; y++) {
			ALLEGRO_COLOR color = al_get_pixel(data->matryca, x, y);
			for (int i=0; i<20; i++) {
				for (int j=0; j<6; j++) {
					al_set_target_bitmap(data->pola[i][j].bmp);
					al_put_pixel(x, y, transparent);
					//PrintConsole(game, "r: %d, g: %d", color.r, color.g);
					if (color.r*255 == 60 + i*10) {
						if (color.g*255 == 100 + j*30) {
							al_put_pixel(x, y, black);

							if (data->pola[i][j].x1 > x) {
								data->pola[i][j].x1 = x;
							}
							if (data->pola[i][j].y1 > y) {
								data->pola[i][j].y1 = y;
							}
							if (data->pola[i][j].x2 < x) {
								data->pola[i][j].x2 = x;
							}
							if (data->pola[i][j].y2 < y) {
								data->pola[i][j].y2 = y;
							}

						}
					}
				}
			}
			al_set_target_bitmap(data->duzepole.bmp);
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
			al_unlock_bitmap(data->pola[i][j].bmp);

			char filename[255];
			snprintf(filename, 255, "mask-%d-%d.png", i, j);

			ALLEGRO_BITMAP *tmp;
			if (data->pola[i][j].x2 == -1) {
				tmp = al_create_bitmap(1, 1);
			} else {
				tmp = al_create_bitmap(data->pola[i][j].x2 - data->pola[i][j].x1 + 1, data->pola[i][j].y2 - data->pola[i][j].y1 + 1);
			}
			al_set_target_bitmap(tmp);
			al_clear_to_color(al_map_rgba(0,0,0,0));
			al_draw_bitmap(data->pola[i][j].bmp, -data->pola[i][j].x1, -data->pola[i][j].y1, 0);

			al_save_bitmap(filename, tmp);

			ALLEGRO_CONFIG *config = al_create_config();
			snprintf(filename, 255, "%d", data->pola[i][j].x1);
			al_set_config_value(config, "", "x", filename);
			snprintf(filename, 255, "%d", data->pola[i][j].y1);
			al_set_config_value(config, "", "y", filename);
			snprintf(filename, 255, "mask-%d-%d.ini", i, j);

			al_save_config_file(filename, config);
		}
	}
	al_unlock_bitmap(data->duzepole.bmp);
	al_unlock_bitmap(data->matryca);
	progress(game);
	*/

	for (int i=0; i<20; i++) {
		for (int j=0; j<6; j++) {
			char filename[255];
			snprintf(filename, 255, "mask/mask-%d-%d.png", i, j);

			data->pola[i][j].bmp = al_load_bitmap(GetDataFilePath(game, filename));
			progress(game);

			snprintf(filename, 255, "mask/mask-%d-%d.ini", i, j);
			ALLEGRO_CONFIG *config = al_load_config_file(GetDataFilePath(game, filename));

			data->pola[i][j].x1 = atoi(al_get_config_value(config, "", "x")) + 1;
			data->pola[i][j].y1 = atoi(al_get_config_value(config, "", "y")) + 2;

			al_destroy_config(config);
			progress(game);
		}
	}
	data->duzepole = al_load_bitmap(GetDataFilePath(game, "mask/mask-duze.png"));

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

	data->nozka1 = al_load_bitmap(GetDataFilePath(game, "nozka01.png"));
	data->nozka2 = al_load_bitmap(GetDataFilePath(game, "nozka02.png"));
	data->nozka3 = al_load_bitmap(GetDataFilePath(game, "nozka03.png"));
	data->nozka4 = al_load_bitmap(GetDataFilePath(game, "nozka04.png"));
	data->shadow = al_load_bitmap(GetDataFilePath(game, "cien.png"));
	data->chleb = al_load_bitmap(GetDataFilePath(game, "chleb.png"));
	progress(game);

	data->music = al_load_audio_stream(GetDataFilePath(game, "startrek.flac"), 4, 1024);
	al_set_audio_stream_playing(data->music, false);
	al_set_audio_stream_gain(data->music, 0.85);
	al_attach_audio_stream_to_mixer(data->music, game->audio.music);
	al_set_audio_stream_playmode(data->music, ALLEGRO_PLAYMODE_ONCE);
	progress(game);

	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		data->pajonczki[i] = CreateCharacter(game, "pajonczek");
		data->pajonczki[i]->shared = true;
		data->pajonczki[i]->spritesheets = data->pajonczek->spritesheets;
		data->pajonczki[i]->data = malloc(sizeof(struct PajonkData));
		progress(game);
	}

	/*al_set_audio_stream_playing(data->elevator, false);
	al_destroy_audio_stream(data->elevator);*/

	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);

	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		free(data->pajonczki[i]->data);
		DestroyCharacter(game, data->pajonczki[i]);
	}
	DestroyCharacter(game, data->pajonczek);
	DestroyCharacter(game, data->dron);
	DestroyCharacter(game, data->kula);
	al_destroy_audio_stream(data->music);

	al_destroy_sample_instance(data->boom);
	al_destroy_sample_instance(data->death);
	al_destroy_sample(data->boom_sample);
	al_destroy_sample(data->death_sample);

	for (int i=0; i<17; i++) {
		al_destroy_sample_instance(data->oops[i]);
		al_destroy_sample(data->oops_samples[i]);
	}

	al_destroy_bitmap(data->bg);
	al_destroy_bitmap(data->web);
	al_destroy_bitmap(data->listek03);
	al_destroy_bitmap(data->roslinka04);
	al_destroy_bitmap(data->wp05);
	al_destroy_bitmap(data->listek1);
	al_destroy_bitmap(data->listek2);
	al_destroy_bitmap(data->listek3);
	al_destroy_bitmap(data->cien);
	al_destroy_bitmap(data->matryca);
	al_destroy_bitmap(data->duzepole);

	al_destroy_bitmap(data->disco[0]);
	al_destroy_bitmap(data->disco[1]);
	al_destroy_bitmap(data->disco[2]);
	al_destroy_bitmap(data->disco[3]);
	al_destroy_bitmap(data->disco[4]);
	al_destroy_bitmap(data->disco[5]);

	for (int i=0; i<20; i++) {
		for (int j=0; j<6; j++) {
			al_destroy_bitmap(data->pola[i][j].bmp);
		}
	}

	al_destroy_bitmap(data->nozka1);
	al_destroy_bitmap(data->nozka2);
	al_destroy_bitmap(data->nozka3);
	al_destroy_bitmap(data->nozka4);
	al_destroy_bitmap(data->shadow);

	al_destroy_bitmap(data->mask);
	al_destroy_bitmap(data->tmp);
	al_destroy_bitmap(data->chleb);

	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->blink_counter = 0;
	data->shake = 0;
	game->data->score = 0;
	SelectSpritesheet(game, data->dron, "dance");
	SetCharacterPosition(game, data->dron, 775, 268, 0);

	SelectSpritesheet(game, data->kula, "kula");


	for (int i=0; i<NUMBER_OF_PAJONKS; i++) {
		SelectSpritesheet(game, data->pajonczki[i], "stand");
		//SetCharacterPositionF(game, data->pajonczki[i], (rand() / (float)RAND_MAX) / 2.5 + 0.3 - 0.05, (rand() / (float)RAND_MAX) / 1.5 + 0.165, 0);
		data->pajonczki[i]->pos = rand() % 3;
		struct PajonkData *d = data->pajonczki[i]->data;
		d->angle = (rand() / (float)RAND_MAX) * 2*ALLEGRO_PI;
		d->angle_mod = 0;
		d->right = rand() % 2;
		d->r = rand() % 225 + 125;
		d->dead = false;
		d->angle_range = (rand() / (float)RAND_MAX) * 0.33 + 0.1;
		d->speed = (rand() / (float)RAND_MAX) * 0.005 + 0.005;
		d->sin = (rand() / (float)RAND_MAX);
		data->pajonczki[i]->pos_tmp= rand() % 5;
	}

	data->wind = 0;
	al_set_audio_stream_playing(data->music, true);
	data->discocount = 0.5;
	data->pole = 0;
	data->nozka = 1;

	data->noga1 = 0;
	data->noga2 = 0;
	data->noga3 = 0;
	data->noga4 = 0;
	data->noga1x = 0;
	data->noga2x = 0;
	data->noga3x = 0;
	data->noga4x = 0;
	data->noga1y = 0;
	data->noga2y = 0;
	data->noga3y = 0;
	data->noga4y = 0;
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

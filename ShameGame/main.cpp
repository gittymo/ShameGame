#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#ifdef __APPLE__
#include <SDL/SDL.h>
#else
#include <SDL.h>
#endif

#include "Tile.h"
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>

using UK_CO_MEVANSPN_SHAMEGAME::Tile;
using std::cout;
using std::endl;
using std::string;
using std::ostringstream;

bool loadTileGraphics(SDL_Surface* _display, SDL_Surface** _array_ptr, int _set) {
	bool failure = false;
	const char* prefix = "../../block";
	Uint32 color_key = SDL_MapRGB(_display->format, 255, 0, 0);
	ostringstream* oss;
	for (int i = 0; i < 4 && !failure; i++) {
		oss = new ostringstream();
		*oss << prefix << _set << "-" << i << ".bmp";
		string filename_string = oss->str();
		_array_ptr[i] = SDL_LoadBMP(filename_string.c_str());
		if (_array_ptr[i] == NULL) {
			failure = true;
		} else {
			SDL_SetColorKey(_array_ptr[i], SDL_RLEACCEL | SDL_SRCCOLORKEY, color_key);
		}
		delete oss;
	}

	return !failure;
}

int main ( int argc, char** argv )
{
    // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // Give the window a title
    SDL_WM_SetCaption("ShameGame (v0.01a) - By Morgan Evans","ShameGame");

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // create a new window
    SDL_Surface* screen = SDL_SetVideoMode(640, 480, 32,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !screen )
    {
        printf("Unable to set 640x480 video: %s\n", SDL_GetError());
        return 1;
    }

    // create a surface for displaying the tiles
    SDL_Surface* play_area = SDL_CreateRGBSurface(SDL_HWSURFACE, 480, 320, 32,
		screen->format->Rmask, screen->format->Gmask, screen->format->Bmask,
		screen->format->Amask);
	SDL_Rect pap;
	pap.x = (screen->w - play_area->w) / 2;
	pap.y = (screen->h - play_area->h) / 2;
	pap.w = play_area->w;
	pap.h = play_area->h;

	// Let's load the tile graphics
	SDL_Surface* tile_sprites[5][4];
	for (int i = 0; i < 5; i++) {
		loadTileGraphics(play_area, tile_sprites[i], i);
	}

	// Create a grid of blocks
	const int COLUMNS = ((play_area->w - 32) / 32);
	const int ROWS = ((play_area->h - 32) / 32);
	int TILE_COUNT = COLUMNS * ROWS;
	Tile* tiles[TILE_COUNT];
	int tile_index = 0, y = 16, x= 16;
	for (int i = 0; i < TILE_COUNT; i++) {
		SDL_Rect *r = new SDL_Rect();
		r->w = 32;
		r->h = 32;
		r->x = x;
		r->y = y;
		const int tile_type = rand() % 5;
		tiles[tile_index++] = new Tile(tile_type, play_area, tile_sprites[tile_type], r);
		x += 32;
		if (x > play_area->w - 48) {
			x = 16;
			y += 32;
		}
	}

	// Set relations between the tiles
	for (int i = 0; i < TILE_COUNT; i++) {
		const int col_index = i % COLUMNS;
		const int row_index = i / COLUMNS;
		if (col_index < COLUMNS - 1) tiles[i]->setTileRight(tiles[i+1], tiles[i]);
		if (row_index < ROWS - 1) tiles[i]->setTileBelow(tiles[i + COLUMNS], tiles[i]);
	}

    // program main loop
    int mouse_x, mouse_y;
    bool done = false;
    while (!done)
    {
        // message processing loop
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
                {
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    break;
                }
            } // end switch
        } // end of message processing

        // DRAWING STARTS HERE

        // clear screen
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

		// Fill the play area with a plain background colour for now.
		SDL_FillRect(play_area, 0, SDL_MapRGB(play_area->format, 0, 0, 255));

        // Draw tiles to play area
		for (int i = 0; i < TILE_COUNT; i++) {
			if (tiles[i] != NULL && tiles[i]->isVisible())
				SDL_BlitSurface(tiles[i]->getSprite(), NULL, play_area,
					tiles[i]->getPosition());
			tiles[i]->moveVertical();
		}

		// Draw the play area to the screen
		SDL_BlitSurface(play_area, NULL, screen, &pap);

		// Get mouse state
		Uint8 button = SDL_GetMouseState(&mouse_x, &mouse_y);
		if (button&SDL_BUTTON(1)) {
			mouse_x -= pap.x;
			mouse_y -= pap.y;
			if (mouse_x >= 16 && mouse_x < pap.w - 16 && mouse_y >= 16
				&& mouse_y < pap.h - 16) {
					for (int i = 0; i < TILE_COUNT; i++) {
						if (mouse_x >= tiles[i]->getPosition()->x &&
							mouse_x < tiles[i]->getPosition()->x +
							tiles[i]->getPosition()->w &&
							mouse_y >= tiles[i]->getPosition()->y &&
							mouse_y < tiles[i]->getPosition()->y +
							tiles[i]->getPosition()->h && tiles[i]->isVisible()) {
								tiles[i]->destroy();
							}
					}
			}
		}

        // finally, update the screen :)
        SDL_Flip(screen);
        SDL_Delay(40);
    } // end main loop

    // free loaded bitmap
    for (int i = 0; i < TILE_COUNT; i++) {
    	if (tiles[i] != NULL) delete tiles[i];
    }

    for (int i = 0; i < 5; i++) {
    	for (int j = 0; j < 4; j++) {
    		delete tile_sprites[i][j];
    	}
    }

    // all is well ;)
    printf("Exited cleanly\n");
    return 0;
}

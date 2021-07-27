#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED

#include <SDL.h>
#include <math.h>

#ifndef NULL
	#define NULL 0
#endif

namespace UK_CO_MEVANSPN_SHAMEGAME {
 	class Tile {
 		public:
			Tile(int _type, SDL_Surface *_display, SDL_Surface** _sprite_data, SDL_Rect *_position);
			~Tile();
			bool isVisible();
			void destroy();
			SDL_Rect* getPosition();
			SDL_Surface* getSprite();
			void setTileRight(Tile *_tile, Tile *_left_tile);
			void setTileBelow(Tile *_tile, Tile *_top_tile);
			Tile* getTileBelow();
			Tile* getTileLeft();
			Tile* getTileAbove();
			Tile* getTileRight();
			bool sameAs(Tile *_tile);
			void moveVertical();
		private:
			void setTileLeft(Tile *_tile);
			void setTileAbove(Tile *_tile);

			SDL_Surface **sprite_data, *display;
			SDL_Rect *position;
			int sprite_frame, anim_dir, type;
			bool visible, animating, marked;
			Tile *tile_left, *tile_right, *tile_above, *tile_below;
 	};
}

#endif // TILE_H_INCLUDED

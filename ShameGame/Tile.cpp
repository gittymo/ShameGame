/*	Tile.cpp
 *	Tiles objects represent the tiles the player must remove from the board.
 *	(C)2009 Morgan Evans */
#include "Tile.h"
#include <iostream>

using std::cout;
using std::endl;

namespace UK_CO_MEVANSPN_SHAMEGAME {
	Tile::Tile(int _type, SDL_Surface* _display, SDL_Surface** _sprite_data, SDL_Rect *_position) {
		sprite_data = _sprite_data;
		sprite_frame = rand() % 3;
		anim_dir = 1;
		display = _display;
		position = _position;
		tile_left = tile_right = NULL;
		tile_above = tile_below = NULL;
		visible = true;
		type = _type;
		animating = false;
		marked = false;
	}

	Tile::~Tile() {
		sprite_data = NULL;
		sprite_frame = -1;
		anim_dir = 0;
		display = NULL;
		position = NULL;
		tile_left = tile_right = NULL;
		tile_above = tile_below = NULL;
		visible = false;
		animating = false;
	}

	bool Tile::isVisible() {
		return visible;
	}

	void Tile::destroy() {
		visible = false;
		marked = true;
		if (sameAs(tile_left) && position->x - tile_left->position->x == 32) {
			tile_left->destroy();
		}
		if (sameAs(tile_right) && tile_right->position->x - position->x == 32) {
			tile_right->destroy();
		}
		if (sameAs(tile_above)) {
			tile_above->destroy();
		}
		if (sameAs(tile_below)) {
			tile_below->destroy();
		}
	}

	void Tile::setTileRight(Tile *_tile, Tile *_left_tile) {
		tile_right = _tile;
		if (_tile != NULL) _tile->setTileLeft(_left_tile);
	}

	void Tile::setTileBelow(Tile *_tile, Tile *_top_tile) {
		tile_below = _tile;
		if (_tile != NULL) _tile->setTileAbove(_top_tile);
	}

	void Tile::setTileLeft(Tile *_tile) {
		tile_left = _tile;
	}

	void Tile::setTileAbove(Tile *_tile) {
		tile_above = _tile;
	}

	SDL_Rect* Tile::getPosition() {
		return position;
	}

	SDL_Surface* Tile::getSprite() {
		const int last_frame = sprite_frame;
		sprite_frame = (sprite_frame + anim_dir);
		if (sprite_frame == 3 || sprite_frame == 0) anim_dir = -anim_dir;
		return sprite_data[last_frame];
	}

	Tile* Tile::getTileBelow() {
		return tile_below;
	}

	Tile* Tile::getTileLeft() {
		return tile_left;
	}

	Tile* Tile::getTileAbove() {
		return tile_above;
	}

	Tile* Tile::getTileRight() {
		return tile_right;
	}

	bool Tile::sameAs(Tile *_tile) {
		if (_tile != NULL && _tile->visible && !_tile->marked) {
			if (type == _tile->type) return true;
		}
		return false;
	}

	void Tile::moveVertical() {
		int bottom_line = (tile_below != NULL) ? (tile_below->visible) ?
			tile_below->position->y : (tile_below->tile_below != NULL) ?
			tile_below->tile_below->position->y : display->h - 16 : display->h - 16;
		if (position->y + position->h < bottom_line) {
			position->y += 8;
			animating = true;
		} else {
			if (animating) {
				if (tile_below != NULL) {
					if (tile_below->visible) {
						tile_below->tile_above = this;
						if (tile_below->tile_left != NULL &&
							tile_below->tile_left->tile_above != NULL) {
							tile_below->tile_left->tile_above->tile_right = this;
							this->tile_left = tile_below->tile_left->tile_above;
						}
						if (tile_below->tile_right != NULL &&
							tile_below->tile_right->tile_above != NULL) {
							tile_below->tile_right->tile_above->tile_left = this;
							this->tile_right = tile_below->tile_right->tile_above;
						}
					} else {
						tile_below = tile_below->tile_below;
					}
				}
				animating = false;
			}
		}
	}
 }

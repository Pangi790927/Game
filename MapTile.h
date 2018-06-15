#ifndef MAP_TILE_H
#define MAP_TILE_H

#include "Unit.h"

class MapTile {
public:
	bool empty = true;
	std::shared_ptr<Unit> unitPtr;

	bool aquire() {
		empty = false;
	}

	bool canAquire() const {
		return empty;
	}

	void release() {
		empty = true;
	}
};

#endif
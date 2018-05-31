#ifndef MAP_TILE_H
#define MAP_TILE_H

#include "SubTile.h"

class MapTile {
public:
	std::vector<std::vector<SubTile>> subtiles;

	MapTile ()
	: subtiles(3, std::vector<SubTile>(3)) {}

	// void render (int i, int j, DrawContext& drawContext,
	// 		ShaderProgram &shader)
	// {
	// 	for (int i = 0; i < 3; i++)
	// 		for (int j = 0; j < 3; j++) {
	// 			DrawContext newContext = DrawContext(
	// 				drawContext.proj,
	// 				drawContext.view,
	// 				Math::translation<float>(
	// 						i,
	// 						0,
	// 						j
	// 					) 
	// 					* drawContext.world
	// 			);
	// 			subtiles[i][j].render(newContext, shader);
	// 		}
	// }
};

#endif
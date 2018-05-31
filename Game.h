#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "GameMap.h"
#include "Unit.h"
#include "TankUnit.h"

class Game {
public:
	Player player;
	GameMap map;
	std::vector<std::shared_ptr<Unit>> units;
	ShaderProgram unitShader;

	Game (int mapSize) : map(mapSize) {
		spawnUnit(1, 1, 3, 3);
		spawnUnit(1, 1, 3, 4);
		spawnUnit(1, 1, 3, 5);
		spawnUnit(1, 1, 6, 3);
		spawnUnit(1, 1, 7, 3);
	}

	void spawnUnit (int player, int type, int i, int j) {
		if (!map(i, j).unitPtr) {
			map(i, j).unitPtr = std::shared_ptr<Unit>(new TankUnit(player, type));
			map(i, j).unitPtr->pos = Math::Point3f(i, 0, j) * map.scale;
			units.push_back(map(i, j).unitPtr);
		}
	}

	void initRender() {
		map.initRender();
		unitShader = ShaderProgram({
			{GL_VERTEX_SHADER, "Shaders/mapShader.vert"},
			{GL_FRAGMENT_SHADER, "Shaders/mapShader.frag"}
		});
	}

	void render (DrawContext& drawContext) {
		map.render(drawContext);
		for (auto&& unitPtr : units) {
			if (unitPtr)
				unitPtr->render(drawContext, unitShader);
		}
	}
};

#endif
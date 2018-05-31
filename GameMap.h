#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "MapTile.h"


class GameMap {
public:
	using MapVertexType = Vertex<
		Math::Point3f,	VertexPosition,
		Math::Point3f,	VertexNormal,
		Math::Point4f,	VertexColor,
		Math::Point2f,	VertexTexCoord
	>;

	int size;
	std::vector<std::vector<MapTile>> tiles;

	Mesh<MapVertexType> mMap;
	DeprecatedVBOMeshDraw gMap;
	ShaderProgram shader;

	float scale;

	GameMap (int size, float scale = 50)
	: tiles(size, std::vector<MapTile>(size)), size(size), scale(scale) {}

	SubTile& operator () (int x, int y) {
		return tiles[x / 3][y / 3].subtiles[x % 3][y % 3];
	}

	const SubTile& operator () (int x, int y) const {
		return tiles[x / 3][y / 3].subtiles[x % 3][y % 3];
	}

	std::vector<MapTile>& operator [] (int i) {
		return tiles[i];
	}

	const std::vector<MapTile>& operator [] (int i) const {
		return tiles[i];
	}

	void initRender() {
		using namespace Math;
		shader = ShaderProgram({
			{GL_VERTEX_SHADER, "Shaders/mapShader.vert"},
			{GL_FRAGMENT_SHADER, "Shaders/mapShader.frag"}
		});
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
						Util::addSquareW(mMap, 1.5,
								Vec4f((i + j) % 2, 0, 1, 1),
								translation<float>(i * 3 + 1, 0, j * 3 + 1) *
								rot4<float>(-90, World::west) *
								scale);

		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				for (int k = 0; k < 3; k++)
					for (int o = 0; o < 3; o++)
						Util::addSquareW(mMap, 0.48,
								Vec4f((i + j) % 2, (k + o) % 2, 0, 1),
								translation<float>(i * 3 + k, 0, j * 3 + o) *
								rot4<float>(-90, World::west) *
								scale);
		gMap = DeprecatedVBOMeshDraw(mMap);
	}

	void render (DrawContext& drawContext) {
		using namespace Math;
		shader.useProgram();
		shader.setMatrix("projectionMatrix", drawContext.proj);
		shader.setMatrix("viewMatrix", drawContext.view);
		shader.setMatrix("worldMatrix", drawContext.world);
		gMap.draw(shader);
		// for (int i = 0; i < tiles.size(); i++)
		// 	for (int j = 0; j < tiles[i].size(); j++) {
		// 		DrawContext newContext = DrawContext(
		// 			drawContext.proj,
		// 			drawContext.view,
		// 			Math::translation<float>(
		// 					i * 3 * scale,
		// 					0,
		// 					j * 3 * scale
		// 				) 
		// 				* drawContext.world
		// 		);
		// 		tiles[i][j].render(i, j, newContext, shader);
		// 	}
	}
};

#endif
#ifndef GAME_H
#define GAME_H

#include "Player.h"
#include "GameMap.h"
#include "Unit.h"
#include "TankUnit.h"
#include "GameCamera.h"
#include "GameUtil.h"

class Game {
public:
	constexpr const static float SELECT_THRESHOLD = 0.01;

	Player player;
	GameMap map;
	std::vector<std::shared_ptr<Unit>> units;
	std::vector<std::shared_ptr<Unit>> selectedUnits;
	ShaderProgram unitShader;
	GameCamera camera;
	Math::Point3f selection;

	Game (int mapWidth, int mapHeight) : map(mapWidth, mapHeight) {
		spawnUnit(1, 1, 3, 3);
		spawnUnit(1, 1, 3, 4);
		spawnUnit(1, 1, 3, 5);
		spawnUnit(1, 1, 6, 3);
		spawnUnit(1, 1, 7, 3);
		camera -= World::up * 100;
		camera.horizRot += 120;
	}

	void spawnUnit (int player, int type, int i, int j) {
		if (map.canAquire(i, j)) {
			map.aquire(i, j);
			map(i, j).unitPtr = std::shared_ptr<Unit>(new TankUnit(player, type));
			map(i, j).unitPtr->pos = Math::Point3f(i, 0, j) * map.scale;
			units.push_back(map(i, j).unitPtr);
			std::cout << "spawnUnit" << std::endl;
		}
	}

	void getInput (OpenglWindow& window, DrawContext& drawContext) {
		DrawContext newContext = drawContext;
		newContext.view = camera.getTransform();

		camera.getInput(window);
		
		static Math::Point2f lastPos;
		static bool wasRmb = false;
		if (window.mouse.getRmb() && !wasRmb) {
			lastPos = Util::getMousePos(window.mouse,
					window.width, window.height);
			wasRmb = true;
		}
		if (!window.mouse.getRmb()) {
			if (wasRmb) {
				Math::Point2f currentPos = Util::getMousePos(window.mouse,
						window.width, window.height); 
				if ((lastPos - currentPos).norm2() < SELECT_THRESHOLD)
					onRightClick(lastPos, newContext);
				else
					onMakeSelection(lastPos, currentPos, newContext);
			}
			wasRmb = false;
		}
	}

	void onMakeSelection (Math::Point2f a, Math::Point2f b,
			DrawContext& drawContext)
	{
		selectedUnits.clear();
		for (auto&& unit : units) {
			Math::Point2f result = Util::toScreen(
				unit->pos,
				drawContext.proj,
				drawContext.view
			);
			if (Util::inSquare(result, a, b))
				selectedUnits.push_back(unit);
		}
	}

	void onRightClick (Math::Point2f pos,
			DrawContext& drawContext)
	{
		using namespace Math;
		auto ray = Util::getMouseRay(
			drawContext.view,
			pos,
			drawContext.yFov,
			drawContext.zNear,
			drawContext.aspect
		);
		auto intersect = Util::planeIntersect(
			ray,
			Util::Plane(
				Point3f(),
				World::up
			)
		);
		if (intersect.first) {
			selection = intersect.second;
			spawnUnit(1, 1,
				map.getTilePos(Point2f(selection.x, selection.z)).x,
				map.getTilePos(Point2f(selection.x, selection.z)).y
			);
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
		DrawContext newContext = drawContext;
		newContext.view = camera.getTransform();
		map.render(newContext);
		for (auto&& unitPtr : units) {
			unitPtr->render(newContext, unitShader);
		}
		unitShader.useProgram();
		unitShader.setMatrix("projectionMatrix", newContext.proj);
		unitShader.setMatrix("viewMatrix", newContext.view);
		unitShader.setMatrix("worldMatrix", newContext.world);
		for (auto&& selectUnit : selectedUnits) {
			Util::drawLine(selectUnit->pos, selectUnit->pos + World::up * 10);
		}
		Util::drawLine(selection, selection + World::up * 10);
	}
};

#endif
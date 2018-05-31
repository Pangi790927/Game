#ifndef UNIT_H
#define UNIT_H

#include "ShaderProgram.h"

class Unit {
public:
	int player = 0;
	int type = 0;

	Math::Point3f pos;
	Math::Point3f dir;

	Unit (int player = 0, int type = 0)
	: player(player), type(type)
	{}

	virtual void render (DrawContext& drawContext,
			ShaderProgram &defaultShader)
	{
		defaultShader.useProgram();
		defaultShader.setMatrix("projectionMatrix", drawContext.proj);
		defaultShader.setMatrix("viewMatrix", drawContext.view);
		defaultShader.setMatrix("worldMatrix", 
				Math::translation<float>(pos) *
				drawContext.world);
		Util::drawLine(
			Math::Point3f(0, 0, 0),
			Math::Point3f(0, 10, 0),
			Math::Point4f(
				player % 2,
				(player / 2) % 2,
				(player / 4) % 2,
				1
			)
		);
	}
};

#endif
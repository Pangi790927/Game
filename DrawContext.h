#ifndef DRAW_CONTEXT_H
#define DRAW_CONTEXT_H

class DrawContext {
public:
	Math::Mat4f proj = Math::identity<4, float>();
	Math::Mat4f view = Math::identity<4, float>();
	Math::Mat4f world = Math::identity<4, float>();

	DrawContext(
			const Math::Mat4f& proj = Math::identity<4, float>(),
			const Math::Mat4f& view = Math::identity<4, float>(),
			const Math::Mat4f& world = Math::identity<4, float>())
	: proj(proj), view(view), world(world)
	{}
};

#endif
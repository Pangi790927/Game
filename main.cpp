#include "OpenglWindow.h"
#include "ShaderProgram.h"

// names for the vertex
struct VertexTexID		{ constexpr const static char name[] = "texId"; };
struct VertexTexCoord	{ constexpr const static char name[] = "texCoord"; };
struct VertexNormal		{ constexpr const static char name[] = "normal"; };
struct VertexPosition	{ constexpr const static char name[] = "pos"; };
struct VertexColor		{ constexpr const static char name[] = "color"; };

#include "Mesh.h"
#include "MeshTools.h"
#include "OBJLoader.h"
#include "FixedFunctionMeshDraw.h"
#include "DeprecatedVBOMeshDraw.h"
#include "DynamicVBOMeshDraw.h"

#include "Camera.h"
#include "GameUtil.h"
#include "DrawContext.h"
#include "Game.h"

using VertexType = Vertex<
	Math::Point2f,	VertexTexCoord,
	Math::Point3f,	VertexNormal,
	Math::Point3f,	VertexPosition,
	int,			VertexTexID
>;

using NormalVertexType = Vertex<
	Math::Point3f,	VertexPosition,
	Math::Point3f,	VertexNormal,
	Math::Point4f,	VertexColor,
	Math::Point2f,	VertexTexCoord
>;

int main (int argc, char const *argv[])
{
	using namespace Math;
	/// Window
	OpenglWindow newWindow(500, 800, "Shaders Example");

	/// Shader
	ShaderProgram shader = ShaderProgram({
		{GL_VERTEX_SHADER, "Shaders/textureShader.vert"},
		{GL_FRAGMENT_SHADER, "Shaders/textureShader.frag"}
	});

	DrawContext drawContext;
	newWindow.setResize([&] (int x, int y, int w, int h){
		newWindow.focus();
		glViewport(0, 0, w, h);
		drawContext.proj = projection<float>(55.0,
				newWindow.width / (float)newWindow.height, 0.1, 10000);
		shader.setMatrix("projectionMatrix", drawContext.proj);
	});

	shader.setMatrix("viewMatrix", identity<4, float>());
	shader.setMatrix("worldMatrix", translation<float>(0, -10, -50));

	/// Mesh
	// Mesh mesh = OBJLoader<VertexType>().loadMesh("Obj/TankNoTex/", "T-90.obj");
	// Mesh mesh = OBJLoader<VertexType>().loadMesh("Obj/Tree/", "Lowpoly_tree_sample.obj");
	// Mesh mesh = OBJLoader<VertexType>().loadMesh("Obj/Box/", "box_obj.obj");
	// Mesh mesh = OBJLoader<VertexType>().loadMesh("Obj/Town/OBJ/", "City.obj");
	
	// DeprecatedVBOMeshDraw gMesh(mesh);
	// DynamicVBOMeshDraw gdMesh(mesh);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glLineWidth(6);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glClearColor(1, 1, 1, 1);
	// glEnable(GL_LINE_SMOOTH);
	// glEnable(GL_POLYGON_SMOOTH);
	// glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	// glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Camera testCamera;
	testCamera.rotSpeed *= 3;

	Mesh<NormalVertexType> pot;
	Mesh<NormalVertexType> tile1;
	Mesh<NormalVertexType> tile2;
	Mesh<NormalVertexType> sphere;

	Util::addCube(pot, 3, Vec4f(0.44, 0.9, 0.1, 1));
	Util::addSquare(tile1, 45, Vec4f(0.34, 0.56, 0.1, 1),
			rot4<float>(-90, World::west));
	Util::addSquare(tile2, 45, Vec4f(0.74, 0.23, 0.56, 1),
			rot4<float>(-90, World::west));
	Util::addSphere(sphere, 0.01, 30, Vec4f(0.12, 0.23, 0.42, 1));

	DeprecatedVBOMeshDraw gPot(pot);
	DeprecatedVBOMeshDraw gTile1(tile1);
	DeprecatedVBOMeshDraw gTile2(tile2);
	DeprecatedVBOMeshDraw gSphere(sphere);

	Game newGame(64);
	newGame.initRender();

	while (newWindow.active) {
		if (newWindow.handleInput()) {
			if (newWindow.keyboard.getKeyState(newWindow.keyboard.ESC))
				newWindow.requestClose();
		}
		float speed = 1.0f;
		if (newWindow.keyboard.getKeyState(newWindow.keyboard.L_SHIFT))
			speed = 10;
		if (newWindow.keyboard.getStateNoCase('W'))
			testCamera.moveForward(speed);
		if (newWindow.keyboard.getStateNoCase('S'))
			testCamera.moveBackward(speed);
		if (newWindow.keyboard.getStateNoCase('A'))
			testCamera.moveLeft(speed);
		if (newWindow.keyboard.getStateNoCase('D'))
			testCamera.moveRight(speed);
		if (newWindow.keyboard.getStateNoCase('Q'))
			testCamera.moveDown(speed);
		if (newWindow.keyboard.getStateNoCase('E'))
			testCamera.moveUp(speed);

		if (newWindow.mouse.lmb) {
			testCamera.rotate(
				newWindow.mouse.x - newWindow.mouse.lastX,
				newWindow.mouse.y - newWindow.mouse.lastY,
				newWindow.width,
				newWindow.height
			);
			newWindow.mouse.update();
		}

		shader.setMatrix("projectionMatrix", drawContext.proj);
		shader.setMatrix("viewMatrix",
				drawContext.view = testCamera.getTransform());
		
		newWindow.focus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Point2f mousePos = Util::getMousePos(newWindow.mouse,
				newWindow.width, newWindow.height);
		auto ray = Util::getMouseRay(testCamera.getTransform(), mousePos, 55.0, 0.1,
				float(newWindow.width) / newWindow.height);
		auto intersect = Util::planeIntersect(ray, Util::Plane(
			Point3f(), World::up));
		if (intersect.first) {
			shader.setMatrix("worldMatrix", identity<4, float>());
			Util::drawLine(intersect.second + Point3f(0, 10, 0), intersect.second);
		}


		static Point3f start = 0;
		static Point2f mouseStart = 0;
		if (newWindow.mouse.getOnceRmb()) {
			start = intersect.second;
			mouseStart = mousePos;
		}
		// if (newWindow.mouse.getRmb()) {
		// 	auto end = intersect.second;

		// 	Util::drawLine(start, Point3f(start.x, 0, end.z));
		// 	Util::drawLine(Point3f(start.x, 0, end.z), end);
		// 	Util::drawLine(end, Point3f(end.x, 0, start.z));
		// 	Util::drawLine(Point3f(end.x, 0, start.z), start);
		// }

		// for (int i = 0; i < 100; i++) {
		// 	auto posTransform = translation<float>(
		// 			(i / 10) * 100 * World::north + (i % 10) * 100 * World::west);
			
		// 	shader.setMatrix("worldMatrix", posTransform);
		// 	if ((i / 10) % 2 && (i % 10) % 2)
		// 		gTile1.draw(shader);
		// 	else
		// 		gTile2.draw(shader);
		// 	// gPot.draw(shader);
		// }
		glDisable(GL_BLEND);
		glLineWidth(1);
		newGame.render(drawContext);
		glLineWidth(4);
		glEnable(GL_BLEND);

		glDisable(GL_DEPTH_TEST);
		// for (int i = 0; i < 100; i++) {
		// 	auto posTransform = translation<float>(
		// 			(i / 10) * 100 * World::north + (i % 10) * 100 * World::west);
			
		// 	shader.setMatrix("worldMatrix", posTransform * scale4<float>(3, 3, 3));
		// 	gdMesh.draw(shader);	
		// }

		// menu stuff 
		shader.setMatrix("projectionMatrix", identity<4, float>());
		shader.setMatrix("viewMatrix", identity<4, float>());
		
		// for (int i = 0; i < 100; i++) {
		// 	Mat4f view = testCamera.getTransform();

		// 	Point2f result = Util::toScreen(
		// 		(i / 10) * 100 * World::north + (i % 10) * 100 * World::west,
		// 		drawContext.proj,
		// 		view
		// 	);

		// 	shader.setMatrix("worldMatrix", translation<float>(Point3f(result)));
		// 	gSphere.draw(shader);
		// }
		
		if (newWindow.mouse.getRmb()) {
			auto end = mousePos;

			auto red = Point4f(1, 0, 0, 1);
			shader.setMatrix("worldMatrix", identity<4, float>());
			
			Util::drawLine(mouseStart, Point3f(mouseStart.x, mousePos.y, 0), red);
			Util::drawLine(Point3f(mouseStart.x, mousePos.y, 0), mousePos, red);
			Util::drawLine(mousePos, Point3f(mousePos.x, mouseStart.y, 0), red);
			Util::drawLine(Point3f(mousePos.x, mouseStart.y, 0), mouseStart, red);
		}
		glEnable(GL_DEPTH_TEST);
		newWindow.swapBuffers();
	}
	return 0;
}
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
	/// Window
	OpenglWindow newWindow(500, 500, "Shaders Example");

	/// Shader
	ShaderProgram shader = ShaderProgram({
		{GL_VERTEX_SHADER, "Shaders/textureShader.vert"},
		{GL_FRAGMENT_SHADER, "Shaders/textureShader.frag"}
	});

	newWindow.setResize([&] (int x, int y, int w, int h){
		newWindow.focus();
		glViewport(0, 0, w, h);
		shader.setMatrix("projectionMatrix", Math::projection<float>(
				55.0, newWindow.width / (float)newWindow.height, 0.1, 1000));
	});

	shader.setMatrix("viewMatrix", Math::identity<4, float>());
	shader.setMatrix("worldMatrix", Math::translation<float>(0, -10, -50));

	/// Mesh
	Mesh mesh = OBJLoader<VertexType>().loadMesh("Obj/", "Lowpoly_tree_sample.obj");
	DynamicVBOMeshDraw gdMesh(mesh);

	glEnable(GL_TEXTURE_2D);
	// glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glClearColor(1, 1, 1, 1);

	Mesh<NormalVertexType> myMesh;
	Camera testCamera;

	// Util::addCube(myMesh, 10);
	Util::addSphere(myMesh, 6, 30, Math::Vec4f(0.44, 0.9, 0.1, 1));
	DeprecatedVBOMeshDraw gMesh(myMesh);

	while (newWindow.active) {
		if (newWindow.handleInput()) {
			if (newWindow.keyboard.getKeyState(newWindow.keyboard.ESC))
				newWindow.requestClose();
		}
		if (newWindow.keyboard.getStateNoCase('W'))
			testCamera.moveForward();
		if (newWindow.keyboard.getStateNoCase('S'))
			testCamera.moveBackward();
		if (newWindow.keyboard.getStateNoCase('A'))
			testCamera.moveLeft();
		if (newWindow.keyboard.getStateNoCase('D'))
			testCamera.moveRight();
		if (newWindow.keyboard.getStateNoCase('Q'))
			testCamera.moveDown();
		if (newWindow.keyboard.getStateNoCase('E'))
			testCamera.moveUp();

		if (newWindow.mouse.lmb)
			testCamera.rotate(
				newWindow.mouse.x - newWindow.mouse.lastX,
				newWindow.mouse.y - newWindow.mouse.lastY,
				newWindow.width,
				newWindow.height
			);

		shader.setMatrix("viewMatrix", testCamera.getTransform());
		
		newWindow.focus();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (int i = 0; i < 100; i++) {
			shader.setMatrix("worldMatrix", Math::translation<float>(
					(i / 10) * 100 * World::north + (i % 10) * 100 * World::west));
			FixedFunctionMeshDraw().draw(mesh, shader);
			gMesh.draw(shader);
		}
		// gMesh.draw(shader);
		// gdMesh.draw(shader);
		newWindow.swapBuffers();
	}
	return 0;
}
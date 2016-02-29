#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "gearoenix-main.hpp"
#include "gearoenix-log.hpp"
#include "gearoenix-scene.hpp"
#include "gearoenix-shortcuts.hpp"

std::map<GLFWwindow *, gearoenix::ui::MainWindow *> gearoenix::ui::MainWindow::winMap;

gearoenix::ui::MainWindow *gearoenix::ui::MainWindow::instance;

void gearoenix::ui::MainWindow::onErrorEvent(int errorNumber, const char* errorDescription)
{
	gce("GLFW3 Error number: " << errorNumber << "   Description: " << errorDescription);
}

void gearoenix::ui::MainWindow::onCursorMoveEvent(double x, double y)
{
	if (cameraMove)
	{
		scene->moveCamera(float(x - lastX) * 0.001f, float(y - lastY) * 0.001f);
	}
	lastX = x;
	lastY = y;
}

void gearoenix::ui::MainWindow::onCursorMoveEvent(GLFWwindow* window, double x, double y)
{
	winMap[window]->onCursorMoveEvent(x, y);
}

void gearoenix::ui::MainWindow::onScroll(double x, double y)
{
	(void)x;
	scene->moveForward(float(y));
}

void gearoenix::ui::MainWindow::onScroll(GLFWwindow* window, double x, double y)
{
	winMap[window]->onScroll(x, y);
}

void gearoenix::ui::MainWindow::onMouseKeyEvent(int button, int action, int mods)
{
	(void)mods;
	bool pressed = action == GLFW_PRESS;
	switch (button)
	{
	case GLFW_MOUSE_BUTTON_3:
		cameraMove = pressed;
		break;
	case GLFW_MOUSE_BUTTON_2:
	{
		break;
	}
	default:
		break;
	}
}

void gearoenix::ui::MainWindow::onMouseKeyEvent(GLFWwindow* window, int button, int action, int mods)
{
	winMap[window]->onMouseKeyEvent(button, action, mods);
}

void gearoenix::ui::MainWindow::onKeyEvent(int key, int scanCode, int action, int mods)
{}

void gearoenix::ui::MainWindow::onKeyEvent(GLFWwindow* window, int key, int scanCode, int action, int mods)
{}

void gearoenix::ui::MainWindow::onChangeSizeEvent(int width, int height)
{
	winWidth = width;
	winHeight = height;
	scene->setViewport(float(width), float(height));
	//addGlWork([this]() {
	glViewport(0, 0, this->winWidth, this->winHeight);
	//});
}

void gearoenix::ui::MainWindow::onChangeSizeEvent(GLFWwindow* window, int width, int height)
{
	winMap[window]->onChangeSizeEvent(width, height);
}

gearoenix::ui::MainWindow::MainWindow(const char *const caption, const int &width, const int &height, const bool &fullscreen)
{
	instance = this;
	if (!glfwInit())
	{
		gce("Error in intializing GLFW.");
	}
	glfwSetErrorCallback(MainWindow::onErrorEvent);
	winWidth = width;
	winHeight = height;
	if (!fullscreen) window = glfwCreateWindow(width, height, caption, NULL, NULL);
	if (window == 0)
	{
		gce("Error in creating GLFW window.");
	}
	glfwGetCursorPos(window, &lastX, &lastY);
	winMap[window] = this;
	glfwMakeContextCurrent(window);
	if (glewInit() != GLEW_OK)
	{
		gce("Error in intializing GLEW.");
	}
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glViewport(0, 0, width, height);
	glScissor(0, 0, width, height);
	glfwSetKeyCallback(window, MainWindow::onKeyEvent);
	glfwSetMouseButtonCallback(window, MainWindow::onMouseKeyEvent);
	glfwSetCursorPosCallback(window, MainWindow::onCursorMoveEvent);
	glfwSetScrollCallback(window, MainWindow::onScroll);
	glfwSetWindowSizeCallback(window, MainWindow::onChangeSizeEvent);
	scene = new render::Scene("origin", 9, 5);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

gearoenix::ui::MainWindow::~MainWindow()
{
	if (scene != nullptr)
		delete scene;
	if (window != nullptr)
		glfwDestroyWindow(window);
}

void gearoenix::ui::MainWindow::show()
{
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		scene->render();
		glFinish();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}

#include <iostream>

int main(const int argc, const char **argv)
{
	gearoenix::core::Logger::init();
	gearoenix::ui::MainWindow w("DMonGPU (SOM test)");
	w.show();
	gearoenix::core::Logger::err.close();
	return 0;
}

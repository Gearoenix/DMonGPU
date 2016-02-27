#ifndef GEAROENIX_CORE_MAIN_HPP
#define GEAROENIX_CORE_MAIN_HPP
#include <map>
struct GLFWwindow;
namespace gearoenix
{
	namespace render
	{
		class Scene;
	}
	namespace ui
	{
		class MainWindow
		{
		private:
			double lastX, lastY;
			bool cameraMove = false;
			GLFWwindow* window = nullptr;
			render::Scene *scene = nullptr;
			static std::map<GLFWwindow *, MainWindow *> winMap;
			static void onErrorEvent(int errorNumber, const char* errorDescription);
			void onCursorMoveEvent(double x, double y);
			static void onCursorMoveEvent(GLFWwindow* window, double x, double y);
			void onScroll(double x, double y);
			static void onScroll(GLFWwindow* window, double x, double y);
			void onMouseKeyEvent(int button, int action, int mods);
			static void onMouseKeyEvent(GLFWwindow* window, int button, int action, int mods);
			void onKeyEvent(int key, int scanCode, int action, int mods);
			static void onKeyEvent(GLFWwindow* window, int key, int scanCode, int action, int mods);
			void onChangeSizeEvent(int width, int height);
			static void onChangeSizeEvent(GLFWwindow* window, int width, int height);
		public:
			int winWidth;
			int winHeight;
			static MainWindow *instance;
			MainWindow(const char *const caption, const int &width = 850, const int &height = 500, const bool &fullscreen = false);
			~MainWindow();
			void show();
		};
	}
}
#endif

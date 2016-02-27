#ifndef GEAROENIX_RENDER_SCENE_HPP
#define GEAROENIX_RENDER_SCENE_HPP
namespace gearoenix
{
	namespace math
	{
		class Camera;
	}
	namespace render
	{
		class Mesh;
		class Scene
		{
		private:
			math::Camera *camera;
			Mesh *mainImage;
			Mesh *coprImage;
		public:
			Scene(const char *const &fileAddress, const unsigned int &mapAspect, const unsigned int &numIter);
			void render();
			void moveCamera(const float &dx, const float &dy);
			void moveForward(const float &dis);
			void setViewport(const float &w, const float &h);
		};
	}
}
#endif

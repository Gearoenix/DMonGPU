#ifndef GEAROENIX_CORE_LOG_HPP
#define GEAROENIX_CORE_LOG_HPP
#include <fstream>
namespace gearoenix
{
	namespace core
	{
		class Logger
		{
		public:
			static std::ofstream err;
			static void init();
		};
	}
}
#endif
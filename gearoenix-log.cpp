#include "gearoenix-log.hpp"

std::ofstream gearoenix::core::Logger::err;

void gearoenix::core::Logger::init()
{
	err.open("log.txt");
}
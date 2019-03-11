#include <vector>
#include <string>

#include "sandbox/survival/organism.h"

namespace Survival
{
	std::vector<Obstacle>read_obstacles(const std::string& file_path);

	void generate_obstacles(const unsigned& num, const std::string& file_path);

	int run();
}

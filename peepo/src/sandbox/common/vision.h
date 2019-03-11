#ifndef VISION__H
#define VISION__H
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>



static std::vector<double> rotate_point(std::vector<double> target, const double& angle)
{
	double s = sin(angle);
	double c = cos(angle);
	// rotate point
	double xnew = target[0] * c + target[1] * s;
	double ynew = -target[0] * s + target[1] * c;
	return { xnew,ynew };
}


static bool collision(const std::vector<double>& observer, const std::vector<double>& target, const double& direction,
	const double& edge_1, const double& edge_2, const double& view_radius)
{
	//first calculate coordinates vector  target and observerin rotated axis (as to direction as referential)
	std::vector<double> target_ = { target[0] - observer[0], target[1] - observer[1] };
	target_ = rotate_point(target_, direction);
	//calculate angle vs the x-axis
	if (target_[0] == 0.0) { target_[0] += 0.00000001; }//safety in case atan infinite
	double angle = atan(target_[1] / target_[0]);
	double max_a = std::max(edge_1, edge_2);
	double min_a = std::min(edge_1, edge_2);
	if ((angle <= max_a) && (angle >= min_a))
	{
		return true;
	}
	return false;
}

#endif

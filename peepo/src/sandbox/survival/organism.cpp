#include "organism.h"


SurvivalPeepo::SurvivalPeepo(std::string name_, std::vector<double>& pos_, std::vector<Obstacle>& obstacles_, PPNetwork& pp_network_) :
	name(name_),
	pos(pos_),
	obstacles(obstacles_),
	gen_model(pp_network_, *this),
	rotation(0.f),
	health(0)
{
	motor[LEFT] = false;
	motor[RIGHT] = false;
	view["left"] = false;
	view["right"] = false;
	for (double angle = -30.0; angle < 30.0; angle += 10.0) {
		sectors.push_back({ angle*PI / 180., (angle + 10.0)*PI / 180.0 });
	}
}

void SurvivalPeepo::action(const std::string& node, const std::vector<double>& prediction)
{
	if (std::distance(prediction.begin(), std::max_element(prediction.begin(), prediction.end())) == 0)
	{
		motor[get_direction(node)] = false;
	}
	else {
		motor[get_direction(node)] = true;
	}
}

std::vector<double> SurvivalPeepo::observation(const std::string& node)
{
	if (node.find(VISION) != std::string::npos)
	{
		if (view[get_quadrant(node)])
		{
			return { 0.1, 0.9 };
		}
		else
		{
			return { 0.9, 0.1 };
		}
	}
	if (node.find(MOTOR) != std::string::npos)
	{
		if (motor[get_direction(node)])
		{
			return { 0.1, 0.9 };
		}
		else
		{
			return { 0.9, 0.1 };
		}
	}
	return { 0.5,0.5 };
}

void SurvivalPeepo::update()
{
	gen_model.process();

	pos[0] += std::cos(rotation) * PEEPO_SPEED;
	pos[1] += std::sin(rotation) * PEEPO_SPEED;
	if (motor[LEFT]) {
		rotation -= 2. / 180.*PI;
		if (rotation < 0.0) {
			rotation = 2.0*PI;//??  ??????
		}
	}
	if (motor[RIGHT]) {
		rotation += 2. / 180.*PI;
		if (rotation > 2.0*PI) {
			rotation = 0.0;
		}
	}

	calculate_obstacles();

	// Check collisions between the peepo and the screen
	if (pos[0] - SIZE_PEEPO <= 0.)
	{
		rotation = PI - rotation;
		pos[0] = SIZE_PEEPO + 0.1;
	}
	if (pos[0] + SIZE_PEEPO >= WIN_SIZE)
	{
		rotation = PI - rotation;
		pos[0] = WIN_SIZE - SIZE_PEEPO - 0.1;
	}
	if (pos[1] - SIZE_PEEPO <= 0.)
	{
		rotation = -rotation;
		pos[1] = SIZE_PEEPO + 0.1;
	}
	if (pos[1] + SIZE_PEEPO >= WIN_SIZE)
	{
		rotation = -rotation;
		pos[1] = WIN_SIZE - SIZE_PEEPO - 0.1;
	}

	edge_right = { pos[0] + PEEPO_RADIUS * std::cos(rotation + 30.0 / 180.0*PI),
				   pos[1] + PEEPO_RADIUS * std::sin(rotation + 30.0 / 180.0*PI) };
	edge_left = { pos[0] + PEEPO_RADIUS * std::cos(rotation - 30.0 / 180.0*PI),
				   pos[1] + PEEPO_RADIUS * std::sin(rotation - 30.0 / 180.0*PI) };
}

void SurvivalPeepo::calculate_obstacles()
{
	for (auto vw : view) {
		view[vw.first] = false;
	}

	int to_remove = -1;
	int count = 0;

	double closest_distance = 10000.0;
	relevant_sector.index = 0;
	for (int index = 0; index < sectors.size(); index++) {
		auto sector = sectors[index];
		double lower_edge = sector[0];
		double upper_edge = sector[1];
		//to_remove = -1;
		count = 0;
		for (auto obstacle : obstacles) {
			bool is_collision = collision(pos, { obstacle.x, obstacle.y },
				rotation, lower_edge, upper_edge, PEEPO_RADIUS);
			if (is_collision) {
				double distance = sqrt(pow((obstacle.y - pos[1]), 2.) + pow((obstacle.x - pos[0]), 2.));
				if (distance <= SIZE_PEEPO + SIZE_OBST) {
					health++;
					break;
				}
				if (distance > SIZE_PEEPO + SIZE_OBST && distance <= PEEPO_RADIUS) {
					closest_distance = distance;
					relevant_sector.index = index + 1;
					relevant_sector.xy = { obstacle.x,obstacle.y };
					relevant_sector.distance = closest_distance;
					break;
				}
			}
			count++;
		}
	}
	if ((relevant_sector.index == 4) || (relevant_sector.index == 5) || (relevant_sector.index == 6))
	{
		view["right"] = true;
	}
	else if ((relevant_sector.index == 1) || (relevant_sector.index == 2) || (relevant_sector.index == 3))
	{
		view["left"] = true;
	}
	std::string only_true = std::to_string(relevant_sector.index);

	double sight_angle = 0.0;
	if (only_true == "0") { sight_angle = rotation; }
	if (only_true == "1") { sight_angle = rotation - 25.0 / 180.0*PI; }
	if (only_true == "2") { sight_angle = rotation - 15.0 / 180.0*PI; }
	if (only_true == "3") { sight_angle = rotation - 5.0 / 180.0*PI; }
	if (only_true == "4") { sight_angle = rotation + 5.0 / 180.0*PI; }
	if (only_true == "5") { sight_angle = rotation + 15.0 / 180.0*PI; }
	if (only_true == "6") { sight_angle = rotation + 25.0 / 180.0*PI; }
	edge_middle = { pos[0] + relevant_sector.distance*std::cos(sight_angle),
					pos[1] + relevant_sector.distance*std::sin(sight_angle) };
}

std::string SurvivalPeepo::get_direction(const std::string& name)
{
	std::string direction;
	for (auto dir : { LEFT, RIGHT })
	{
		if (name.find(dir) != std::string::npos) { direction = dir; break; }
	}
	return direction;
}

std::string SurvivalPeepo::get_quadrant(const std::string& name)
{
	std::string quad = "0";
	std::vector<std::string> quadrants = { "left","right"};
	for (auto qd : quadrants)
	{
		if (name.find(qd) != std::string::npos) { quad = qd; break; }
	}
	return quad;
}
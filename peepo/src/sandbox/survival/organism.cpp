#include "organism.h"


SurvivalPeepo::SurvivalPeepo(const std::string& name_, std::vector<double>& pos_, std::vector<Obstacle>& obstacles_, PPNetwork& pp_network_) :
	Peepo(pp_network_),
	name(name_),
	pos(pos_),
	obstacles(obstacles_),
	rotation(0.f),
	health(1.0),
	motor({ {LEFT, false}, {RIGHT, false} }),
	view({ {"1", false}, {"2", false}, {"3", false}, {"4", false}, {"5", false}, {"6", false} })
{
	
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
	generative_model.process(*this);

	pos[0] += std::cos(rotation) * PEEPO_SPEED;
	pos[1] += std::sin(rotation) * PEEPO_SPEED;
	if (motor[LEFT]) {
		rotation -= 2. / 180.*PI;
	}
	if (motor[RIGHT]) {
		rotation += 2. / 180.*PI;
	}

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

	calculate_obstacles();
}

void SurvivalPeepo::calculate_obstacles()
{
	for (auto vw : view) {
		view[vw.first] = false;
	}

	closest_obstacle.distance = 10000.0;

	double lower_edge = sectors[0][0];
	double upper_edge = sectors[sectors.size() - 1][1];

	for (int i = obstacles.size() - 1; i >= 0; i--)
	{
		Obstacle& obstacle = obstacles[i];
		if (collision(pos, { obstacle.x, obstacle.y }, rotation, lower_edge, upper_edge, PEEPO_RADIUS))
		{
			double distance = sqrt(pow((obstacle.y - pos[1]), 2.) + pow((obstacle.x - pos[0]), 2.));

			if (distance <= SIZE_PEEPO + SIZE_OBST)
			{
				health++;
				obstacles.erase(obstacles.begin() + i);
			}

			if (distance > SIZE_PEEPO + SIZE_OBST && distance <= PEEPO_RADIUS)
			{
				if (distance <= closest_obstacle.distance)
				{
					closest_obstacle.distance = distance;
					closest_obstacle.xy = { obstacle.x,obstacle.y };
				}
			}
		}
	}

	if (closest_obstacle.distance < 10000.0)
	{
		for (int index = 0; index < sectors.size(); index++)
		{
			auto sector = sectors[index];
			double lower_edge = sector[0];
			double upper_edge = sector[1];

			if (collision(pos, { closest_obstacle.xy[0], closest_obstacle.xy[1] }, rotation, lower_edge, upper_edge, PEEPO_RADIUS))
			{
				std::string target_sector = std::to_string(index + 1);
				view[target_sector] = true;
			}
		}

	}
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
	std::vector<std::string> quadrants = { "1","2","3","4","5","6" };	
	for (auto qd : quadrants)
	{
		if (name.find(qd) != std::string::npos) { quad = qd; break; }
	}
	return quad;
}
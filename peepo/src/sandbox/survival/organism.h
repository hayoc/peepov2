#ifndef ORGANISM__H
#define ORGANISM__H

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "pp/pp_network.h"
#include "pp/peepo.h"
#include "pp/generative_model.h"
#include "sandbox/common/vision.h"

const std::string LEFT = "left";
const std::string RIGHT = "right";
const std::string VISION = "vision";
const std::string MOTOR = "motor";
const int WIN_SIZE = 900;
const double SIZE_OBST = 5.f;
const double SIZE_PEEPO = 5.f;
const double PEEPO_RADIUS = 50.0;
const double PI = 3.14159f;
const double PEEPO_SPEED = 0.2f;

struct Obstacle
{
	std::string id;
	double x;
	double y;
	sf::CircleShape shape;

	Obstacle(std::string id, double x, double y) :
		id(id), x(x), y(y)
	{
		shape.setRadius(SIZE_OBST - 3);
		shape.setOutlineThickness(3);
		shape.setOutlineColor(sf::Color::Black);
		shape.setFillColor(sf::Color::Red);
		shape.setOrigin(SIZE_OBST / 2, SIZE_OBST / 2);
	}
};

struct Ballz
{
	std::string id;
	double x;
	double y;
	sf::CircleShape shape;

	Ballz(std::string id, double x, double y) :
		id(id), x(x), y(y)
	{
		shape.setRadius(SIZE_OBST - 3);
		shape.setOutlineThickness(3);
		shape.setOutlineColor(sf::Color::Black);
		shape.setFillColor(sf::Color::Green);
		shape.setOrigin(SIZE_OBST / 2, SIZE_OBST / 2);
	}
};

class SurvivalPeepo : public Peepo
{
private:
	struct ClosestObstacle 
	{
		std::vector<double> xy;
		double distance;
	};
	
	ClosestObstacle closest_obstacle;
	std::vector<std::vector<double>> sectors;
	
public:
	SurvivalPeepo(const std::string&, std::vector<double>&, std::vector<Obstacle>&, PPNetwork&);

	GenerativeModel generative_model;

	std::string name;
	std::vector<double> pos;
	std::vector<Obstacle> obstacles;
	double rotation;
	double health;

	std::vector<double> edge_right;
	std::vector<double> edge_left;
	std::vector<double> edge_middle;

	std::map<std::string, bool> motor;
	std::map<std::string, bool> view;

	void action(const std::string&, const std::vector<double>&);
	std::vector<double> observation(const std::string&);
	static std::string get_quadrant(const std::string&);
	static std::string get_direction(const std::string&);

	void update();
	void calculate_obstacles();
};

#endif
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
const double PEEPO_SPEED = 0.1f;

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

class SurvivalPeepo : public Peepo
{
private:
	struct RelevantSector 
	{
		int index;
		std::vector<double> xy;
		double distance;
	};
	
	RelevantSector relevant_sector;
	std::vector<std::vector<double>> sectors;
	
	std::string name;
	PPNetwork& pp_network;
	GenerativeModel gen_model;

public:

	std::vector<double> pos;
	std::vector<Obstacle> obstacles;
	double rotation;
	unsigned health;

	std::vector<double> edge_right;
	std::vector<double> edge_left;
	std::vector<double> edge_middle;

	std::map<std::string, bool> motor;
	std::map<std::string, bool> view;

	SurvivalPeepo(std::string, std::vector<double>&, std::vector<Obstacle>&, PPNetwork&);
	
	void action(const std::string&, const std::vector<double>&);
	std::vector<double> observation(const std::string&);
	static std::string get_quadrant(const std::string&);
	static std::string get_direction(const std::string&);

	void update();
	void draw();
	void calculate_obstacles();
};

#endif
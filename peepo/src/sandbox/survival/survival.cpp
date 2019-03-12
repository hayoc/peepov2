#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>


#include "pp/pp_network.h"
#include "pp/generative_model.h"
#include "sandbox/survival/organism.h"
#include "sandbox/survival/survival.h"

using json = nlohmann::json;



namespace Survival
{
	std::vector<Obstacle>read_obstacles(const std::string& file_path)
	{
		std::vector<Obstacle> objects;

		std::ifstream ifs(file_path);
		json jzon{ json::parse(ifs) };

		for (json::iterator it = jzon.begin(); it != jzon.end(); ++it)
		{
			std::string id = it.key();
			double x = (*it)["x"].get<double>();
			double y = (*it)["y"].get<double>();
			Obstacle obst(id, x, y);
			obst.shape.setPosition(obst.x, obst.y);
			objects.push_back(obst);
		}
		return objects;
	}

	void generate_obstacles(const unsigned& num, const std::string& file_path)
	{
		json jzon;
		for (unsigned i = 0; i < num; i++)
		{
			std::string id = "obj_" + std::to_string(i);
			jzon[id]["x"] = double(std::rand() % WIN_SIZE);
			jzon[id]["y"] = double(std::rand() % WIN_SIZE);
		}
		std::ofstream ofs(file_path);
		std::string json_string{ jzon.dump() };
		ofs << json_string;
	}

	class World
	{
	private:
		bool graphical;
		sf::RenderWindow& window;

	public:

		World(const bool& graphical_, sf::RenderWindow& window_) :
			graphical(graphical_), window(window_)
		{

		}

		void world_loop(const unsigned& max_age, PPNetwork& pp_network, const std::string& obstacles_path)
		{
			bool isPlaying = true;
			std::vector<Obstacle> obstacles = read_obstacles(obstacles_path);
			std::vector<double> pos = { WIN_SIZE / 2, WIN_SIZE / 2 };
			double peepo_angle = (std::rand() % 180)*PI / 180.f; // to be changed later ??
			sf::CircleShape peepo_;
			peepo_.setRadius(SIZE_PEEPO - 3);
			peepo_.setOutlineThickness(3);
			peepo_.setOutlineColor(sf::Color::Black);
			peepo_.setFillColor(sf::Color::White);
			peepo_.setOrigin(SIZE_PEEPO / 2, SIZE_PEEPO / 2);
			peepo_.setPosition(pos[0], pos[1]);

			SurvivalPeepo peepo{ "Bob", graphical, pos, obstacles, pp_network };

			while (window.isOpen())
			{
				sf::Event event;
				while (window.pollEvent(event))
				{
					if ((event.type == sf::Event::Closed) ||
						((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
					{
						window.close();
						break;
					}

					if (event.type == sf::Event::Resized)
					{
						sf::View view;
						view.setSize(WIN_SIZE, WIN_SIZE);
						view.setCenter(WIN_SIZE / 2.f, WIN_SIZE / 2.f);
						window.setView(view);
					}
				}

				if (isPlaying)
				{
					peepo.update();
					obstacles = peepo.obstacles;
					pos = peepo.pos;
					peepo_angle = peepo.rotation;
					peepo_.setPosition(pos[0], pos[1]);
				}
				// Clear the window and draw the elements
				window.clear(sf::Color(150, 150, 150));
				if (isPlaying)
				{
					window.draw(peepo_);

					sf::Vertex edge_right[] =
					{
						sf::Vertex(sf::Vector2f(pos[0], pos[1])),
						sf::Vertex(sf::Vector2f(peepo.edge_right[0], peepo.edge_right[1]))
					};
					window.draw(edge_right, 2, sf::Lines);

					sf::Vertex edge_left[] =
					{
						sf::Vertex(sf::Vector2f(pos[0], pos[1])),
						sf::Vertex(sf::Vector2f(peepo.edge_left[0], peepo.edge_left[1]))
					};
					window.draw(edge_left, 2, sf::Lines);

					sf::Vertex edge_middle[] =
					{
						sf::Vertex(sf::Vector2f(pos[0], pos[1])),
						sf::Vertex(sf::Vector2f(peepo.edge_middle[0], peepo.edge_middle[1]))
					};
					window.draw(edge_middle, 2, sf::Lines);

					for (auto obst : obstacles) {
						window.draw(obst.shape);
					}
				}
				// Display things on screen
				window.display();
			}

		}
	};

	static void verification(const bool& graphical, PPNetwork& pp_network, const std::string& obstacles_path)
	{
		unsigned max_age = 2000;
		// Create the window of the application
		sf::RenderWindow window(sf::VideoMode(WIN_SIZE, WIN_SIZE, 32), "Survival",
			sf::Style::Titlebar | sf::Style::Default);
		window.setVerticalSyncEnabled(false);
		World world(graphical, window);
		world.world_loop(max_age, pp_network, obstacles_path);
		std::cin.get();
	}

	int run()
	{
		PPNetwork pp;
		std::ifstream ifs("data/survival_network.json");
		pp.from_file(ifs);

		generate_obstacles(400, "data/survival_obstacles.json");

		verification(true, pp, "data/survival_obstacles.json");

		return 0;
	}
}
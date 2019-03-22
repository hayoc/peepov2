#include "sandbox/cointoss/cointoss.h"
#include "sandbox/survival/survival.h"

int main()
{
	//return Cointoss::run();
	return Survival::run();
}

//#include <vector>
//#include <iostream>
//#include <fstream>
//
//#include "pp/pp_network.h"
//#include "utilities.h"
//#include "pp/pp_network.h"
//#include "evolution/genetic_algorithm.h"
//
//using json = nlohmann::json;
//
//
//struct Ass
//{
//	Ass();
//	Ass(std::vector<std::string> edges_);
//	Ass(const Ass&);
//
//	std::vector<std::string> edges;
//};
//
//Ass::Ass()
//{
//
//}
//
//Ass::Ass(std::vector<std::string> edges_) :
//	edges(edges_)
//{
//
//}
//
//Ass::Ass(const Ass& ass) :
//	edges(ass.edges)
//{
//
//}
//
//struct Balls
//{
//
//	Balls(Ass& ass);
//	Balls(const Balls&);
//
//	Ass& ass;
//};
//
//Balls::Balls(Ass& ass_) :
//	ass(ass_)
//{
//
//}
//
//Balls::Balls(const Balls& balls) :
//	ass(balls.ass)
//{
//
//}
//
//int main()
//{
//	std::vector<Balls> population;
//
//	std::vector<std::vector<std::string>> topologies;
//	std::vector<std::string> v1{"ok", "ok2", "ok3", "ok4"};
//	std::vector<std::string> v2{"ok", "ok2", "ok3"};
//	std::vector<std::string> v3{"ok", "ok2"};
//
//	topologies.push_back(v1);
//	topologies.push_back(v2);
//	topologies.push_back(v3);
//
//	for (std::vector<std::string> edges : topologies)
//	{
//		Ass ass{ edges };
//
//		std::cout << ass.edges.size() << std::endl;
//		Balls individual{ ass };
//		population.push_back(individual);
//
//
//		std::cout << "[";
//		for (Balls individual : population)
//		{
//			std::cout << individual.ass.edges.size() << ", ";
//		}
//		std::cout << "]" << std::endl;
//	}
//
//	std::cin.get();
//}
#include "sandbox/cointoss/cointoss.h"
#include "sandbox/survival/survival.h"

int main()
{
	//return Cointoss::run();
	return Survival::run();
}

//#include <vector>
//#include <iostream>
//#include <algorithm>
//
//struct Ballz
//{
//	Ballz();
//	Ballz(const Ballz&);
//	Ballz(std::string, double = 0.0);
//	Ballz& operator=(const Ballz&);
//
//	double x;
//	std::string y;
//};
//
//Ballz::Ballz()
//{
//
//}
//
//Ballz::Ballz(std::string y_, double x_) :
//	x(x_),
//	y(y_)
//{
//
//}
//
//Ballz::Ballz(const Ballz& ball) :
//	pp_network(individual.pp_network),
//	fitness(0.0),
//	mut_top(0.0),
//	mut_cpd(0.0)
//{
//
//}
//
//Ballz& Ballz::operator=(const Ballz& individual)
//{
//	pp_network = individual.pp_network;
//	fitness = individual.fitness;
//	mut_top = individual.mut_top;
//	mut_cpd = individual.mut_cpd;
//	return *this;
//}
//
//int main()
//{
//	std::vector<Ballz> balls;
//	balls.push_back(Ballz{ "ok2", 2.0 });
//	balls.push_back(Ballz{ "ok3", 48.2 });
//	balls.push_back(Ballz{ "ok4", 33.7 });
//	balls.push_back(Ballz{ "ok5", 4.4 });
//	balls.push_back(Ballz{  "ok6", 0.1 });
//
//	std::sort(balls.begin(), balls.end(), [](const Ballz& a, const Ballz& b)
//	{
//		return a.x > b.x;
//	});
//
//	for (auto& offspring : balls)
//	{
//		std::cout << "Offspring fitness: " << offspring.x << std::endl;
//	}
//
//	std::cin.get();
//
//}

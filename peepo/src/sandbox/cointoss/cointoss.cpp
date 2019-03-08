#include <iostream>

#include <nlohmann/json.hpp>

#include <pp/pp_network.h>
#include <pp/sensory_input.h>
#include <pp/generative_model.h>

using json = nlohmann::json;


class SensoryInputCointoss :public SensoryInput {
private:
	std::vector<unsigned>& coin_set;
	int index{ 0 };
public:
	SensoryInputCointoss(std::vector<unsigned>& coin_set_)
		: coin_set(coin_set_) {}

	void action(const std::string& node_name, const std::vector<double>& prediction)
	{

	}

	std::vector<double> observation(const std::string& node_name)
	{
		// 0 = heads, 1 = tails
		if (coin_set[index] == 0)
		{
			std::vector<double> v = { 0.99, 0.01 };
			index++;
			return v;
		}
		else
		{
			std::vector<double> v = { 0.01, 0.99 };
			index++;
			return v;
		}
	}
};

int main()
{
	PPNetwork pp;
	std::ifstream ifs("data/cointoss_network.json");
	pp.from_file(ifs);

	std::vector<unsigned> coin_set;
	for (unsigned i = 0; i < 100; i++)
	{
		if (i % 2 == 0)
		{
			coin_set.push_back(0);
		}
		else {
			coin_set.push_back(1);
		}
	}
	SensoryInputCointoss si{ coin_set };
	GenerativeModel gen_model{ pp, si };

	std::vector<double> errors;

	for (unsigned i = 0; i < coin_set.size(); i++)
	{
		double er = gen_model.process();
		errors.push_back(er);
		std::cout << er << std::endl;
	}

	std::cin.get();
	return 0;
}
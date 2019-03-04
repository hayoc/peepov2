#include <iostream>
#include <fstream>

#include <pp/pp_network.h>


int main()
{
	PPNetwork pp;

	std::ifstream ifs("data/survival_network.json");
	pp.from_file(ifs);

	std::map<std::string, unsigned> evidence;
	evidence["LEN_vision_1"] = 1;
	evidence["LEN_vision_2"] = 1;
	evidence["LEN_vision_3"] = 1;
	evidence["LEN_vision_4"] = 0;
	evidence["LEN_vision_5"] = 0;
	evidence["LEN_vision_6"] = 0;

	pp.to_bayesian_network();
	std::map<std::string, matrix<double>> inference = pp.do_inference(evidence);

	for (auto& entry : inference)
	{
		std::cout << entry.first << " = " << entry.second << std::endl;
	}



	std::cin.get();
	return 0;
}
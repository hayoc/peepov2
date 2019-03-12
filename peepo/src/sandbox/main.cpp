#include "sandbox/cointoss/cointoss.h"
#include "sandbox/survival/survival.h"

int main()
{
	//return Cointoss::run();
	return Survival::run();
}


//#include <iostream>
//#include <fstream>
//
//#include "pp/pp_network.h"
//#include "pp/generative_model.h"
//#include "sandbox/survival/organism.h"
//
//int main()
//{
//	PPNetwork pp;
//	std::ifstream ifs("data/survival_network.json");
//	pp.from_file(ifs);
//	std::vector<Obstacle> obstacles;
//	std::vector<double> pos = { WIN_SIZE / 2, WIN_SIZE / 2 };
//	SurvivalPeepo peepo{ "Bob", true, pos, obstacles, pp };
//
//	GenerativeModel gen_model{ pp, peepo };
//
//	std::vector<double> prediction = {0.82, 18};
//	std::vector<double> prediction_error = {-0.72, 0.72};
//
//	std::map<std::string, std::vector<double>> predictions = gen_model.predict();
//
//	for (auto& entry : predictions)
//	{
//		std::string node = entry.first;
//		if (pp.is_leaf(node))
//		{
//			std::vector<double> prediction = entry.second;
//			std::cout << "Prediction for node: " << node << " = [" << prediction[0] << ", " << prediction[1] << "]" << std::endl;
//		}
//	}
//
//	gen_model.hypothesis_update("LEN_vision_1", prediction_error, prediction);
//
//	std::map<std::string, std::vector<double>> predictions2 = gen_model.predict();
//
//	for (auto& entry : predictions2)
//	{
//		std::string node = entry.first;
//		if (pp.is_leaf(node))
//		{
//			std::vector<double> prediction = entry.second;
//			std::cout << "Prediction for node: " << node << " = [" << prediction[0] << ", " << prediction[1] << "]" << std::endl;
//		}
//	}
//	std::cout << "ballz" << std::endl;
//	std::cin.get();
//}


//#include <dlib/bayes_utils.h>
//#include <dlib/graph_utils.h>
//#include <dlib/graph.h>
//#include <dlib/directed_graph.h>
//#include <iostream>
//
//
//using namespace dlib;
//using namespace std;
//
//// ----------------------------------------------------------------------------------------
//
//int main()
//{
//	try
//	{
//		
//		using namespace bayes_node_utils;
//
//		directed_graph<bayes_node>::kernel_1a_c bn;
//
//		enum nodes
//		{
//			A = 0,
//			B = 1
//		};
//
//		bn.set_number_of_nodes(2);
//		bn.add_edge(A, B);
//
//		set_node_num_values(bn, A, 2);
//		set_node_num_values(bn, B, 2);
//
//		assignment parent_state;
//
//		set_node_probability(bn, A, 1, parent_state, 0.2);
//		set_node_probability(bn, A, 0, parent_state, 1 - 0.2);
//
//		parent_state.add(A, 1);
//		//set_node_probability(bn, B, 1, parent_state, 0.7);
//		set_node_probability(bn, B, 1, parent_state, 0.7);
//		set_node_probability(bn, B, 0, parent_state, 1 - 0.7);
//
//		parent_state[A] = 0;
//		set_node_probability(bn, B, 1, parent_state, 0.9);
//		set_node_probability(bn, B, 0, parent_state, 1 - 0.9);
//
//
//		//parent_state.clear();
//
//
//		typedef dlib::set<unsigned long>::compare_1b_c set_type;
//		typedef graph<set_type, set_type>::kernel_1a_c join_tree_type;
//		join_tree_type join_tree;
//
//		create_moral_graph(bn, join_tree);
//		create_join_tree(join_tree, join_tree);
//
//		set_node_value(bn, B, 1);
//		set_node_as_evidence(bn, B);
//
//		bayesian_network_join_tree solution_with_evidence(bn, join_tree);
//
//		cout << "Using the join tree algorithm:\n";
//		cout << "p(A=1 | C=1) = " << solution_with_evidence.probability(A)(1) << endl;
//		cout << "p(A=0 | C=1) = " << solution_with_evidence.probability(A)(0) << endl;
//		cout << "p(B=1 | C=1) = " << solution_with_evidence.probability(B)(1) << endl;
//		cout << "p(B=0 | C=1) = " << solution_with_evidence.probability(B)(0) << endl;
//		cout << "\n\n\n";
//
//		cin.get();
//	}
//	catch (std::exception& e)
//	{
//		cout << "exception thrown: " << endl;
//		cout << e.what() << endl;
//		cout << "hit enter to terminate" << endl;
//		cin.get();
//	}
//}
//
//
//
//
//
//

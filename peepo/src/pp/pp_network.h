#ifndef PP_NETWORK_H
#define PP_NETWORK_H

#include <string>
#include <vector>
#include <fstream>

#include <nlohmann/json.hpp>

#include <dlib/bayes_utils.h>
#include <dlib/graph_utils.h>
#include <dlib/graph.h>
#include <dlib/directed_graph.h>


using namespace dlib;
using namespace bayes_node_utils;

typedef dlib::set<unsigned long>::compare_1b_c set_type;
typedef graph<set_type, set_type>::kernel_1a_c join_tree_type;


class PPNetwork
{

private:

	//------------- MEMBERS ---------------------

	std::string identification;
	std::string description;
	std::string date;

	std::vector<std::string> root_nodes;
	std::vector<std::string> ext_nodes;
	std::vector<std::string> pro_nodes;

	std::vector<std::vector<std::string>> edges;
	nlohmann::json cpds;

	std::map<std::string, unsigned>  node_map;
	std::map<std::string, unsigned> card_map;
	std::map<std::string, std::vector<double>>  omega_map;

	//------------- FUNCTIONS --------------------

	void add_node_to_maps(nlohmann::json&, unsigned&, const std::string&);
	void make_omega_map();

public:
	PPNetwork();

	directed_graph<bayes_node>::kernel_1a_c bn;

	void from_json(const nlohmann::json&);
	nlohmann::json to_json();
	void from_file(std::ifstream&);
	void to_file(std::ofstream&);
	void to_bayesian_network();
	std::map<std::string, std::vector<double>> do_inference(std::vector<std::string>& inferred);
	std::map<std::string, std::vector<double>> do_inference(std::map<std::string, unsigned>& evidence, std::vector<std::string>& inferred);

	std::vector<std::string> get_nodes();
	std::vector<std::string> get_root_nodes();
	std::vector<std::string> get_leaf_nodes();
	std::vector<std::string> get_pro_nodes();
	std::vector<std::string> get_parents(std::string&);

	std::map<std::string, unsigned>  get_root_values(void);
	bool is_leaf(const std::string&);

	void add_cpd(const std::string&, const std::vector<double>);
	void add_cpd(const std::string&, const std::vector<std::vector<double>>);

};


#endif
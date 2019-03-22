#include "pp_network.h"
#include "utilities.h"

#include <random>

#include <range/v3/view.hpp>

using nlohmann::json;


PPNetwork::PPNetwork()
{

}

PPNetwork::PPNetwork(const PPNetwork& pp) :
	identification(pp.identification),
	description(pp.description),
	date(pp.date),
	root_nodes(pp.root_nodes),
	ext_nodes(pp.ext_nodes),
	pro_nodes(pp.pro_nodes),
	edges(pp.edges),
	cpds(pp.cpds),
	node_map(pp.node_map),
	card_map(pp.card_map),
	omega_map(pp.omega_map)
{

}

PPNetwork& PPNetwork::operator=(const PPNetwork& pp)
{
	identification = pp.identification;
	description = pp.description;
	date = pp.date;
	root_nodes = pp.root_nodes;
	ext_nodes = pp.ext_nodes;
	pro_nodes = pp.pro_nodes;
	edges = pp.edges;
	cpds = pp.cpds;
	node_map = pp.node_map;
	card_map = pp.card_map;
	omega_map = pp.omega_map;
	return *this;
}


void PPNetwork::from_json(const json& jzon)
{
	identification = jzon["header"]["identification"].get<std::string>();
	description = jzon["header"]["description"].get<std::string>();
	date = jzon["header"]["date"].get<std::string>();

	//TODO: Remove all those .get<...>()s
	std::vector<json> ron = jzon["nodes"]["RON"].get<std::vector<json>>();
	std::vector<json> ext = jzon["nodes"]["EXT"].get<std::vector<json>>();
	std::vector<json> pro = jzon["nodes"]["PRO"].get<std::vector<json>>();

	edges = jzon["edges"].get<std::vector<std::vector<std::string>>>();
	cpds = jzon["cpds"].get<json>();

	unsigned index{};
	for (json node : ron) { add_node_to_maps(node, index, "RON"); }
	for (json node : ext) { add_node_to_maps(node, index, "EXT"); }
	for (json node : pro) { add_node_to_maps(node, index, "PRO"); }

	make_omega_map();
}

json PPNetwork::to_json(void)
{
	return {
		{"header", {
			{"identification", identification},
			{"description", description},
			{"date", date}
		}},
		{"nodes", {
			{"RON", root_nodes},
			{"EXT", ext_nodes},
			{"PRO", pro_nodes}
		}},
		{"edges", edges},
		{"cpds", cpds}
	};
}

void PPNetwork::from_file(std::ifstream& ifs)
{
	json jzon{ json::parse(ifs) };
	from_json(jzon);
}

void PPNetwork::to_file(std::ofstream& ofs)
{
	std::string json_string{ to_json().dump() };
	ofs << json_string;
}

void PPNetwork::to_bayesian_network(void)
{
	int num = node_map.size();
	bn.set_number_of_nodes(num);

	// Add edges
	for (std::vector<std::string> edge : edges) 
	{ 
		bn.add_edge(node_map[edge[0]], node_map[edge[1]]); 
	}
	if (!graph_is_connected(bn)) { 
		std::cout << "Error: graph is not connected." << std::endl;
		std::cin.get();
	}

	// Set node cardinalities
	for (auto& entry : card_map) { set_node_num_values(bn, node_map[entry.first], entry.second); }
	//Set Conditional Probabilities
	assignment parent_state;
	// Root nodes
	for (std::string node : root_nodes)
	{
		unsigned card = card_map[node];
		unsigned long num = node_map[node];

		std::vector<double> probs = cpds[node];

		parent_state.clear();
		for (unsigned state = 0; state < card; state++) { set_node_probability(bn, num, state, parent_state, probs[state]); }
	}
	// Leaf nodes
	for (std::string leaf : get_leaf_nodes())
	{
		unsigned card = card_map[leaf];
		unsigned long num = node_map[leaf];

		parent_state.clear();
		std::vector<std::vector<double>> probs = cpds[leaf];
		std::vector<std::string> parents = get_parents(leaf);
		std::vector<int> parents_card;
		for (std::string parent : parents)
		{
			parents_card.push_back(card_map[parent]);
			parent_state.add(node_map[parent], 1);
		}
		std::vector<std::vector<int>> parents_states_matrix = States::get_index_matrix(parents_card);
		for (unsigned states = 0; states < parents_states_matrix[0].size(); states++) {
			for (unsigned parent = 0; parent < parents_states_matrix.size(); parent++) {
				parent_state[node_map[parents[parent]]] = parents_states_matrix[parent][states];
			}
			for (unsigned leaf_state = 0; leaf_state < card; leaf_state++) {
				set_node_probability(bn, node_map[leaf], leaf_state, parent_state, probs[leaf_state][states]);
			}
		}
	}
}

std::map<std::string, std::vector<double>> PPNetwork::do_inference(std::vector<std::string>& inferred)
{
	std::map<std::string, std::vector<double>> inference;

	join_tree_type join_tree;
	create_moral_graph(bn, join_tree);
	create_join_tree(join_tree, join_tree);
	bayesian_network_join_tree solution(bn, join_tree);

	for (std::string node : inferred)
	{
		unsigned num = node_map[node];
		matrix<double> prob = solution.probability(num);
		std::vector<double> result;
		for (auto p : prob) { result.push_back(p); }
		inference[node] = result;
	}

	return inference;
}

std::map<std::string, std::vector<double>> PPNetwork::do_inference(std::map<std::string, unsigned>& evidence, std::vector<std::string>& inferred)
{
	std::map<std::string, std::vector<double>> inference;

	for (auto& entry : evidence)
	{
		set_node_value(bn, node_map[entry.first], entry.second);
		set_node_as_evidence(bn, node_map[entry.first]);
	}

	join_tree_type join_tree;
	create_moral_graph(bn, join_tree);
	create_join_tree(join_tree, join_tree);
	bayesian_network_join_tree solution(bn, join_tree);

	for (auto& entry : evidence)
	{
		set_node_as_nonevidence(bn, node_map[entry.first]);
	}

	for (std::string node : inferred)
	{
		unsigned num = node_map[node];
		matrix<double> prob = solution.probability(num);
		std::vector<double> result;
		for (auto p : prob) { result.push_back(p); }
		inference[node] = result;
	}

	return inference;
}


void PPNetwork::add_node_to_maps(json& node, unsigned& index, const std::string& type)
{
	std::string name = node["name"];
	unsigned card = node["card"];

	node_map[name] = index;
	card_map[name] = card;

	if (type == "RON") { root_nodes.push_back(name); }
	if (type == "EXT") { ext_nodes.push_back(name); }
	if (type == "PRO") { pro_nodes.push_back(name); }

	index++;
}

void PPNetwork::make_omega_map(void)
{
	for (auto& entry : card_map)
	{
		std::string node = entry.first;
		unsigned card = entry.second;

		double max_omega = 2.0f * 3.141596f;
		for (std::string parent : get_parents(node)) { max_omega *= card_map[parent]; }
		std::vector<double> omega(card, 0.0);
		std::default_random_engine gen;
		std::uniform_real_distribution<double> dis(0.0, 1.0);
		for (int i = 0; i < omega.size(); i++) { omega[i] = dis(gen) * max_omega; }
		omega_map[node] = omega;
	}

}

std::vector<std::string> PPNetwork::get_nodes(void)
{
	return ranges::view::concat(root_nodes, ext_nodes, pro_nodes);
}

std::vector<std::string> PPNetwork::get_root_nodes(void)
{
	return root_nodes;
}

std::vector<std::string> PPNetwork::get_leaf_nodes(void)
{
	return ranges::view::concat(ext_nodes, pro_nodes);
}

std::vector<std::string> PPNetwork::get_ext_nodes(void)
{
	return ext_nodes;
}

std::vector<std::string> PPNetwork::get_pro_nodes(void)
{
	return pro_nodes;
}

std::vector<std::vector<std::string>> PPNetwork::get_edges()
{
	return edges;
}

std::vector<std::string> PPNetwork::get_parents(std::string& node)
{
	std::vector<std::string> parents;
	for (std::vector<std::string> edge : edges) { if (edge[1] == node) { parents.push_back(edge[0]); } }
	return parents;
}

std::vector<std::string> PPNetwork::get_children(std::string& node)
{
	std::vector<std::string> children;
	for (std::vector<std::string> edge : edges) { if (edge[0] == node) { children.push_back(edge[1]); } }
	return children;
}

std::map<std::string, unsigned> PPNetwork::get_root_values(void)
{
	std::map<std::string, unsigned> root_values;
	for (std::string root : get_root_nodes())
	{
		std::vector<double> cpd = cpds[root];
		root_values[root] = std::distance(cpd.begin(), std::max_element(cpd.begin(), cpd.end()));
	}
	return root_values;
}


std::vector<double> PPNetwork::get_cpd(const std::string& node)
{
	std::vector<double> cpd = cpds[node];
	return cpd;
}

unsigned PPNetwork::get_cardinality(const std::string& node)
{
	return card_map[node];
}

std::vector<double> PPNetwork::get_omega(const std::string& node)
{
	return omega_map[node];
}

void PPNetwork::add_root_node(const std::string node, unsigned cardinality)
{
	root_nodes.push_back(node);
	card_map[node] = cardinality;
}

void PPNetwork::add_ext_node(const std::string node, unsigned cardinality)
{
	ext_nodes.push_back(node);
	card_map[node] = cardinality;
}

void PPNetwork::add_pro_node(const std::string node, unsigned cardinality)
{
	pro_nodes.push_back(node);
	card_map[node] = cardinality;
}

void PPNetwork::add_edge(const std::vector<std::string> edge)
{
	edges.push_back(edge);
}

void PPNetwork::add_cpd(const std::string node, const std::vector<double> cpd)
{
	cpds.update({ { node, cpd } });
}

void PPNetwork::add_cpd(const std::string node, const std::vector<std::vector<double>> cpd)
{
	cpds.update({ { node, cpd } });
}

void PPNetwork::add_omega(const std::string node, std::vector<double> omega)
{
	omega_map[node] = omega;
}

void PPNetwork::remove_root_node(const std::string node)
{
	root_nodes.erase(std::remove(root_nodes.begin(), root_nodes.end(), node), root_nodes.end());
	card_map.erase(node);
	node_map.erase(node);
	omega_map.erase(node);
}

void PPNetwork::remove_edge(const std::vector<std::string> edge)
{
	edges.erase(std::remove(edges.begin(), edges.end(), edge), edges.end());
}

void PPNetwork::set_edges(std::vector<std::vector<std::string>> edges_)
{
	edges = edges_;
}

bool PPNetwork::is_leaf(const std::string& node)
{
	for (std::string leaf : get_leaf_nodes())
	{
		if (leaf == node)
		{
			return true;
		}
	}
	return false;
}

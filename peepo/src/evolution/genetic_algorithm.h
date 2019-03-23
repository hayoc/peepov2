#include <vector>
#include <random>

#include "utilities.h"
#include "pp/pp_network.h"

using json = nlohmann::json;

struct Individual
{
	Individual();
	Individual(const Individual&);
	Individual(PPNetwork& pp_network_, double = 0.0, double = 0.0, double = 0.0);
	Individual& operator=(const Individual&);

	PPNetwork pp_network;
	double fitness;
	double mut_top;
	double mut_cpd;
};

std::vector<json> get_topologies(PPNetwork&, unsigned);
std::vector<std::vector<std::string> > adjency_to_edges(const matrix<unsigned>&, const std::vector<std::string>&, const std::vector<std::string>&);
std::vector<std::vector<Individual>> get_mating_couples(std::vector<Individual>&);

std::vector<double> generate_random_vector(unsigned dim, double coef);
std::vector<std::vector<double>> ga_child_cpd(const std::vector<unsigned>& card_parents, const std::vector<double>& omega);
std::vector<std::vector<double>> normalize_distribution(std::vector<std::vector<double>> matrix);

matrix<unsigned> get_adjency_map(const std::vector<std::vector<std::string>>&, const std::vector<std::string>&, const std::vector<std::string>&);
bool valid_graph(matrix<unsigned>&, const std::vector<std::string>&, const std::vector<std::string>&);
bool is_in_edges(const std::vector<std::vector<std::string>>&, const std::string&, const std::string&);

void mutate_topology(PPNetwork&);
void mutate_cpds(PPNetwork&);
void add_edge(PPNetwork&);
void remove_edge(PPNetwork&);
void add_node(PPNetwork&);
void remove_node(PPNetwork&);


class GeneticAlgorithm {

private:
	double PI = 3.1415926;
	unsigned NUMBER_OF_MATING_PARENTS = 2;
	unsigned CROSS_OVER_TRESHOLD = 3;
	

	const std::string& source;
	unsigned n_pop;
	double p_mut_top;
	double p_mut_cpd;

	void select_parents(void);
	void cross_over(void);
	void mutate(void);

public:
	GeneticAlgorithm(const std::string& source_, unsigned  n_pop_, double p_mut_top_, double p_mut_cpd_);
	
	std::vector<Individual> selected_offspring;
	std::vector<Individual> selected_parents;
	Individual best_chromosome;

	double avg_fitness = 0;
	
	std::vector<Individual>& first_generation(void);
	void evolve(void);
};
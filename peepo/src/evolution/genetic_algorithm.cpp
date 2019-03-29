#include "genetic_algorithm.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>


Individual::Individual()
{

}

Individual::Individual(PPNetwork& pp_network_, double fitness_, double mut_top_, double mut_cpd_) :
	pp_network(pp_network_),
	fitness(fitness_),
	mut_top(mut_top_),
	mut_cpd(mut_cpd_)
{

}

Individual::Individual(const Individual& individual) :
	pp_network(individual.pp_network),
	fitness(individual.fitness),
	mut_top(individual.mut_top),
	mut_cpd(individual.mut_cpd)
{

}

Individual& Individual::operator=(const Individual& individual)
{
	pp_network = individual.pp_network;
	fitness = individual.fitness;
	mut_top = individual.mut_top;
	mut_cpd = individual.mut_cpd;
	return *this;
}

std::vector<json> get_topologies(PPNetwork& peepo, unsigned npop)
{
	std::vector<std::string> root_nodes = peepo.get_root_nodes();
	std::vector<std::string> leaf_nodes = peepo.get_leaf_nodes();
	std::vector<json> topologies;
	std::vector<std::vector<std::string> > a_topology;
	matrix<unsigned > map = ones_matrix<unsigned>(leaf_nodes.size(), root_nodes.size());
	map = ones_matrix<unsigned>(leaf_nodes.size(), root_nodes.size());
	json a;

	std::default_random_engine dre(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<double> di(0.0, 2.0*0.999999999999);//uniform distribution in interval [0,2)
	int count = 0;
	while (true) {
		for (int row = 0; row < map.nr(); row++) {
			for (int edge = 0; edge < map.nc(); edge++) {
				map(row, edge) = (unsigned)floor(di(dre));
			}
		}
		bool valid = valid_graph(map, root_nodes, leaf_nodes);
		if (valid) {
			a["edges"] = adjency_to_edges(map, root_nodes, leaf_nodes);
			a["entropy"] = adjency_to_edges(map, root_nodes, leaf_nodes).size();
			topologies.push_back(a);
		}
		if (topologies.size() > npop) { break; }
		count++;
		if (count > 2000)//safety net in order to limit the search time
		{
			topologies.insert(topologies.end(), topologies.begin(), topologies.end());//taking duplicates of already fount topologies
		}
	} //while loop
	std::random_shuffle(topologies.begin(), topologies.end());
	topologies.resize(npop - 1);
	map = ones_matrix<unsigned>(leaf_nodes.size(), root_nodes.size());//adding a fully connected network
	a["edges"] = adjency_to_edges(map, root_nodes, leaf_nodes);
	a["entropy"] = adjency_to_edges(map, root_nodes, leaf_nodes).size();
	topologies.push_back(a);

	return topologies;
}

std::vector<double> generate_random_vector(unsigned dim, double coef)
{
	std::default_random_engine dre(std::chrono::system_clock::now().time_since_epoch().count());
	std::uniform_real_distribution<double> di(0.0, coef);
	std::vector<double> my_rand(dim);
	std::generate(my_rand.begin(), my_rand.end(), [&] { return di(dre); });
	return my_rand;
}

std::vector<std::vector<double>> ga_child_cpd(const std::vector<unsigned>& card_parents, const std::vector<double>& omega)
{
	std::vector<std::vector<double>> pdf;
	double phase_shift = omega[0];
	unsigned n_comb = 1;
	for (auto card : card_parents) { n_comb *= card; };
	for (auto ang : omega)
	{
		std::vector<double> pdf_row;
		for (unsigned col = 0; col < n_comb; col++) { pdf_row.push_back(sin(ang*(col + 1) + phase_shift) + 1.2); }
		pdf.push_back(pdf_row);
	}
	return normalize_distribution(pdf);
}

std::vector<std::vector<double>> normalize_distribution(std::vector<std::vector<double>> matrix)
{
	std::vector<double> sum(matrix[0].size(), 0.0);
	for (unsigned column = 0; column < sum.size(); column++)
	{
		for (unsigned row = 0; row < matrix.size(); row++)
		{
			sum[column] += matrix[row][column];
		}
	}
	for (unsigned column = 0; column < sum.size(); column++)
	{
		for (unsigned row = 0; row < matrix.size(); row++)
		{
			matrix[row][column] /= sum[column];;
		}
	}
	return matrix;
}

bool valid_graph(matrix<unsigned>& map_,
	const std::vector<std::string>& root_nodes,
	const std::vector<std::string>& leaf_nodes)
{
	//check for  a minimum of edgs

	if (sum(map_) < unsigned(map_.nc())*unsigned(map_.nr()) / 2) { return false; }

	//check for orphan leaf nodes
	if (prod(sum_cols(map_)) == 0) { return false; }


	//check for unconnected root_nodes and adapt adjecnty map accordingly
	std::vector<unsigned> loose_roots;
	matrix<unsigned> sm_rows = sum_rows(map_);
	for (int root = 0; root < sm_rows.nc(); root++) {
		if (sm_rows(root) == 0) { loose_roots.push_back(root); }
	}

	if (loose_roots.size() != 0) {
		for (unsigned i = 0; i < loose_roots.size(); i++) {
			map_ = remove_col(map_, loose_roots[i]);
		}
	}
	//the next piece ofcode uses the bfs algo to check whether the network does not contains isolated garphs 
	//(i.e. a multigraph is excluded)
	//construct a normal adjancy matrix
	unsigned R = map_.nc();
	unsigned V = R + leaf_nodes.size();

	std::vector<std::vector<unsigned> > map(V, std::vector<unsigned>(V, 0));
	for (unsigned leaf = 0; leaf < leaf_nodes.size(); leaf++) {
		for (unsigned root = 0; root < R; root++) {
			if (map_(leaf, root) == 1) {
				map[root][leaf + R] = 1;
				map[leaf + R][root] = 1;//the true adgency matrix is symmetric}
			}

		}
	}
	//set diagonal to 1
	for (unsigned row = 0; row < R + leaf_nodes.size(); row++) { map[row][row] = 1; }
	//fill the rest
	std::vector<std::vector<unsigned> >   adj(V, std::vector<unsigned>(0, 0));
	for (unsigned row = 0; row < R + leaf_nodes.size(); row++) {
		for (unsigned col = 0; col < R + leaf_nodes.size(); col++) {
			if (map[row][col] == 1) { adj[row].push_back(col); }
		}
	}

	//loop through all leaf_nodes using the bfs algo
	for (unsigned leaf = R; leaf < R + leaf_nodes.size(); leaf++) {
		unsigned s = leaf;
		// Mark all the vertices as not visited
		std::vector<bool> visited(V, false);

		// Create a queue for BFS 
		std::list<unsigned> queue;

		// Mark the current node as visited and enqueue it 
		visited[s] = true;
		queue.push_back(s);
		// 'i' will be used to get all adjacent 
		// vertices of a vertex 
		std::vector<unsigned>::iterator i;

		while (!queue.empty())
		{
			// Dequeue a vertex from queue and print it 
			s = queue.front();
			queue.pop_front();

			// Get all adjacent vertices of the dequeued 
			// vertex s. If a adjacent has not been visited, 
			// then mark it visited and enqueue it 
			for (i = adj[s].begin(); i != adj[s].end(); ++i)
			{
				if (!visited[*i])
				{
					visited[*i] = true;
					queue.push_back(*i);
				}
			}
		}
		for (unsigned i = 0; i < visited.size(); i++) {
			if (visited[i] == false) { return false; }//an isolated subgraph ahs been encontered
		}
	}//end of leaf loop
	return true;
}


matrix<unsigned> get_adjency_map(const std::vector<std::vector<std::string> >& adges,
	const std::vector<std::string>& root_nodes,
	const std::vector<std::string>& leaf_nodes)
{
	matrix<unsigned> map;
	map = zeros_matrix<unsigned>(leaf_nodes.size(), root_nodes.size());
	for (unsigned row = 0; row < root_nodes.size(); row++) {
		for (unsigned col = 0; col < leaf_nodes.size(); col++) {
			for (auto edge : adges) {
				if (edge[1] == leaf_nodes[col] && edge[0] == root_nodes[row]) {
					map(col, row) = 1;
				}
			}
		}
	}
	return map;
}


std::vector<std::vector<std::string> > adjency_to_edges(const matrix<unsigned>& map,
	const std::vector<std::string>& root_nodes,
	const std::vector<std::string>& leaf_nodes)
{
	std::vector<std::vector<std::string> > edges;
	for (unsigned col = 0; col < root_nodes.size(); col++) {
		for (unsigned row = 0; row < leaf_nodes.size(); row++) {
			if (map(row, col) == 1) { edges.push_back({ root_nodes[col],leaf_nodes[row] }); }
		}
	}
	return edges;
}

std::vector<std::vector<Individual>> get_mating_couples(std::vector<Individual>& parents)
{
	std::vector<int> agents;
	for (int i = 0; i < parents.size(); i++) {
		agents.push_back(i);
	}
	std::vector<std::vector<Individual>> mating_couples;
	Combinations<int> comb_obj(agents, 2);
	std::vector<std::vector<int>> mating_indexes = comb_obj.get();
	for (int a = 0; a < mating_indexes.size(); a++)
	{
		mating_couples.push_back({ (parents[mating_indexes[a][0]]),(parents[mating_indexes[a][1]]) });
	}

	return mating_couples;

}

bool is_in_edges(const std::vector<std::vector<std::string>>& edges, const std::string&  root, const std::string& leaf)
{
	bool stop = true;
	for (auto edge : edges) {
		if (edge[0] == root && edge[1] == leaf) { stop = false; break; }
	}
	return stop;
}

void mutate_topology(PPNetwork& pp)
{
	if (std::rand() % 100 > 20)
	{
		add_edge(pp);
	}
	else
	{
		remove_edge(pp);
	}
}

void mutate_cpds(PPNetwork& pp)
{
	double epsilon = 0.05 + double(std::rand() % 1000 / 1000.0 * 0.7);
	for (std::string leaf : pp.get_leaf_nodes())
	{
		std::vector<unsigned> parents_card;
		for (std::string parent : pp.get_parents(leaf)) { parents_card.push_back(pp.get_cardinality(parent)); }
		
		std::vector<double> omega = pp.get_omega(leaf);
		for (unsigned i = 0; i < omega.size(); i++) { omega[i] += (0.5 - double(std::rand() % 1000) / 1000.0)*epsilon; }
		pp.add_omega(leaf, omega);

		std::vector<std::vector<double>> cpd = ga_child_cpd(parents_card, omega);
		pp.add_cpd(leaf, cpd);
	}
}

void add_edge(PPNetwork& pp)
{
	std::vector<std::string> roots = pp.get_root_nodes();
	if (roots.empty())
	{
		add_node(pp);
	}
	else
	{
		std::mt19937 engine{ std::random_device{}() };
		std::uniform_int_distribution<int> dist(0, roots.size() - 1);
		std::string parent = roots[dist(engine)];

		std::vector<std::string> child_pool;
		std::vector<std::string> children = pp.get_children(parent);
		for (std::string leaf : pp.get_leaf_nodes()) { if (std::find(children.begin(), children.end(), leaf) == children.end()) { child_pool.push_back(leaf); } }
		if (child_pool.empty())
		{
			remove_edge(pp);
		}
		else
		{
			std::uniform_int_distribution<int> dist(0, child_pool.size() - 1);
			std::string child = child_pool[dist(engine)];
			std::vector<std::string> edge{ parent, child };
			pp.add_edge(edge);

			std::vector<unsigned> parents_card;
			for (std::string parent : pp.get_parents(child)) { parents_card.push_back(pp.get_cardinality(parent)); }
			std::vector<std::vector<double>> cpd = ga_child_cpd(parents_card, pp.get_omega(child));
			pp.add_cpd(child, cpd);
		}
	}
}

void remove_edge(PPNetwork& pp)
{
	std::vector<std::string> roots = pp.get_root_nodes();
	if (roots.empty())
	{
		add_node(pp);
	}
	else
	{
		std::mt19937 engine{ std::random_device{}() };
		std::uniform_int_distribution<int> dist(0, roots.size() - 1);
		std::string parent = roots[dist(engine)];

		std::vector<std::string> children = pp.get_children(parent);
		if (children.size() <= 1)
		{
			add_edge(pp);
		}
		else
		{
			std::uniform_int_distribution<int> dist(0, children.size() - 1);
			std::string child = children[dist(engine)];

			if (pp.get_parents(child).size() <= 1) { add_edge(pp); }
			
			std::vector<std::string> edge{ parent, child };
			pp.remove_edge(edge);

			std::vector<unsigned> parents_card;
			for (std::string parent : pp.get_parents(child)) { parents_card.push_back(pp.get_cardinality(parent)); }
			std::vector<std::vector<double>> cpd = ga_child_cpd(parents_card, pp.get_omega(child));
			pp.add_cpd(child, cpd);
		}
	}
}

void add_node(PPNetwork& pp)
{
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	const std::string new_parent = boost::uuids::to_string(uuid);

	std::vector<std::string> leaves = pp.get_leaf_nodes();

	std::mt19937 engine{ std::random_device{}() };
	std::uniform_int_distribution<int> dist(0, leaves.size() - 1);
	std::string child = leaves[dist(engine)];

	std::vector<std::string> edge{ new_parent, child };
	pp.add_root_node(new_parent, 2);
	pp.add_edge(edge);

	std::vector<double> omega{ 0.0, 0.0 };
	std::vector<double> parent_cpd{ 0.5, 0.5 };
	pp.add_omega(new_parent, omega);
	pp.add_cpd(new_parent, parent_cpd);

	std::vector<unsigned> parents_card;
	for (std::string parent : pp.get_parents(child)) { parents_card.push_back(pp.get_cardinality(parent)); }
	std::vector<std::vector<double>> child_cpd = ga_child_cpd(parents_card, pp.get_omega(child));
	pp.add_cpd(child, child_cpd);
}

void remove_node(PPNetwork& pp)
{
	std::vector<std::string> roots = pp.get_root_nodes();
	if (roots.empty())
	{
		add_node(pp);
	}
	else
	{
		std::mt19937 engine{ std::random_device{}() };
		std::uniform_int_distribution<int> dist(0, roots.size() - 1);
		std::string to_remove = roots[dist(engine)];

		std::vector<std::string> affected_children = pp.get_children(to_remove);
		
		pp.remove_root_node(to_remove);

		for (std::string child : affected_children)
		{
			std::vector<unsigned> parents_card;
			for (std::string parent : pp.get_parents(child)) { parents_card.push_back(pp.get_cardinality(parent)); }
			std::vector<std::vector<double>> cpd = ga_child_cpd(parents_card, pp.get_omega(child));
			pp.add_cpd(child, cpd);
		}
	}
}

GeneticAlgorithm::GeneticAlgorithm(
	const std::string& source_,
	unsigned  n_pop_,
	double p_mut_top_,
	double p_mut_cpd_) :
	source(source_),
	n_pop(n_pop_),
	p_mut_top(p_mut_top_),
	p_mut_cpd(p_mut_cpd_)
{
	srand(time(NULL));
}

std::vector<Individual>& GeneticAlgorithm::first_generation(void)
{
	selected_offspring.clear();

	PPNetwork pp_template;
	std::ifstream ifs(source);
	pp_template.from_file(ifs);

	std::vector<json> topologies = get_topologies(pp_template, n_pop);
	
	for (json topology : topologies)
	{
		std::vector<std::vector<std::string>> edges = topology["edges"];
		pp_template.set_edges(edges);
		PPNetwork pp = PPNetwork(pp_template);

		for (auto node : pp_template.get_nodes())
		{
			std::vector<std::string> parents = pp.get_parents(node);
			if (parents.size() == 0)
			{
				std::vector<double> cpd;
				std::vector<double> omega;

				unsigned card = pp.get_cardinality(node);
				for (unsigned i = 0; i < card; i++)
				{
					cpd.push_back(1.0 / double(card));
					omega.push_back(0.0);
				}

				pp.add_cpd(node, cpd);
				pp.add_omega(node, omega);
			}
			else
			{
				std::vector<unsigned> parents_card;
				unsigned product = 1;
				for (std::string parent : parents)
				{
					parents_card.push_back(pp.get_cardinality(parent));
					product *= pp.get_cardinality(parent);
				}
				std::vector<double> omega = generate_random_vector(pp.get_cardinality(node), 2.0 * PI * product);
				std::vector<std::vector<double>> cpd = ga_child_cpd(parents_card, omega);

				pp.add_omega(node, omega);
				pp.add_cpd(node, cpd);
			}
		}
		Individual individual{ pp };
		selected_offspring.push_back(individual);
	}
	return selected_offspring;
}

void GeneticAlgorithm::evolve(void)
{
	std::sort(selected_offspring.begin(), selected_offspring.end(), [](const Individual& a, const Individual& b) { return a.fitness > b.fitness; });
	if (selected_offspring[0].fitness >= best_chromosome.fitness) 
	{ 
		best_chromosome = Individual{ selected_offspring[0] }; 
	}
	
	avg_fitness = 0.0;
	for (int individual = 0; individual < selected_offspring. size(); individual++) {		
		avg_fitness += selected_offspring[individual].fitness; 
	}
	avg_fitness /= selected_offspring.size();

	// SELECTION
	select_parents();

	// CROSS-OVER
	cross_over();

	// MUTATION
	mutate();
	
}

void GeneticAlgorithm::select_parents(void)
{
	selected_parents.clear();

	std::vector<unsigned> pool;
	for (unsigned index = 0; index < n_pop; index++) {
		unsigned repeat = n_pop - index;
		for (unsigned i = 0; i < repeat; i++) {
			pool.push_back(index);
		}
	}
	
	std::random_shuffle(pool.begin(), pool.end());
	for (unsigned draw = 0; draw < NUMBER_OF_MATING_PARENTS; draw++) {
		unsigned pool_index = std::rand() % (pool.size() - 1);
		unsigned parent_index = pool[pool_index];
		PPNetwork pp{ selected_offspring[parent_index].pp_network };
		selected_parents.push_back(Individual{ pp });
	}
}

void GeneticAlgorithm::cross_over(void)
{
	selected_offspring.clear();
	std::vector<std::vector<Individual>> mating_couples = get_mating_couples(selected_parents);

	for (std::vector<Individual> couple : mating_couples)
	{
		matrix<unsigned> map_1 = get_adjency_map(couple[0].pp_network.get_edges(), couple[0].pp_network.get_root_nodes(), couple[0].pp_network.get_leaf_nodes());
		matrix<unsigned> map_2 = get_adjency_map(couple[1].pp_network.get_edges(), couple[1].pp_network.get_root_nodes(), couple[1].pp_network.get_leaf_nodes());
		matrix<int> diff = abs(matrix_cast<int>(map_1) - matrix_cast<int>(map_2));
		if (sum(diff) == 0 || sum(diff) > CROSS_OVER_TRESHOLD) {
			//-> there is no difference in topology between the parents, two offsprings, identical to the parents are created but they will have 100% chance to mutate,  both in topology as for cpd
			// or the difference between parents is too big.We assume cloning of the  parents(will be mutated!)
			selected_offspring.push_back(Individual{ couple[0] });
			selected_offspring.push_back(Individual{ couple[1] });
			continue;
		}

		//we now construct offsprings for all other cases:
		//if there are q positions with different values in the two adjency matrices, then we will have 2 ^ q - 2 offspring
		std::vector<std::vector<int>> indices;
		for (int row = 0; row < diff.nr(); row++) {
			for (int col = 0; col < diff.nc(); col++) {
				if (diff(row, col) == 1) {
					indices.push_back({ row,col });
				}
			}
		}

		std::vector<std::vector<int>> combinations;
		combinations.push_back({ 1,0 });
		combinations.push_back({ 0,1 });
		if (indices.size() > 1) {
			std::vector<int> combo(indices.size(), 2);
			combinations = States::transpose(States::get_index_matrix(combo));
		}

		for (std::vector<int> combination : combinations)
		{
			matrix<unsigned> map = map_1;
			for (unsigned pos = 0; pos < indices.size(); pos++) {
				map(indices[pos][0], indices[pos][1]) = combination[pos];
			}
			matrix<unsigned> check_map = sum_cols(map);
			if (min(check_map) == 0 || map == map_1 || map == map_2) {
				continue;
			}

			std::vector<std::vector<std::string>> edges = adjency_to_edges(map, selected_parents[0].pp_network.get_root_nodes(), selected_parents[0].pp_network.get_leaf_nodes());
			
			PPNetwork pp{ couple[0].pp_network };
			pp.set_edges(edges);

			for (std::string node : pp.get_nodes())
			{
				std::vector<std::string> parents = pp.get_parents(node);
				if (parents.size() == 0)
				{
					std::vector<double> cpd;
					std::vector<double> omega;

					unsigned card = pp.get_cardinality(node);
					for (unsigned i = 0; i < card; i++)
					{
						cpd.push_back(1.0 / double(card));
						omega.push_back(0.0);
					}

					pp.add_cpd(node, cpd);
					pp.add_omega(node, omega);
				}
				else
				{
					std::vector<unsigned> parents_card;
					unsigned product = 1;
					for (std::string parent : parents)
					{
						parents_card.push_back(pp.get_cardinality(parent));
						product *= pp.get_cardinality(parent);
					}
					std::vector<double> omega = generate_random_vector(pp.get_cardinality(node), 2.0 * PI * product);
					std::vector<std::vector<double>> cpd = ga_child_cpd(parents_card, omega);

					pp.add_omega(node, omega);
					pp.add_cpd(node, cpd);
				}
			}

			double mut_top = double(std::rand() % 100) / 100.0;
			double mut_cpd = double(std::rand() % 100) / 100.0;
			if (combination[0] == combination[1]) {
				mut_top = 0.0;
				mut_cpd = 0.0;
			}
			Individual individual{ pp, 0.0, mut_top, mut_cpd };
			selected_offspring.push_back(individual);
		}
	}
	while (true) {//expand the population to npop; the added peepo which are clones, will be mutated
		int n_chrom = selected_parents.size() + selected_offspring.size();
		if (n_chrom > n_pop) { break; }
		for (auto x : selected_parents) {
			Individual y(x.pp_network, 0.0, 0.0, 0.0);
			selected_offspring.push_back(y);
		}
	}
}

void GeneticAlgorithm::mutate(void)
{
	for (Individual individual : selected_offspring)
	{
		if (individual.mut_top <= p_mut_top) { mutate_topology(individual.pp_network); }
		if (individual.mut_cpd <= p_mut_cpd) { mutate_cpds(individual.pp_network); }
	}
}
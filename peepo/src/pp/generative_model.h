#ifndef GENERATIVE_MODEL__H
#define GENERATIVE_MODEL__H

#include "pp_network.h"
#include "peepo.h"

class GenerativeModel {

public:

	double process(Peepo&);

	std::map<std::string, std::vector<double>> predict(Peepo& peepo);
	std::vector<double> calculate_error(const std::vector<double>&, const std::vector<double>&);
	double calculate_error_size(const std::vector<double>&, const std::vector<double>&);
	void error_minimization(Peepo& peepo, const std::string&, const std::vector<double>&, const std::vector<double>&);
	void hypothesis_update(Peepo& peepo, const std::string&, const std::vector<double>&, const std::vector<double>&);
};

#endif

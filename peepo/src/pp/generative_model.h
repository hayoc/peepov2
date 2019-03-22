#ifndef GENERATIVE_MODEL__H
#define GENERATIVE_MODEL__H

#include "pp_network.h"
#include "peepo.h"

class GenerativeModel {

private:
	PPNetwork pp_network;
	Peepo& peepo;


public:
	GenerativeModel(PPNetwork&, Peepo&);
	double process();

	std::map<std::string, std::vector<double>> predict();
	std::vector<double> calculate_error(const std::vector<double>&, const std::vector<double>&);
	double calculate_error_size(const std::vector<double>&, const std::vector<double>&);
	void error_minimization(const std::string&, const std::vector<double>&, const std::vector<double>&);
	void hypothesis_update(const std::string&, const std::vector<double>&, const std::vector<double>&);
};

#endif

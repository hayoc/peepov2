#ifndef PEEPO__H
#define PEEPO__H

#include <string>
#include <vector>
#include <pp/pp_network.h>

class Peepo {

public:
	PPNetwork& pp_network;
	
	Peepo(PPNetwork& pp_network_) : pp_network(pp_network_) {};

	virtual void action(const std::string&, const std::vector<double>&) = 0;
	virtual std::vector<double> observation(const std::string&) = 0;
};

#endif

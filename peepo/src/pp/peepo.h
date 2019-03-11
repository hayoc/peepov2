#ifndef PEEPO__H
#define PEEPO__H

#include <string>
#include <vector>


class Peepo {

public:
	virtual void action(const std::string&, const std::vector<double>&) = 0;
	virtual std::vector<double> observation(const std::string&) = 0;
};

#endif

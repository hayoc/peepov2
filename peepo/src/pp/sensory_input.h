#ifndef SENSORY_INPUT__H
#define SENSORY_INPUT__H

#include <string>
#include <vector>


class SensoryInput {

public:
	virtual void action(const std::string&, const std::vector<double>&) = 0;
	virtual std::vector<double> observation(const std::string&) = 0;
};

#endif

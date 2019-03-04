#ifndef COMBINATIONS_H
#define COMBINATIONS_H
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <vector>
struct Agent;

class States {
public:

	static std::vector<std::vector<unsigned int> > cartesian(std::vector<std::vector<unsigned int> >& v)
	{

		auto product = [](long long a, std::vector<unsigned int>& b) { return a * b.size(); };
		const long long N = accumulate(v.begin(), v.end(), 1LL, product);
		std::vector<unsigned int> u(v.size());
		std::vector<std::vector<unsigned int> > result_;
		for (long long n = 0; n < N; ++n) {
			lldiv_t q{ n, 0 };
			for (long long i = v.size() - 1; 0 <= i; --i) {
				q = std::div(q.quot, v[i].size());
				u[i] = v[i][q.rem];
			}
			result_.push_back(u);
		}
		return result_;
	}


	static std::vector<std::vector<unsigned int> > get_index_matrix(const   std::vector<unsigned int> & cardinality)
	{
		std::vector<std::vector<unsigned int> > v;
		for (int k = 0; k < cardinality.size(); k++) {
			std::vector<unsigned int> help;
			for (unsigned int m = 0; m < cardinality[k]; m++) {
				help.push_back(m);
			}
			v.push_back(help);
		}
		std::vector<std::vector<unsigned int> > result_ = cartesian(v);
		std::vector<std::vector<unsigned int> > result;
		for (unsigned int col = 0; col < result_[0].size(); col++) {
			std::vector<unsigned int> help;
			for (unsigned int row = 0; row < result_.size(); row++) {
				help.push_back(result_[row][col]);
			}
			result.push_back(help);
		}
		return result;
	}
};

#endif
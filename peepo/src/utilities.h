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

	static std::vector<std::vector<int>> cartesian(std::vector<std::vector<int> >& v)
	{

		auto product = [](long long a, std::vector<int>& b) { return a * b.size(); };
		const long long N = accumulate(v.begin(), v.end(), 1LL, product);
		std::vector<int> u(v.size());
		std::vector<std::vector<int> > result_;
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


	static std::vector<std::vector<int>> get_index_matrix(const std::vector<int>& cardinality)
	{
		std::vector<std::vector<int> > v;
		for (int k = 0; k < cardinality.size(); k++) {
			std::vector<int> help;
			for (int m = 0; m < cardinality[k]; m++) {
				help.push_back(m);
			}
			v.push_back(help);
		}
		std::vector<std::vector<int>> result_ = cartesian(v);
		std::vector<std::vector<int>> result;
		for (int col = 0; col < result_[0].size(); col++) {
			std::vector<int> help;
			for (int row = 0; row < result_.size(); row++) {
				help.push_back(result_[row][col]);
			}
			result.push_back(help);
		}
		return result;
	}

	static std::vector<std::vector<int>> transpose(const std::vector<std::vector<int>>& mat)
	{
		static std::vector<std::vector<int>> tp(mat[0].size(), std::vector<int>(mat.size()));
		for (int row = 0; row < mat.size(); row++) {
			for (int col = 0; col < mat[row].size(); col++) {
				tp[col][row] = mat[row][col];

			}
		}
		return tp;
	}
};

template<typename T>
class Combinations {
	// Combinations(std::vector<T> s, int m) iterate all Combinations without repetition
	// from set s of size m s = {0,1,2,3,4,5} all permuations are: {0, 1, 2}, {0, 1,3}, 
	// {0, 1, 4}, {0, 1, 5}, {0, 2, 3}, {0, 2, 4}, {0, 2, 5}, {0, 3, 4}, {0, 3, 5},
	// {0, 4, 5}, {1, 2, 3}, {1, 2, 4}, {1, 2, 5}, {1, 3, 4}, {1, 3, 5}, {1, 4, 5}, 
	// {2, 3, 4}, {2, 3, 5}, {2, 4, 5}, {3, 4, 5}
private:
	void generate(int i, int j, int m);
	unsigned long long comb(unsigned long long n, unsigned long long k); // C(n, k) = n! / (n-k)!

	int N;
	int M;
	std::vector<T> set;
	std::vector<T> partial;
	std::vector<std::vector<T>> out;
	int count = 0;

public:
	Combinations(std::vector<T> s, int m) : M(m), set(s), partial(std::vector<T>(M))
	{
		N = s.size(); // unsigned long can't be casted to int in initialization

		out = std::vector<std::vector<T>>(comb(N, M), std::vector<T>(M)); // allocate space

		generate(0, N - 1, M - 1);
	};

	typedef typename std::vector<std::vector<T>>::const_iterator const_iterator;
	typedef typename std::vector<std::vector<T>>::iterator iterator;
	iterator begin() { return out.begin(); }
	iterator end() { return out.end(); }
	std::vector<std::vector<T>> get() { return out; }
	unsigned long long get_n_comb(void)
	{
		return comb(N, M);
	}

};

template<typename T>
void Combinations<T>::generate(int i, int j, int m) {
	// combination of size m (number of slots) out of set[i..j]
	if (m > 0) {
		for (int z = i; z < j - m + 1; z++) {
			partial[M - m - 1] = set[z]; // add element to permutation
			generate(z + 1, j, m - 1);
		}
	}
	else {
		// last position
		for (int z = i; z < j - m + 1; z++) {
			partial[M - m - 1] = set[z];
			out[count++] = std::vector<T>(partial); // add to output vector
		}
	}
}

template<typename T>
unsigned long long
Combinations<T>::comb(unsigned long long n, unsigned long long k) {
	// this is from Knuth vol 3

	if (k > n) {
		return 0;
	}
	unsigned long long r = 1;
	for (unsigned long long d = 1; d <= k; ++d) {
		r *= n--;
		r /= d;
	}
	return r;
}


#endif
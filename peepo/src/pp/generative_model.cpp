#include "generative_model.h"


double GenerativeModel::process(Peepo& peepo)
{
	if (peepo.pp_network.bn.number_of_nodes() == 0) 
	{ 
		peepo.pp_network.to_bayesian_network();
	}

	double total_prediction_error_size{ 0.0 };
	std::map<std::string, std::vector<double>> predictions = predict(peepo);
	for (auto& entry : predictions)
	{
		std::string node = entry.first;
		if (peepo.pp_network.is_leaf(node))
		{
			std::vector<double> prediction = entry.second;
			std::vector<double> observation = peepo.observation(node);

			std::vector<double> prediction_error = calculate_error(prediction, observation);
			double prediction_error_size = calculate_error_size(prediction, observation);
			total_prediction_error_size += prediction_error_size;
			if (prediction_error_size > 0.1) //TODO: Arbitrary
			{
				//std::cout << "Prediction for node: " << node << " = [" << prediction[0] << ", " << prediction[1] << "] VS Observation = [" << observation[0] << ", " << observation[1] <<  std::endl;
				error_minimization(peepo, node, prediction_error, prediction);
			}
		}
	}
	return total_prediction_error_size;
}

std::map<std::string, std::vector<double>> GenerativeModel::predict(Peepo& peepo)
{
	std::vector<std::string> inferred = peepo.pp_network.get_leaf_nodes();
	return peepo.pp_network.do_inference(inferred);
}

std::vector<double> GenerativeModel::calculate_error(const std::vector<double>& pred, const std::vector<double>& obs)
{
	if (pred.size() != obs.size()) {
		std::cout << "Error in error estimation (prediction and observation have different size." << std::endl;
		std::cin.get();
	}
	std::vector<double> err;
	for (int i = 0; i < pred.size(); i++) {
		err.push_back(obs[i] - pred[i]);
	}
	return err;
}

double GenerativeModel::calculate_error_size(const std::vector<double>& pred, const std::vector<double>& obs)
{
	if (pred.size() != obs.size()) {
		std::cout << "Error in error size estimation (prediction and observation have different size." << std::endl;
		std::cin.get();
	}
	double entropy = 0.0;
	for (int i = 0; i < pred.size(); i++) {
		entropy += obs[i] * log(pred[i] / obs[i]);
	}
	return -entropy;
}

void GenerativeModel::error_minimization(Peepo& peepo, const std::string& node_name, const std::vector<double>& prediction_error, const std::vector<double>& prediction)
{
	hypothesis_update(peepo, node_name, prediction_error, prediction);
}

void GenerativeModel::hypothesis_update(Peepo& peepo, const std::string& node_name, const std::vector<double>& prediction_error, const std::vector<double>& prediction)
{
	std::vector<std::string> pro_nodes = peepo.pp_network.get_pro_nodes();
	if (std::find(pro_nodes.begin(), pro_nodes.end(), node_name) != pro_nodes.end())
	{
		peepo.action(node_name, prediction);
	}
	else
	{
		std::vector<double> sum = prediction_error;
		std::transform(sum.begin(), sum.end(), prediction.begin(), sum.begin(), std::plus<double>());

		std::map<std::string, unsigned> evidence = { {node_name, std::distance(sum.begin(), std::max_element(sum.begin(), sum.end())) } };
		std::vector<std::string> inferred = peepo.pp_network.get_root_nodes();
		std::map<std::string, std::vector<double>> inference = peepo.pp_network.do_inference(evidence, inferred);
		for (auto& entry : inference)
		{
			std::string oldname = entry.first;
			std::vector<double> old = peepo.pp_network.get_cpd(oldname);
			//std::cout << "Hypo Update " << oldname << "  FROM [" << old[0] << ", " << old[1] << "] TO [" << entry.second[0] << ", " << entry.second[1] << "]" << std::endl;

			//TODO: This only works for hypos with card 2
			std::vector<double> hypo = entry.second;
			if (hypo[0] == 1.0)
			{
				hypo[0] -= 0.01;
				hypo[1] += 0.01;
			}
			if (hypo[1] == 1.0)
			{
				hypo[0] += 0.01;
				hypo[1] -= 0.01;
			}

			peepo.pp_network.add_cpd(entry.first, hypo);
		}
		peepo.pp_network.to_bayesian_network();
	}
}

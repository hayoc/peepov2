#include "generative_model.h"

GenerativeModel::GenerativeModel(PPNetwork& pp_network_, Peepo& peepo_):
	pp_network(pp_network_),
	peepo(peepo_)
{
	pp_network.to_bayesian_network();
}

double GenerativeModel::process(void)
{
	double total_prediction_error_size{ 0.0 };
	std::map<std::string, std::vector<double>> predictions = predict();
	for (auto& entry : predictions)
	{
		std::string node = entry.first;
		if (pp_network.is_leaf(node))
		{
			std::vector<double> prediction = entry.second;
			std::vector<double> observation = peepo.observation(node);

			std::vector<double> prediction_error = calculate_error(prediction, observation);
			double prediction_error_size = calculate_error_size(prediction, observation);
			total_prediction_error_size += prediction_error_size;
			if (prediction_error_size > 0.1) //TODO: Arbitrary
			{
				//std::cout << "Prediction for node: " << node << " = [" << prediction[0] << ", " << prediction[1] << "] VS Observation = [" << observation[0] << ", " << observation[1] <<  std::endl;
				error_minimization(node, prediction_error, prediction);
			}
		}
	}
	return total_prediction_error_size;
}

std::map<std::string, std::vector<double>> GenerativeModel::predict(void)
{
	std::vector<std::string> inferred = pp_network.get_leaf_nodes();
	return pp_network.do_inference(inferred);
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

void GenerativeModel::error_minimization(const std::string& node_name, const std::vector<double>& prediction_error, const std::vector<double>& prediction)
{
	hypothesis_update(node_name, prediction_error, prediction);
}

void GenerativeModel::hypothesis_update(const std::string& node_name, const std::vector<double>& prediction_error, const std::vector<double>& prediction)
{
	std::vector<std::string> pro_nodes = pp_network.get_pro_nodes();
	if (std::find(pro_nodes.begin(), pro_nodes.end(), node_name) != pro_nodes.end())
	{
		peepo.action(node_name, prediction);
	}
	else
	{
		std::vector<double> sum = prediction_error;
		std::transform(sum.begin(), sum.end(), prediction.begin(), sum.begin(), std::plus<double>());

		std::map<std::string, unsigned> evidence = { {node_name, std::distance(sum.begin(), std::max_element(sum.begin(), sum.end())) } };
		std::vector<std::string> inferred = pp_network.get_root_nodes();
		std::map<std::string, std::vector<double>> inference = pp_network.do_inference(evidence, inferred);
		for (auto& entry : inference)
		{
			//std::string oldname = entry.first;
			//std::vector<double> old = pp_network.get_cpd(oldname);
			//std::cout << "Hypo Update " << oldname << "  FROM [" << old[0] << ", " << old[1] << "] TO [" << entry.second[0] << ", " << entry.second[1] << "]" << std::endl;
			pp_network.add_cpd(entry.first, entry.second);
		}
		pp_network.to_bayesian_network();
	}
}

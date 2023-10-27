// Gates.cpp
// Implementation of the Circuit class.

#include "Circuit.h"
#include <iostream>  
#include <sstream>  


// constructor
Circuit::Circuit(void) {
}


// set the numbre of bits inputs of the circuit
void Circuit::set_inputs(int bit_count) {
	for (int i = 0; i < bit_count; i++) {
		Connection* input_i = new Connection();
		inputs.push_back(input_i);
}}

// add connections ouput
void Circuit::add_output(std::vector<Connection*> outputs)
{
	int i = nb_bits_output;
	for (Connection* connexion : outputs) {
		connexion->is_output = true;
		connexion->n_ouput_index = i;
		i++;
	}
	nb_bits_output = 1;
}


// add a gate into the circuit
void Circuit::add_gate(Gate* gate) {
	// add the gate to the circuit
	gates.push_back(gate);
	// add ouputs to the circuits
	std::vector<Connection*> outs = gate->get_outputs();
	for (Connection* ouput_gate_i : outs) {
		// +1 connection
		connections.push_back(ouput_gate_i);
	}
	
}

// get all gartes that have calculated inputes but have not been run yet
std::vector< Gate*> Circuit::_get_computable_gate(void) const {
	std::vector< Gate*> computable_gates;
	for (Gate* gate : gates) {
		if (    gate->all_inputs_calculated() 
			&& !gate->is_computed) {
			computable_gates.push_back(gate);
		}
	}
	return computable_gates;
}

// run the circuit
std::vector<Bit> Circuit::run(const CInputs& in_values) const {
	assert(nb_bits_output > 0);
	assert(in_values.size() > 0);
	assert(in_values.size() == inputs.size());

	// init bits values in input 
	for (int i = 0; i < in_values.size(); i++)
	{
		inputs[i]->set_value(in_values[i]);
	}

	// run whle we have not a result 
	while (true)
	{
		// get gates that have calculated intpus
		std::vector< Gate*> gate_to_run = _get_computable_gate();
		// if no more gates to run, error
		if (gate_to_run.size() == 0) 
			throw "internal error : no more gates";
		// run the gates
		for (Gate* gate : gate_to_run) {
			gate->compute();
			assert(gate->is_computed);
		}
		// do we have a result ?
		// =all the needed outpus bits are calculated

		// init result from calculated outos
		std::vector<Bit> result(nb_bits_output);
		int bits_calculated = 0;
		for (Connection* connection_i : connections) {

			if (connection_i->is_output
				&& connection_i->is_calculated()) {
				bool value_calculted = connection_i->get_value();
				result[connection_i->n_ouput_index].set_value(value_calculted);
				bits_calculated++;
			}
		}
		// if all bits are calculated, return the result
		if (bits_calculated == nb_bits_output)
			return result;
		// continue calculation
		
	} // while (true)

}
// reset the circuit gate before a new run
void Circuit::reset(void) const
{
	for (Gate* gate : gates) {
		gate->is_computed = false;
	}
	for (Connection* connection_i : connections) {
		connection_i->reset();
	}
	for (Connection* connection_i : inputs){
		connection_i->reset();
	}
}
// export to a string
// inspired from https://github.com/mcbagz/LogicGates/blob/main/Example.ipynb
// format : https://pypi.org/project/circuit/
std::string Circuit::export_to_string(void) const {
	//std::string result;
	std::ostringstream str_stream;
	export_to_stream(str_stream);
	return str_stream.str();
}
// export to a strem
// inspired from https://github.com/mcbagz/LogicGates/blob/main/Example.ipynb
// format : https://pypi.org/project/circuit/
void Circuit::export_to_stream(std::ostream& out) const {


	//# of gates
	out << std::to_string(gates.size());
	out << ' ';
	//# of of wires
	out << std::to_string(connections.size());
	out << "\n";
	//# of inouts
	out << std::to_string(inputs.size());
	out << " ";
}

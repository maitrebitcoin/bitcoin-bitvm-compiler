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
	assert(!is_fully_constructed);
	for (int i = 0; i < bit_count; i++) {
		Connection* input_i = new Connection();
		inputs.push_back(input_i);
}}

// set connections ouput
void Circuit::set_output(std::vector<Connection*> new_outputs)
{
	assert(!is_fully_constructed);
	assert(outputs.size() == 0);
	assert(new_outputs.size() > 0);
	int i = nb_bits_output();
	assert(i == 0);

	for (Connection* connexion : new_outputs) {
		connexion->is_output = true;
		connexion->n_ouput_index = i;
		i++;
		outputs.push_back(connexion);
	}
}


// add a gate and its connexions into the circuit
void Circuit::add_gate(Gate* gate) {
	assert(!is_fully_constructed);
	// add the gate to the circuit
	gates.push_back(gate);
	// add ouputs to the circuits
	std::vector<Connection*> outs = gate->get_outputs();
	for (Connection* ouput_gate_i : outs) {
		// +1 connection
		connections.push_back(ouput_gate_i);
	}
}
// a get connection for a litteral values
Connection* Circuit::get_literal_values(bool b) {
	// init at 1st call
	if (literals01[b] == nullptr) {
		literals01[b] = new Connection();
		literals01[b]->set_value(b);
	}
	return literals01[b];
}
// is the circuit usign litteral values ?
bool Circuit::is_using_litteral_values(void) const {
	return literals01[0] != nullptr
		|| literals01[1] != nullptr; 
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

// get inout of the circuit. to set values before running it
CRunInputs Circuit::get_run_inputs(void) const {
	assert(is_fully_constructed);
	CRunInputs result;
	result.init((int)inputs.size());
	return std::move(result);
}


// run the circuit
std::vector<Bit> Circuit::run(const CRunInputs& in_values) const {
	assert(nb_bits_output() > 0);
	assert(in_values.size() > 0);
	assert(in_values.size() == inputs.size());

	// init bits values in input 
	for (int i = 0; i < in_values.size(); i++)
	{
		inputs[i]->set_value(in_values[i]);
	}

	// run whle we have not a result 
	int nb_step = 0;
	while (true)
	{
		// get gates that have calculated intpus
		std::vector< Gate*> gate_to_run = _get_computable_gate();
		// if no more gates to run, error
		if (gate_to_run.size() == 0) 
			throw Error( "internal error : no more gates" );
		// run the gates
		for (Gate* gate : gate_to_run) {
			gate->compute();
			assert(gate->is_computed);
		}
		// do we have a result ?
		// =all the needed outpus bits are calculated

		// init result from calculated outos
		std::vector<Bit> result(nb_bits_output());
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
		if (bits_calculated == nb_bits_output())
			return result;
		// continue calculation
		nb_step++;
		
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
// init gates and connections ID
void Circuit::init_id(void) 
{
	// give an ID to all gates and all connections
	for (int i = 0; i < gates.size(); i++) {
		gates[i]->id = i;
	}
	// init literals
	int connection_id = 0;
	if (literals01[0])
		literals01[0]->id = connection_id++;
	if (literals01[1])
		literals01[1]->id = connection_id++;
	// init inputs and connections
	for (Connection* input_i : inputs) {
		input_i->id = connection_id;
		connection_id++;
	}
	for (Connection* connection_i : connections) {
		if (connection_i->id != 0)
			continue;
		connection_i->id = connection_id;
		connection_id++;
	}

	is_fully_constructed = true;
}

// get stats on the circuit
Circuit::Stats Circuit::get_stats(void) const {
	Circuit::Stats result;
	result.nb_gates		 =(int)gates.size();
	result.nb_connections=(int)connections.size();
	result.nb_inputs	 =(int)inputs.size();
	result.nb_outputs	 =(int)outputs.size();
	return result;
}

// export to a string
std::string Circuit::export_to_string(void) const {
	//std::string result;
	std::ostringstream str_stream;
	export_to_stream(str_stream);
	return str_stream.str();
}
// export to a stream in the Bristol Fashion
// inspired from https://github.com/mcbagz/LogicGates/blob/main/Example.ipynb
// format : https://pypi.org/project/circuit/
// see also https://homes.esat.kuleuven.be/~nsmart/MPC/
void Circuit::export_to_stream(std::ostream& out) const {
	assert(is_fully_constructed);

	//# of gates + # of of wires
	out << std::to_string(gates.size()) << ' ' << std::to_string(connections.size());
	out << "\n";
	//# of inputs
	out << std::to_string(inputs.size());
	// inputs
	for (Connection* input_i : inputs) {
		out << " " << std::to_string(input_i->id);
	}
	out << "\n";
	//# of outputs
	out << std::to_string(outputs.size());
	// outputs
	for (Connection* output_i : outputs) {
		out << " " << std::to_string(output_i->id);
	}

	//  0 and 1 for litterals
	if (is_using_litteral_values())
	{
		// wire 0 is assigned the value 0 and wire 1 the value 1
		if (literals01[0]) {
			out << "\n" << "1 1 0 " << literals01[0]->id << " EQ";
		}
		if (literals01[1]) {
			out << "\n" << "1 1 1 " << literals01[1]->id << " EQ";
		}
	}
	
	// Gates
	for (Gate* gate_i : gates) {
		out << "\n";
		// # inputs 
		auto outs = gate_i->get_outputs();
		out << std::to_string(outs.size());
		for (Connection* out_i : outs) {
			out << " " << std::to_string(out_i->id);
		}
		// # outputs
		out << " ";
		auto ins = gate_i->get_inputs();
		out << std::to_string(outs.size());
		for (Connection* in_i : ins) {
			out << " " << std::to_string(in_i->id);
		}
		// gate type
		out << " " << gate_i->get_export_type();

	}

	out << "\n";
}

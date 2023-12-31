// Gates.cpp
// Implementation of the Circuit class.


#include <iostream>  
#include <sstream>  
#include "Circuit.h"
#include "Literal.h"

// constructor
Circuit::Circuit(void) {
}


// set the numbre of bits inputs of the circuit
void Circuit::set_circuit_inputs(int bit_count, InputsMap map) {
	assert(!is_fully_constructed); // too late to set inputs
	for (int i = 0; i < bit_count; i++) {
		Connection* input_i = new Connection();
		inputs.push_back(input_i);
	}
	correspondance_inputs = map;
}

// set connections ouput
void Circuit::set_output(std::vector<Connection*> new_outputs)
{
	assert(!is_fully_constructed);
	assert(outputs.size() == 0);
	assert(new_outputs.size() > 0);
	assert(output_size_child == 0);

	for (Connection* connexion : new_outputs) {
		connexion->is_output = true;
		outputs.push_back(connexion);
	}
}
// if the circuit have sub circuits (If statement for exemple), set the output size only
void Circuit::set_output_size_child(NbBit size) {
	assert(!is_fully_constructed);
	assert(outputs.size() == 0);
	assert(output_size_child == 0);
	assert(size > 0);

	output_size_child = size; 
}



// add a gate and its connexions into the circuit
void Circuit::add_gate(TapScriptGate* gate) {
	assert(!is_fully_constructed);
	// set debug info
	gate->debug_info = debug_info;
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
std::vector<TapScriptGate*> Circuit::_get_computable_gate(void) const {
	std::vector<TapScriptGate*> computable_gates;
	for (TapScriptGate* gate : gates) {
		if (    gate->all_inputs_calculated() 
			&& !gate->is_computed) {
			computable_gates.push_back(gate);
		}
	}
	return computable_gates;
}

// init the value of a variable
void CRunInputs::set_varaible_value(std::string name, std::string value) {

	assert(correspondance_inputs != nullptr);
	// look for the variable
	auto info_var = correspondance_inputs->find_info_by_name(name);
	if (!info_var.found)
		throw Error("Unknown variable name", name);
	// get the type of the variable
	Type *type = info_var.type;
	// muet be a basic type 
	if (!type->is_basic())
		throw Error("Not a basic type", name);
	Type::Native native_type = type->cast_to_TypeBasic()->get_native_type();
	
	// convert the value to  array of bit
	std::vector<bool> bits = Literal::get_bools_from_value_str(native_type, value);
	int nb_bit = type->size_in_bit();
	assert(nb_bit== bits.size());

	// set x bits of the value
	for (int i = 0; i < nb_bit; i++) {
		set_bit_value(info_var.offset_in_bit + i, bits[i]);
	}

}


// get inout of the circuit. to set values before running it
CRunInputs Circuit::get_run_inputs(void) const {
	assert(is_fully_constructed);
	CRunInputs result;
	result.init((int)inputs.size(), correspondance_inputs);
	return std::move(result);

	
}

// size in bits of the output
NbBit Circuit::nb_bits_output(void) const 
{
	// if the size comme from a sub circuit
	if (output_size_child != 0)
	{
		assert(outputs.size() == 0);
		return output_size_child;
	}
	return (NbBit)outputs.size();
}

// check if all outputs are computed
bool Circuit::_all_outputs_calculated(void) const {
	if (outputs.size() == 0)
		return false;

	for (Connection* output : outputs) {
		if (!output->is_calculated())
			return false;
	}
	return true;
}


// run the circuit
std::vector<Bit> Circuit::run(const CRunInputs& in_values) const {

	assert(nb_bits_output() > 0);
	//assert(in_values.size() > 0); no in sub circuit. ex : "Return 0"
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
		// if all outputs are computed, we have a result
		if (_all_outputs_calculated()) {
			// build the result
			std::vector<Bit> result(nb_bits_output());
			int i = 0;
			for (Connection* connection_i : outputs) {
				assert(connection_i->is_output);
				bool value_calculted = connection_i->get_value();
				result[i].set_value(value_calculted);
				i++;
			}
			return result;
		}

		// get gates that have calculated intpus
		std::vector<TapScriptGate*> gate_to_run = _get_computable_gate();
		// if no more gates to run, error
		if (gate_to_run.size() == 0) 
			throw Error( "internal error : no more gates" );

		// special case If gate
		if (gate_to_run.size() == 1 && gate_to_run[0]->cast_to_IF() != nullptr) {
			// run the if gate : a sub progrma depending on the value of the condition
			Gate_IF* gate_if = gate_to_run[0]->cast_to_IF();
			return gate_if->compute_if();
		}

		// run all the gates, but if gate
		for (TapScriptGate* gate : gate_to_run) {
			if (!gate->cast_to_IF()) {
				gate->compute();
				assert(gate->is_computed);
			}
		}

		// continue calculation
		nb_step++;
		
	} // while (true)

}

// reset the circuit gates before a new run. resursive to all  sub circuits
void Circuit::reset(void) const {
	_reset_non_recursive();
	// reset sub circuits
	visit_sub_circuits([](Circuit& sub_circiut) {
		sub_circiut._reset_non_recursive();
		});
}

// reset the circuit gate before a new run. non recursive
void Circuit::_reset_non_recursive(void) const
{
	for (TapScriptGate* gate : gates) {
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
void Circuit::init_id_gates_and_connexions(int &connection_id)
{
	// give an ID to all gates and all connections
	for (int i = 0; i < gates.size(); i++) {
		gates[i]->id = i;
	}
	// init literals
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
	result.nb_gate		 =(int)gates.size();
	result.nb_connection =(int)connections.size();
	result.nb_input	     =(int)inputs.size();
	result.nb_output	 =(int)outputs.size();
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
	for (TapScriptGate* gate_i : gates) {
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
// visit all sub circuits
void Circuit::visit_sub_circuits(std::function<void(Circuit&)> visitor) const {
	for (TapScriptGate* gate_i : gates) {
		// special case If gate
		if (gate_i->cast_to_IF()) {
			// cast to IF gate
			Gate_IF* gate_if = gate_i->cast_to_IF();
			// visit sub circuits of the IF gate
			visitor(*gate_if->circuit_if_true);
			gate_if->circuit_if_true->visit_sub_circuits(visitor);
			visitor(*gate_if->circuit_if_false);
			gate_if->circuit_if_false->visit_sub_circuits(visitor);

		}
	}

}
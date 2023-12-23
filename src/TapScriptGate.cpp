// implemeation of all TapScriptGate logic


#include "TapScriptGate.h"
#include "Circuit.h"

// ---- NAND ----

// compute the output of the NAND gate
void Gate_NAND::compute(void) {
	// compute the output of the NAND gate
	output[0]->set_value(!(input[0]->get_value() & input[1]->get_value()));
	is_computed = true;
}
// add the gate into the circuir
std::array<Connection*, 1> Gate_NAND::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _input) {
	circuit.add_gate(this);
	set_inputs(_input);
	return this->output;
}

// --- IF ----

// IGate redefinition
// get the inputs of the gate
std::vector<Connection*> Gate_IF::get_inputs(void) const {
	// IF gate need 1 input or the condition and MAX<B0,B1> inputs for the 2 possible sub circuits
	// the first input is the condition
	std::vector<Connection*> connections(input.begin(), input.end());
	// add the inputs of the 2 sub circuits
	for (Connection* cnx : input_for_circuit_if_true)
		connections.push_back(cnx);
	for (Connection* cnx : input_for_circuit_if_false) {
		// add only if not already present in connections
		bool already_present = false;
		for (auto c : connections) {
			if (cnx == c)
				already_present = true;
		}
		if (!already_present)
			connections.push_back(cnx);
	}

	// return the inputs
	return std::move(connections);
}

// declare a new input for the gate : to be used by the circuit if_true and/or circuit_if_false
void Gate_IF::add_input(Connection* input, bool if_true) {
	// get the good vector
	std::vector<Connection*>& inputs = if_true ? input_for_circuit_if_true : input_for_circuit_if_false;
	// do not add the same input twice
	for (int i = 0; i < inputs.size(); i++) {
		if (inputs[i] == input) {
			return;
		}
	}
	// add the input
	inputs.push_back(input);

}

// compute the output of the IF_ gate
void Gate_IF::compute(void)
{
	assert(false);

	throw Error("gate IF is not computable");
}

// compute the output of the If gate : one of the sub cirusit 
std::vector<Bit> Gate_IF::compute_if(void) const {
	bool if_true = input[0]->get_value();
	// get the circuit to compute
	Circuit& sub_circuit				   = if_true ? *circuit_if_true : *circuit_if_false;
	const std::vector<Connection*>& inputs = if_true ? input_for_circuit_if_true : input_for_circuit_if_false;

	// build in values for the circuit with all the other inputs
	//InputsMap input_map = (InterfaceInputsMap*)this;
	CRunInputs in_sub_values = sub_circuit.get_run_inputs();
	int i = 0;
	for (Connection* connection : inputs) {
		if (connection->is_calculated()) {
			bool bit_i = connection->get_value();
			in_sub_values.set_bit_value(i, bit_i);
		}
		i++;
	}

	// compute the sub-circuit
	std::vector<Bit> sub_result = sub_circuit.run(in_sub_values);
	assert(sub_result.size() > 0);
	return  sub_result;
}
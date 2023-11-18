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
	for (Connection* cnx : input_for_circuit)
		connections.push_back(cnx);
	// return the inputs
	return std::move(connections);
}

// compute the output of the IF_ gate
void Gate_IF::compute(void)
{
	throw Error("gate IF is not computable");
}
// compute the output of the If gate : one of the sub cirusit 
std::vector<Bit> Gate_IF::compute_if(void) const {
	bool input_val = input[0]->get_value();
	// get the circuit to compute
	Circuit& sub_circuit = input_val ? *circuit_if_true : *circuit_if_false;

	// build in values for the circuit with alll the other inputs
	CRunInputs in_sub_values;
	for (int i = 1; i < input.size(); i++) {
		bool bit_i = input[i]->get_value();
		in_sub_values.set_bit_value(i - 1, bit_i);
	}

	// compute the circuit
	return sub_circuit.run(in_sub_values);

}
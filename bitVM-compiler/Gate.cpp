// Gates.cpp
// Implemnetaions of the getas logic


#include "Gate.h"
#include "Circuit.h"

// compute the output of the NAND gate
void Gate_NAND::compute(void) {
	// compute the output of the NAND gate
	output[0]->set_value( !(input[0]->get_value() & input[1]->get_value()));
	is_computed = true;
}
// add the gate into the circuir
std::array<Connection*, 1> Gate_NAND::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _input)  {
	circuit.add_gate(this);
	set_inputs(_input);
	return this->output;
}

// add a NOT gate into the circuir
std::array<Connection*,1> Gate_NOT::add_to_circuit(Circuit& circuit, std::array<Connection*, 1>& _inputs)  {
	// implemented with a nand gate
	Gate_NAND* _nand = new Gate_NAND();
	std::array<Connection*, 2> inputs2 = { _inputs[0], _inputs[0] };
	return _nand->add_to_circuit(circuit, inputs2);
};


// compute the output of the AND gate
std::array<Connection*, 1> Gate_AND::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs) {
	// implemented with a nand + not
	Gate_NAND* _nand	 = new Gate_NAND();
	Gate_NAND* _nand_not = new Gate_NAND();
//	std::array<Connection*, 2> inputs12 = { _inputs[0], _inputs[1] };
	std::array<Connection*, 1> output_gate1 =  _nand->add_to_circuit(circuit, _inputs);
	std::array<Connection*, 2> output_gate2 = { output_gate1[0], output_gate1[0] };
	return _nand_not->add_to_circuit(circuit, output_gate2);
};
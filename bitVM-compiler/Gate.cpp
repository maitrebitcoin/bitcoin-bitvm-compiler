// Gates.cpp
// Implemnetaions of the getas logic


#include "Gate.h"
#include "Circuit.h"

// compute the output of the NAND gate
void Gate_NAND::compute(void) {
	// compute the output of the NAND gate
	output[0]->set_value( !(input[0]->get_value() & input[1]->get_value()));
}
// add the gate into the circuir
std::array<Connection*, 1> Gate_NAND::add_gate(Circuit& circuit, std::array<Connection*, 2>& _input)  {
	circuit.add_gate(this);
	set_inputs(_input);
	return this->output;
}

// add a NOT gate into the circuir
std::array<Connection*,1> Gate_NOT::add_gate(Circuit& circuit, std::array<Connection*, 1>& _inputs)  {
	// implemented with a nand gate
	Gate_NAND* _nand = new Gate_NAND();
	std::array<Connection*, 2> inputs2 = { _inputs[0], _inputs[0] };
	return _nand->add_gate(circuit, inputs2);
};

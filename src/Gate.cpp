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
	std::array<Connection*, 1> output_gate1 =  _nand->add_to_circuit(circuit, _inputs);
	std::array<Connection*, 2> output_gate2 = { output_gate1[0], output_gate1[0] };
	return _nand_not->add_to_circuit(circuit, output_gate2);
};

// compute the output of the OR gate
std::array<Connection*, 1> Gate_OR::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs)
{
	// implemented with r = not(a) nand not(b)
	Gate_NAND* _nand_a = new Gate_NAND();
	Gate_NAND* _nand_b = new Gate_NAND();
	Gate_NAND* _nand_r = new Gate_NAND();
	std::array<Connection*, 2> inputs_a = { _inputs[0], _inputs[0] };
	std::array<Connection*, 2> inputs_b = { _inputs[1], _inputs[1] };
	std::array<Connection*, 1> not_a = _nand_a->add_to_circuit(circuit, inputs_a);
	std::array<Connection*, 1> not_b = _nand_b->add_to_circuit(circuit, inputs_b);
	std::array<Connection*, 2> not_ab = { not_a[0], not_b[0] };
	return _nand_r->add_to_circuit(circuit, not_ab);

}

// compute the output of the XOR gate
std::array<Connection*, 1> Gate_XOR::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs)
{
	// implemented with 4 nand gates
	Gate_NAND* _nand_1 = new Gate_NAND();
	Gate_NAND* _nand_2 = new Gate_NAND();
	Gate_NAND* _nand_3 = new Gate_NAND();
	Gate_NAND* _nand_4 = new Gate_NAND();
	// nand(a, b)
	std::array<Connection*, 2> inputs_1 = { _inputs[0], _inputs[1] };
	std::array<Connection*, 1> nand_ab = _nand_1->add_to_circuit(circuit, inputs_1);
	// nand(a, nand(a, b))
	std::array<Connection*, 2> inputs_2 = { _inputs[0], nand_ab[0] };
	std::array<Connection*, 1> nand_a_nand_ab = _nand_2->add_to_circuit(circuit, inputs_2);
	// nand(b, nand(a, b))
	std::array<Connection*, 2> inputs_3 = { _inputs[1], nand_ab[0] };
	std::array<Connection*, 1> nand_b_nand_ab = _nand_3->add_to_circuit(circuit, inputs_3);
	// result
	std::array<Connection*, 2> inputs_4 = { nand_a_nand_ab[0], nand_b_nand_ab[0] };
	std::array<Connection*, 1> xor_ab = _nand_4->add_to_circuit(circuit, inputs_4);

	return  xor_ab;

}
// compute the output of the XNOR gate
std::array<Connection*, 1> Gate_XNOR::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs)
{
	// !(a xor b)
	std::array < Connection*, 1> xor_ab = Gate_XOR().add_to_circuit(circuit, _inputs);
	// !
	return Gate_NOT().add_to_circuit(circuit, xor_ab);
}
// (r,carry) = a + b
// compute the output of the gate
std::array<Connection*, 2> Gate_ADD::add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs) {

	std::array<Connection*, 2> _ab = { _inputs[0], _inputs[1] };
	std::array<Connection*, 1> a_xor_b = Gate_XOR().add_to_circuit(circuit, _ab);
	std::array<Connection*, 1> a_and_b = Gate_AND().add_to_circuit(circuit, _ab);
	// low   = a xor b
	// carry = a and b
	return { a_xor_b[0], a_and_b[0] };
}

// full adder
// (r,carry) = a + b + carry_in 
// compute the output of the gate
std::array<Connection*, 2> Gate_ADDC::add_to_circuit(Circuit& circuit, std::array<Connection*, 3>& _inputs) {
	// easy implentation wirh 2 half adders and an or gate
	// source : https://www.geeksforgeeks.org/full-adder-in-digital-logic/
	std::array<Connection*, 2> _ab = { _inputs[0], _inputs[1] };
	std::array<Connection*, 2> add_ab = Gate_ADD().add_to_circuit(circuit, _ab);
	std::array<Connection*, 2> _c_add_ab = { _inputs[2], add_ab[0] };
	std::array<Connection*, 2> add_c_addab = Gate_ADD().add_to_circuit(circuit, _c_add_ab);
	// carry from the 2 addders
	std::array<Connection*, 2> _2c = { add_ab[1], add_c_addab[1] };
	std::array<Connection*, 1> c_out = Gate_OR().add_to_circuit(circuit, _2c);

	// low   = a + b + c
	return { add_c_addab[0], c_out[0] };
}


// add the gate into the circuir
std::array<Connection*, 0> Gate_IF::add_to_circuit(Circuit& circuit, std::array<Connection*, 1>& _input)
{
	circuit.add_gate(this);
	set_inputs(_input);
	// no raal output
	std::array<Connection*, 0> output_void = {};
	return output_void;

}

// declare a new input for the gate : to be used by the circuit if_true and/or circuit_if_false
void Gate_IF::add_input(Connection* input) {
	
	// do not add the same input twice
	for (int i = 0; i < input_for_circuit.size(); i++) {
		if (input_for_circuit[i] == input) {
			return;
		}
	}
	// add the input
	input_for_circuit.push_back(input);

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



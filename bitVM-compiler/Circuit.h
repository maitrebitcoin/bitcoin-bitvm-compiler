#pragma once
#include <vector>
#include "Bits.h"
#include "Gate.h"

class Gate;
class Circuit;

class CInputs : public std::vector <Bit>
{
public:
	// add 1 bit
	void add_bit(bool b) {
		Bit bit;
		bit.set_value(b);
		this->push_back(bit);
	}
};

// represents a virtual circuit
class Circuit {
private:
	// number of bits for the inputs
	//int nb_bits_inputs = 0;
	// number of bits for the outputs
	int nb_bits_output = 0;
	// inputs in the circuit
	std::vector<Connection*>  inputs;
	// gates in the circuit
	std::vector<Gate*> gates;
	// connections in the circuit
	std::vector<Connection*> connections;



public:
	// set the nimbre of bits inputs of the circuit
	void set_inputs(int bit_count);
	// add a gate into the circuit
	void add_gate(Gate* gate);
	// add connections ouput
	void add_output(std::vector<Connection*> outputs);
	// get the input gates in the circuit
	std::vector<Connection*> getInputs(void) { return inputs; }

	// run the circuit
	std::vector<Bit> run(const CInputs& inputs) const;

protected:
	// get all garbes that have calculated inputes
	std::vector< Gate*> _get_computable_gate(void) const;

};
#pragma once

#include <array>
#include "Connection.h"
class Circuit;

// represents the base class of any logical gatet, basic or composited
class IGate {

public:
	// get the inputs of the gate
	virtual std::vector<Connection*> get_inputs(void) const = 0;
	// get the outputs of the gate
	virtual std::vector<Connection*> get_outputs(void) const = 0;

}; 


// tamplate that represents a gate with N inputs and M outputs
// GateBaseClass must be either TapScriptGate or CompositedGate
template <int N, int M, class GateBaseClass>
class T_NMGate : public GateBaseClass
{
protected:
	// inputs and outputs of the gate
	std::array<Connection*, N> input;
	std::array<Connection*, M> output;

public:
	// constructor
	T_NMGate(void) {
		_init_output();
	}
	// init in connexions ( from others gates )
	void set_inputs(std::array<Connection*, N>& input) {
		this->input = input;
	}

	// get the outputs of the gate
	virtual std::vector<Connection*> get_outputs(void) const override {
		std::vector<Connection*> connections(output.begin(), output.end());
		return std::move(connections);
	}
	// get the inputs of the gate
	virtual std::vector<Connection*> get_inputs(void) const override {
		std::vector<Connection*> connections(input.begin(), input.end());
		return std::move(connections);
	}

	// to bez implemented by the derived class :

	// add a gate into the circuir
	virtual std::array<Connection*, M> add_to_circuit(Circuit& circuit, std::array<Connection*, N>& _input) = 0;

protected:
	// init out connexions
	void _init_output(void) {
		for (int i = 0; i < M; i++) {
			output[i] = new Connection();
		}
	}
}; //T_NMGate
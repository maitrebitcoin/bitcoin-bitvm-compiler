#pragma once

#include <array>
#include <vector>
#include <string>
#include "Gate.h"
#include "Connection.h"
class Circuit;

// represents a gate in the circuit
class Gate {
public:
	// is the gates used for computation ?
	bool is_computed = false;
	// id of the gate, for export
	int id = 0;

public:
	// compute the output of the gate
	virtual void compute(void) = 0;

	// check if all the inputs of the gate are calculated
	virtual bool all_inputs_calculated(void) const = 0;
	// get the inputs of the gate
	virtual std::vector<Connection*> get_inputs(void) const = 0;
	// get the outputs of the gate
	virtual std::vector<Connection*> get_outputs(void) const = 0;
	// export name of the gate. ex : "NAND"
	virtual std::string get_export_type(void) const = 0;

};



// tamplate that represents a gate with N inputs and M outputs
template <int N, int M>
class T_NMGate : public Gate
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
	// check if all the inputs of the gate are calculated
	virtual bool all_inputs_calculated(void) const override {
		for (int i = 0; i < N; i++) {
			if (!input[i]->is_calculated()) {
				return false;
			}
		}
		return true;
	}
	// get the outputs of the gate
	virtual std::vector<Connection*> get_outputs(void) const override {
		std::vector<Connection*> connections( output.begin(), output.end());
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

// template that represents a basic gate that can be implemented in the circuit
// a gate that can be implemented in a circuit = there is a bitcoin taproot script that can implement it
template <int N, int M>
class T_BasiceGate : public T_NMGate<N,M> {
	// export name of the gate. ex : "NAND"
	virtual std::string get_export_type(void) const = 0;

};

// template that represents a complexe gate, implemented with other T_ImplementableGate and T_ComplexeGate
template <int N, int M>
class T_CompositeeGate { // : public T_NMGate < N, M> {
public:
	// add a gate into the circuir
	virtual std::array<Connection*, M> add_to_circuit(Circuit& circuit, std::array<Connection*, N>& _input) = 0;
	// export name of the gate : error if called
	virtual std::string get_export_type(void) const { assert(false); return ""; }
};

// r = nand(a,b)
class Gate_NAND : public T_BasiceGate < 2, 1 >
{

public:
	// compute the output of the NAND gate
	virtual void compute(void) override;
	// add the gate into the circuir
	virtual std::array<Connection*, 1> add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _input) override;
	// export name of the gate. ex : "NAND"
	virtual std::string get_export_type(void) const override { return "NAND"; }

};

class UnaryGate : public T_CompositeeGate<1, 1> {};
// r = !a
class Gate_NOT : public UnaryGate
{
public:
	// compute the output of the NOT gate
	virtual std::array<Connection*, 1> add_to_circuit(Circuit& circuit, std::array<Connection*, 1>& _inputs) override;
};
class BinaryGate : public T_CompositeeGate<2, 1> {};
// r = a & b
class Gate_AND : public BinaryGate
{
public:
	// compute the output of the AND gate
	virtual std::array<Connection*, 1> add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs) override;
};
// r = a | b
class Gate_OR : public BinaryGate
{
public:
	// compute the output of the OR gate
	virtual std::array<Connection*, 1> add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs) override;
};
// r = a ^ b
class Gate_XOR : public BinaryGate
{
public:
	// compute the output of the OR gate
	virtual std::array<Connection*, 1> add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs) override;
};

// (r,carry) = a + b
class Gate_ADD : public T_CompositeeGate<2, 2>
{
public:
	// compute the output of the OR gate
	virtual std::array<Connection*, 2> add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs) override;
};
// full adder
// (r,carry) = a + b + c
class Gate_ADDC : public T_CompositeeGate<3, 2>
{
public:
	// compute the output of the gate
	virtual std::array<Connection*, 2> add_to_circuit(Circuit& circuit, std::array<Connection*, 3>& _inputs) override;
};

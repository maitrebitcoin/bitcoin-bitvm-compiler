#pragma once

#include <string>
#include "IGate.h"
#include "InterfaceInputsMap.h"
class Circuit;

// debug info for a gate
class CDebugInfo {
public:
	// source file name line number
	int source_line = 0;
	// description for debug purpose
	std::string description;
};

// represents a gate that can be implemented in the circuit
// ie, can bien represented with a tapscript
class TapScriptGate : public IGate {
public:
	// description for debug purpose
	CDebugInfo debug_info;
	// is the gates used for computation ?
	bool is_computed = false;
	// id of the gate, for export
	int id = 0;

public:
	// compute the output of the gate
	virtual void compute(void) = 0;
	// check if all the inputs of the gate are calculated
	virtual bool all_inputs_calculated(void) const = 0;
	// export name of the gate. ex : "NAND"
	virtual std::string get_export_type(void) const = 0;
	// cast to special if Gate
	virtual	class Gate_IF* cast_to_IF(void) { return nullptr; }

};
// template to build N/M gates
template <int N, int M>
class T_TapScriptGate : public T_NMGate< N, M, TapScriptGate>
{
public:
	// check if all the inputs of the gate are calculated
	virtual bool all_inputs_calculated(void) const override {
		for (int i = 0; i < N; i++) {
			if (!T_NMGate< N, M, TapScriptGate>::input[i]->is_calculated()) {
				return false;
			}
		}
		return true;
	}
};

// r = nand(a,b)
class Gate_NAND : public T_TapScriptGate<2, 1>
{

public:
	// compute the output of the NAND gate
	virtual void compute(void) override;
	// add the gate into the circuir
	virtual std::array<Connection*, 1> add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _input) override;
	// export name of the gate. ex : "NAND"
	virtual std::string get_export_type(void) const override { return "NAND"; }

};

// spécial gate : If
// jump to other cirucuit depending on the value of the input
class Gate_IF : public T_TapScriptGate<1, 0>
{
public:
	Circuit* circuit_if_true = nullptr;
	Circuit* circuit_if_false = nullptr;
	std::vector<Connection*> input_for_circuit_if_true;
	std::vector<Connection*> input_for_circuit_if_false;

public:

	Gate_IF(Circuit* if_true, Circuit* if_false) : circuit_if_true(if_true), circuit_if_false(if_false) {}
	// declare a new input for the gate : to be used by the circuit if_true and/or circuit_if_false
	void add_input(Connection* input, bool if_true);

	// IGate redefinition
	// get the inputs of the gate
	virtual std::vector<Connection*> get_inputs(void) const override;

	// compute the output of the If  gate : error if called
	virtual void compute(void) override;
	// compute the output of the If gate : one of the sub cirusit 
	std::vector<Bit> compute_if(void) const;
	// add the gate into the circuir
	virtual std::array<Connection*, 0> add_to_circuit(Circuit& circuit, std::array<Connection*, 1>& _input) override;
	// export name of the gate. ex : "NAND"
	virtual std::string get_export_type(void) const override { return "IF"; }
	// cast to special if Gate
	virtual	class Gate_IF* cast_to_IF(void) { return this; }

};



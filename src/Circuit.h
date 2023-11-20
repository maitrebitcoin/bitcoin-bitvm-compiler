#pragma once
#include <vector>
#include "Bits.h"
#include "Gate.h"
#include "Error.h"
#include "Function.h"
#include "InterfaceInputsMap.h"

class TapScriptGate;
class Circuit;
class CRunInputs;


using InputsMap = InterfaceInputsMap*;
using NbBit = int;

// represents a virtual circuit
class Circuit {
private:
	// circuit id
	int id = 0;
	// inputs in the circuit
	std::vector<Connection*>  inputs;
	// gates in the circuit
	std::vector<TapScriptGate*> gates;
	// ouputs of the circuit
	std::vector<Connection*>  outputs;
	// if the circuit have sub circuits (If statement for exemple), set the output size only
	NbBit output_size_child = 0;

	// connections in the circuit
	std::vector<Connection*> connections;
	// litteral values : 0 an 1
	std::array<Connection*,2>  literals01;

	// set to true when the circuit is connot be modified anymore
	bool is_fully_constructed = false;

	// map for input correspondance 
	InputsMap correspondance_inputs = nullptr;

public:

	// constructor
	Circuit(void);

	// set the number of bits inputs of the circuit
	void set_circuit_inputs(int bit_count, InputsMap input_map);
	// add a gate into the circuit
	void add_gate(TapScriptGate* gate);
	// set all connections ouput
	void set_output(std::vector<Connection*> new_outputs);
	// a get connection for a litteral values
	Connection* get_literal_values(bool b);
	// get the input gates in the circuit
	std::vector<Connection*> getInputs(void) { return inputs; }
	// is the circuit usign litteral values ?
	bool is_using_litteral_values(void) const;

	// if the circuit have sub circuits (If statement for exemple), set the output size only
	void set_output_size_child(NbBit size) { output_size_child = size; }
	// size in bits of the output
	NbBit nb_bits_output(void) const;

	// get inout of the circuit. to set values before running it
	CRunInputs get_run_inputs(void) const;
	// run the circuit
	std::vector<Bit> run(const CRunInputs& inputs) const;
	// reset the circuit gate before a new run
	void reset(void) const;
	// set id fro the ciruit
	void set_id(int id) { this->id = id; }
	// init gates and connections ID
	void init_id_gates_and_connexions(int& connection_id);

	// export to a string
	std::string export_to_string(void) const;
	// export to a strem
	// inspired from https://github.com/mcbagz/LogicGates/blob/main/Example.ipynb
	// format : https://pypi.org/project/circuit/
	void export_to_stream(std::ostream& out) const;
	// get stats
	struct Stats {
		int nb_gate = 0;
		int nb_connection = 0;
		int nb_input = 0;
		int nb_output = 0;
	};
	Stats get_stats(void) const;

protected:
	// get all gates that have calculated inputes
	std::vector<TapScriptGate*> _get_computable_gate(void) const;
	// check if all outputs are computed
	bool _all_outputs_calculated(void) const;
};

class CRunInputs : public std::vector <Bit>
{
	friend Circuit;

protected:
	// map for inoput correspondance 
	InputsMap correspondance_inputs = nullptr;

private:
	// construstor, for Circuit ad gate_If only
	CRunInputs(void) {};
	// init inpute
	void init(int nb_bit, InputsMap _inputs) {
		// init the vector
		for (int i = 0; i < nb_bit; i++)
			this->push_back(Bit());
		correspondance_inputs = _inputs;
	}

public:
	// init the value of a variable
	void set_varaible_value(std::string name, std::string value);

	// set a 1 bit value
	void set_bit_value(int numvar, bool b) {
		assert(numvar < (int)this->size());
		// set the value
		(*this)[numvar].set_value(b);
	}
	// set the value of a 8 bit var
	void set_int8_value(int numvar, std::vector<bool> bits) {
		assert(numvar * 8 < (int)this->size());
		assert(bits.size() == 8);
		// set 8 bits of the value
		for (int i = 0; i < 8; i++)	{
			set_bit_value(numvar * 8 + i, bits[i]);
		}
	}
	// set the value of a 32 bit var
	void set_int32_value(int numvar, std::vector<bool> bits) {
		assert(numvar * 8 < (int)this->size());
		assert(bits.size() == 32);
		// set 8 bitsd of the value
		for (int i = 0; i < 32; i++) {
			set_bit_value(numvar * 32 + i, bits[i]);
		}
	}
	// set the value of a 64 bit var
	void set_int64_value(int numvar, std::vector<bool> bits) {
		assert(numvar * 8 < (int)this->size());
		assert(bits.size() == 64);
		// set 8 bitsd of the value
		for (int i = 0; i < 64; i++) {
			set_bit_value(numvar * 64 + i, bits[i]);
		}
	}
	// set the value of a 256 bit var
	void set_int256_value(int numvar, std::vector<bool> bits) {
		assert(numvar * 8 < (int)this->size());
		assert(bits.size() == 256);
		// set 8 bitsd of the value
		for (int i = 0; i < 256; i++) {
			set_bit_value(numvar * 256 + i, bits[i]);
		}
	}
};

#pragma once
#include <vector>
#include "Bits.h"
#include "Gate.h"
#include "Error.h"


class Gate;
class Circuit;
class CRunInputs;


// represents a virtual circuit
class Circuit {
private:
	// inputs in the circuit
	std::vector<Connection*>  inputs;
	// gates in the circuit
	std::vector<Gate*> gates;
	// ouputs of the circuit
	std::vector<Connection*>  outputs;
	// connections in the circuit
	std::vector<Connection*> connections;
	// litteral values : 0 an 1
	std::array<Connection*,2>  literals01;

	// set to true when the circuit is connot be modified anymore
	bool is_fully_constructed = false;

public:

	// constructor
	Circuit(void);

	// set the nimbre of bits inputs of the circuit
	void set_inputs(int bit_count);
	// add a gate into the circuit
	void add_gate(Gate* gate);
	// set all connections ouput
	void set_output(std::vector<Connection*> new_outputs);
	// a get connection for a litteral values
	Connection* get_literal_values(bool b);
	// get the input gates in the circuit
	std::vector<Connection*> getInputs(void) { return inputs; }
	// is the circuit usign litteral values ?
	bool is_using_litteral_values(void) const;

	// size in bits of the output
	int nb_bits_output(void) const { return (int)outputs.size(); }

	// get inout of the circuit. to set values before running it
	CRunInputs get_run_inputs(void) const;
	// run the circuit
	std::vector<Bit> run(const CRunInputs& inputs) const;
	// reset the circuit gate before a new run
	void reset(void) const;
	// init gates and connections ID
	void init_id(void);

	// export to a string
	std::string export_to_string(void) const;
	// export to a strem
	// inspired from https://github.com/mcbagz/LogicGates/blob/main/Example.ipynb
	// format : https://pypi.org/project/circuit/
	void export_to_stream(std::ostream& out) const;
	// get stats
	struct Stats {
		int nb_gates = 0;
		int nb_connections = 0;
		int nb_inputs = 0;
		int nb_outputs = 0;
	};
	Stats get_stats(void) const;

protected:
	// get all garbes that have calculated inputes
	std::vector< Gate*> _get_computable_gate(void) const;

};

class CRunInputs : public std::vector <Bit>
{
	friend Circuit;

private:
	// construstor, for Circuit only
	CRunInputs(void) {};
	// init inpute
	void init(int nb_bit) {
		// init the vector
		for (int i = 0; i < nb_bit; i++)
			this->push_back(Bit());
	}

public:
	// set a 1 bit value
	void set_bit_value(int numvar, bool b) {
		assert(numvar < (int)this->size());
		// set the value
		(*this)[numvar].set_value(b);
	}
	// set the value of a 8 bit var
	void set_byte_value(int numvar, std::vector<bool> bits) {
		assert(numvar * 8 < (int)this->size());
		assert(bits.size() == 8);
		// set 8 bitsd of the value
		for (int i = 0; i < 8; i++)	{
			set_bit_value(numvar * 8 + i, bits[i]);
		}
	}

};

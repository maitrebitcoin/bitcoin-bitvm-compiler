#pragma once

#include <vector>
#include <assert.h>
#include "Bits.h"
class Gate;


// represents a connection in the circuit betwen 2 gates
class Connection {
public:
	// gates that set the bit value
	Gate* gate_out = nullptr;
	// gates that use the bit value a their input
	std::vector<Gate*> tab_gate_in;
	// set if this is an output of the circuit
	bool is_output = false;
	int  n_ouput_index = 0;
private:
	// value in the connexion, during calculation only
	Bit* bit_value = nullptr;

public:
	bool is_calculated(void) const;
	// get the value of the bit
	bool get_value(void) const;
	// set the value of the bit
	void set_value(bool b);
	// rest the value of the bit for a new run
	void reset(void);

};

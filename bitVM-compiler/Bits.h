#pragma once
#include <vector>
#include <assert.h>
class Gate;


// represents a bit in the circuit 
class Bit {
private:
	bool value = false;
public:
	// get the value of the bit : overrides the cast to bool operator
	operator bool() const {
		return value;
	}
	// set the value of the bit
	void set_value(bool b) {
		value = b;
	}
};
// arrays of bits
using Bits = std::vector<Bit*>;


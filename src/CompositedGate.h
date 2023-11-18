#pragma once

#include "IGate.h"

// represent a gate implementd with other base gates. ex : a+b
class CompositedGate : public IGate {
public:

};
// template to build N/M composite gates
template <int N, int M>
class T_CompositedGate : public T_NMGate< N, M, CompositedGate>
{
};



class UnaryGate : public T_CompositedGate<1, 1> {};
// r = !a
class Gate_NOT : public UnaryGate
{
public:
	// compute the output of the NOT gate
	virtual std::array<Connection*, 1> add_to_circuit(Circuit& circuit, std::array<Connection*, 1>& _inputs) override;
};
class BinaryGate : public T_CompositedGate<2, 1> {};
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
// r = (a == b)
class Gate_XNOR : public BinaryGate
{
public:
	// compute the output of the OR gate
	virtual std::array<Connection*, 1> add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs) override;
};

// (r,carry) = a + b
class Gate_ADD : public T_CompositedGate<2, 2>
{
public:
	// compute the output of the OR gate
	virtual std::array<Connection*, 2> add_to_circuit(Circuit& circuit, std::array<Connection*, 2>& _inputs) override;
};
// full adder
// (r,carry) = a + b + c
class Gate_ADDC : public T_CompositedGate<3, 2>
{
public:
	// compute the output of the gate
	virtual std::array<Connection*, 2> add_to_circuit(Circuit& circuit, std::array<Connection*, 3>& _inputs) override;
};





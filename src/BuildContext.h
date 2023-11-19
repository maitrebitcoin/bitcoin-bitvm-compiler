#pragma once

#include <vector>
#include <functional>
#include "VariableDefinition.h"
#include "ScopeVariable.h"

class Circuit;
class ScopeVariables;
class Connection;

// context for building the circuit
class BuildContext {


public:
	Circuit &circuit; // the circuit to build
	std::vector<Circuit*> sub_circuits; // sub circuits created pour if/loop/prorcedure
	ScopeVariables variables; // current known variables in the current scope

public:
	// constructor
	BuildContext(Circuit& c) : circuit(c) {}

	// visit all the circuits
	void visit_circuits(std::function<void (Circuit&)> fnVisit);

	// get a new sub circuit
	Circuit& get_new_sub_circuit(void);

};


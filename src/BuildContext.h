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
	Circuit *ctx_circuit = nullptr; // the circuit to build
	ScopeVariables variables; // current known variables in the current scope

public:
	// creator on copy coller type
	enum class Caller { main_body, if_statement, for_statement };
	// constructor
	BuildContext(Caller caller);
	// copy constructor
	BuildContext(const BuildContext& source, Caller caller);

	// get the current circuit to build in the context
	Circuit& circuit(void) { return *ctx_circuit; }
	// visit all the circuits
	void visit_circuits(std::function<void(Circuit&)> fnVisit);

};


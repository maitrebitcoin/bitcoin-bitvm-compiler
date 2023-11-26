#pragma once

#include <vector>
#include <functional>
#include "VariableDefinition.h"
#include "ScopeVariable.h"
#include "Statement.h"

class Circuit;
class ScopeVariables;
class Connection;
class Statement_For;

// context for building the circuit
class BuildContext {
public:
	// creation or copy coller type
	enum class Caller { main_body, if_statement, for_statement };


public:
	Caller create_caller;
	Circuit *ctx_circuit = nullptr; // the circuit to build
	ScopeVariables variables; // current known variables in the current scope
	Statement_For * for_statement = nullptr; // the current for statement if we are building a loop
	// action to do to build alls next statementq
	std::function<Statement::NextAction (BuildContext&)> all_next_statements_builder;

public:
	// constructor
	BuildContext(Caller caller);
	// copy constructor
	BuildContext(const BuildContext& source, Caller caller);

	// get the current circuit to build in the context
	Circuit& circuit(void) { return *ctx_circuit; }
	// visit all the circuits
	void visit_circuits(std::function<void(Circuit&)> fnVisit);
	// are we in a for statement ?
	bool is_in_for_loop(void) const { return create_caller == Caller::for_statement; }

};


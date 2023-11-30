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
class IVisitExpression;
class InterfaceInputsMap;
using InputsMap = InterfaceInputsMap*;

// context for building the circuit
class BuildContext {
public:
	// creation or copy coller type
	enum class Caller { main_body, if_statement, for_statement, build_next
	};

public:
	Caller create_caller;
	Circuit *ctx_circuit = nullptr; // the circuit to build
	ScopeVariables variables; // current known variables in the current scope
	Statement_For * for_statement = nullptr; // the current for statement if we are building a loop
	// action to do to build all next statementq
	std::function<Statement::NextAction (BuildContext&)> build_all_next_statements;

public:
	// constructor
	BuildContext(Caller caller);
	// copy constructor
	BuildContext(const BuildContext& source, Caller caller);
	//  get all info needed to create a new copy of acontexte; for "if statement" 
	class InfoCopy {
	public:
		ScopeVariables variables_dest; // copy of all variables t
		std::vector<Connection*> connexions_dest; // connexion in the copied var
		InputsMap input_map = nullptr; // map for connexions_dest
	public:
		// number of bits in the new context
		int nb_bits_in(void) const { return (int)connexions_dest.size(); }
	};
	InfoCopy get_info_copy(void) const;

	// get the current circuit to build in the context
	Circuit& circuit(void) { return *ctx_circuit; }
	// visit all the circuits
	void visit_circuits(std::function<void(Circuit&)> fnVisit);
	// are we in a for statement ?
	bool is_in_for_loop(void) const { return for_statement != nullptr; }

};


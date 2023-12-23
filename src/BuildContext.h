#pragma once

#include <vector>
#include <functional>
#include "VariableDefinition.h"
#include "ScopeVariable.h"


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
	// description for debug purpose
	std::string debug_description;
	// current statement pour debug purpose
	class Statement* current_statement = nullptr;

	// creation or copy coller type
	enum class Caller { main_body, if_statement, for_statement, build_next_lambda};
	// action return, what to do to build all next statements
	enum class NextAction { Continue, Break, Return };
	// number of nested if
	int nested_if = 0;

protected:
	Caller create_caller;
	Circuit* ctx_circuit = nullptr; // the circuit to build
	ScopeVariables ctx_variables; // current known variables in the current scope
	ScopeVariables* parent_ctx_variables= nullptr; // variables ftom the parent context


public:
	Statement_For* for_statement = nullptr; // the current for statement if we are building a loop

	// action to do to build all next statementq
	std::function < NextAction(BuildContext&) > build_all_next_statements;
	// action to do in case a "break" is encountered
	std::function < NextAction(BuildContext&) > build_on_break;

public:
	// constructor
	BuildContext(Caller caller);
	// copy constructor
	BuildContext(const BuildContext& source, Caller caller);
	// destructor
	~BuildContext();

	//  Init variables and If Gate for a "IF"  statmeent
	void init_variables_if_gate(BuildContext& ctx_source, class Gate_IF* gate, bool bloc_side);

	// get the current circuit to build in the context
	Circuit& circuit(void) { return *ctx_circuit; }
	// get the variables in the contexte
	ScopeVariables& variables(void) const;
	// visit all the circuits
	void visit_circuits(std::function<void(Circuit&)> fnVisit);
	// are we in a for statement ?
	bool is_in_for_loop(void) const { return for_statement != nullptr; }

protected:
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
};


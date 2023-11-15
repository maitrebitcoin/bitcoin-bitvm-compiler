#pragma once

#include "Scope.h"
class Statement;
class Function;
class BuildContext;

class CodeBloc : public Scope {
public:
	// code statements
	std::vector<Statement*> statements;

public:
	// constructor
	CodeBloc(Statement* first_statement) { statements.push_back(first_statement); }
	// add a statement
	void add_statement(Statement* s) { statements.push_back(s); }
	// init a bloc
	void init( Scope& global_scope);
	// build a circuit that represents the bloc
	void build_circuit(BuildContext& ctx);
	// get the return statement of the bloc
	class Statement_Return* get_return_statement(void) const;

};
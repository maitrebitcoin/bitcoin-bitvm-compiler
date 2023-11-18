#pragma once

#include <functional>
#include "Scope.h"
class Statement;
class Function;
class BuildContext;
class Expression_Variable;
class IVariableToConnexion;

class CodeBloc : public Scope {
public:
	// code statements
	std::vector<Statement*> statements;

public:
	// constructors
	CodeBloc(void) {} // code reorg  for if statement
	CodeBloc(Statement* first_statement) { statements.push_back(first_statement); } // parsing
	// add a statement
	void add_statement(Statement* s) { statements.push_back(s); }
	// init a bloc
	void init( Scope& global_scope);
	// build a circuit that represents the bloc
	void build_circuit(BuildContext& ctx);
	// get the return statement of the bloc
	class Statement_Return* get_return_statement(void) const;
	// visit all variables connexion used in the bloc
	void visit_all_used_variable(std::function<void(IVariableToConnexion& var2cnx)>);

protected:
	// reoorganize the bloc in case of "If"
	void _reorganize_bloc_if_statement(void);
};
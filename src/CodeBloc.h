#pragma once

#include <functional>
#include "Scope.h"
#include "Statement.h"

class Function;
class BuildContext;
class Expression;
class Expression_Variable;
class IVisitExpression;

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
	// init a bloc, extentend version used in "for" statement
	enum class InitOption { return_must_be_present, return_or_break_must_be_present, return_not_required };
	void init_ex(Scope& global_scope, InitOption option);
	// build a circuit that represents the bloc
	BuildContext::NextAction build_circuit(BuildContext& ctx);
	// get the return statement of the bloc
	class Statement_Return* get_return_statement(void) const;
	// visit all part used in the bloc
	void visit_all_epressions(IVisitExpression& visitor);
	// visit part used in the bloc, fram a given statement
	void visit_all_epressions_from(IVisitExpression& visitor, int statement_index);
	// visit all expression used in all the statement
	void visit_Expression(std::function<void(Expression& expr)> visitor) const;

protected:
	// build a circuit that represents the bloc, from a statement index
	BuildContext::NextAction _build_circuit_from(BuildContext& ctx, int first_statement_index);
	// reoorganize the bloc in case of "If"
	void _reorganize_bloc_if_statement(void);
};
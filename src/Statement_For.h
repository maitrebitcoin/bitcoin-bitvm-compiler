// the for() statement

#pragma once
#include "Statement.h"

class CodeBloc;
class Expression;
// "if" statement
class Statement_For : public Statement {
protected:
	// the 3 parts of the for statement
	Statement*  for_init      = nullptr; // ex : int i=0
	Expression* for_condition = nullptr; // ex : i<10
	Statement*	for_increment = nullptr; // ex : i++
	// code bloc executed in the loop
	CodeBloc* code = nullptr;

 	// start value
	int start_value = 0;
	// en value
	int end_value = 0;
	// increment value 1 ou -1
	int incr_value = 1;

public:
	// constructor
	Statement_For(int line, Statement* init, Expression* condition, Statement* increment, CodeBloc* code);

	// visit all expression ueed in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override;
	// init for statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the for statement
	virtual void build_circuit(BuildContext& ctx) const override;

};

// the break statement 
// in for() loop only

#pragma once
#include "Statement.h"

class Statement_Break : public Statement {

public:
	// constructor
	Statement_Break(int line) : Statement(line) {}
	// cast to Statement_Break
	virtual	class Statement_Break* cast_to_Statement_Break(void) override { return this; }

	// visit all expression ueed in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override {};
	// init statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit 
	virtual NextAction build_circuit(BuildContext& ctx) const override;

};


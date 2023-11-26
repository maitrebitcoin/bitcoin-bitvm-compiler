// var++ statement

#pragma once
#include "Statement.h"

class Statement_Increment : public Statement {
public:
	// name of the variable
	std::string var_name;
	bool is_increment = true; // true: ++ / false --
	// inter expression usse to calculate ++ or --
	Expression * incr_expression = nullptr;
	// type of the variable
	Type* type = nullptr;
	// 1 vith the same type as the variable
	Literal* _1 = nullptr;

public:
	// constructor
	Statement_Increment(int line, std::string name, bool increment);
	// cast to Statement_Increment
	virtual	Statement_Increment* cast_to_Statement_Increment(void) override { return this; }

	// get return type
	const Type& get_type(void) const;
	// visit all expression ueed in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override;
	// init statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the statement
	virtual NextAction build_circuit(BuildContext& ctx) const override;
};

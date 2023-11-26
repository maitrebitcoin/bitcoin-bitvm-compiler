#pragma once

// set a variable statement
class Statement_SetVar : public Statement {
public:
	// name of the variable
	std::string var_name;
	Expression* expression;
public:
	// constructor
	Statement_SetVar(int line, std::string name, Expression* op) : Statement(line), var_name(name), expression(op) {}
	// get return type
	const Type& get_type(void) const { return expression->get_type(); }
	// visit all expression ueed in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override { visitor(*expression); }
	// init a statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the declaration statement
	virtual NextAction build_circuit(BuildContext& ctx) const override;
	// build the circuit for the assignment to int (for loop)
	void build_circuit_set_to_int(BuildContext& ctx, int new_val) const;

};

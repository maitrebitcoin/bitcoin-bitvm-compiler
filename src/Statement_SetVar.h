#pragma once

// set a variable statement
class Statement_SetVar : public Statement {
protected:
	// name of the variable
	std::string var_name;
	Expression* expression;
public:
	// constructor
	Statement_SetVar(int line, std::string name, Expression* op) : Statement(line), var_name(name), expression(op) {}
	// get return type
	const Type& get_type(void) const { return expression->get_type(); }
	// init a statmenet
	virtual void init(CodeBloc* parent) override;
	// build the circuit for the declaration statement
	virtual void build_circuit(BuildContext& ctx) const override;
};

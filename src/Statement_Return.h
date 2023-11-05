#pragma once

// "return" statement
class Statement_Return : public Statement {
protected:
	// expression to return
	Expression* expression;
public:
	// constructor
	Statement_Return(int line, Expression* e) : Statement(line), expression(e) {}
	// init a statmenet
	virtual void init(Scope& parent_scope) override;

	virtual bool is_return(void) const { return true; }
	// get return type
	const Type& get_type(void) const { return expression->get_type(); }

	// build the circuit for the return statement
	virtual void build_circuit(BuildContext& ctx) const override;
};


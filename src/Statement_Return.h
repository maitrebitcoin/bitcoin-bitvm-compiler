#pragma once

// "return" statement
class Statement_Return : public Statement {
protected:
	// expression to return
	Expression* expression;
public:
	// constructor
	Statement_Return(int line, Expression* e) : Statement(line), expression(e) {}
	// cast in Statement_Return
	virtual class Statement_Return* cast_to_Statement_Return(void) override { return this; }
	virtual bool is_return(void) const { return true; }
	// init a statmenet
	virtual void init(Scope& parent_scope) override;

	// get return type
	const Type& get_type(void) const { return expression->get_type(); }

	// build the circuit for the return statement
	virtual void build_circuit(BuildContext& ctx) const override;
};


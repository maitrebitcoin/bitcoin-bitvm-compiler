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
	// get return type
	const Type& get_type(void) const { return expression->get_type(); }
	// visit all expression ueed in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override { visitor(*expression); }

	// init a statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the return statement
	virtual BuildContext::NextAction build_circuit(BuildContext& ctx) const override;
};


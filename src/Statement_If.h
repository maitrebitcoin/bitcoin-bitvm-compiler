#pragma once

class CodeBloc;
// "if" statement
class Statement_If : public Statement {
protected:
	// expression to test
	Expression* expression;
	// bloc executed if  expression if true
	CodeBloc* bloc_if_true = nullptr;
	// bloc executed if  expression if true
	CodeBloc* bloc_if_false = nullptr;

public:
	// constructor
	Statement_If(int line, Expression* condidtion, CodeBloc* if_true, CodeBloc* if_false);
	// init if statmenet
	virtual void init(Scope& parent_scope) override;

	// build the circuit for the return statement
	virtual void build_circuit(BuildContext& ctx) const override;
};


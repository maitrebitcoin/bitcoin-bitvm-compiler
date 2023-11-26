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
	// cast in Statement_If
	virtual class Statement_If* cast_to_Statement_If(void) override  { return this; }
	// setthe bloc executed if  expression if true
	void setElseBloc( CodeBloc*  bloc_else) { bloc_if_false = bloc_else; }
	// visit all expression ueed in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override;

	// init if statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the return statement
	virtual NextAction build_circuit(BuildContext& ctx) const override;

protected:
	//  Init variables and If Gate for one side
	void _init_variables_and_gate(BuildContext& ctx_source, ScopeVariables& variables_dest, class Gate_IF* gate, Circuit& circuit, bool bloc_side) const;
};


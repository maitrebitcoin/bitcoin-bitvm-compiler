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
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override { visitor(*expression); }

	// init if statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the return statement
	virtual void build_circuit(BuildContext& ctx) const override;

protected:
	// add ircuits inputs requirements from a bloc to a IF gat
	void _add_all_bloc_input(BuildContext& ctx, CodeBloc* code_bloc, class Gate_IF* gate) const;
};


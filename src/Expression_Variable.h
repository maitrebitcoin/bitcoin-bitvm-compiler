#pragma once


// a variable usage in an expression
class Expression_Variable: public VariableDefinition, public Expression {
public:
	// constructor
	Expression_Variable(std::string n) : VariableDefinition( (Type *)nullptr, n) {}
	// init, will grag the type from the symbol table
	virtual void init(CodeBloc* parent_bloc) override;
	// get Operand type
	const Type& get_type(void) const { assert(type != nullptr); return *type; }
	// build the circuit for the  expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
};


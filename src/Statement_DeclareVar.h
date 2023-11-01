#pragma once


// int i => declaration of a variable statement
class Statement_DeclareVar : public Statement, public VariableDefinition {

public:
	// constructor
	Statement_DeclareVar(int line, Type* type, std::string name);
	// cast in Statement_DeclareVar
	virtual Statement_DeclareVar* cast_to_Statement_DeclareVarr(void) override { return this; }
	// get return type
	const Type& get_type(void) const { return *type; }
	// init a statmenet
	virtual void init(CodeBloc* parent) override;
	// build the circuit for the declaration statement
	virtual void build_circuit(BuildContext& ctx) const override;
};
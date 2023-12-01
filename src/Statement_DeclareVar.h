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
	// visit all expression ueed in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override {}
	// init a statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the declaration statement
	virtual BuildContext::NextAction build_circuit(BuildContext& ctx) const override;
};
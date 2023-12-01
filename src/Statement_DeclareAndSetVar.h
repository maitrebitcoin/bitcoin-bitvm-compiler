#pragma once

// déclare and set a variable statement
class Statement_DeclareAndSetVar : public Statement {
public:
	Statement_DeclareVar declaration;
	Statement_SetVar	 affectation;

public:
	// constructor
	Statement_DeclareAndSetVar(int line, Type* type, std::string varname, Expression* expr) :
		Statement(line),
		declaration(line, type, varname),
		affectation{ line, varname, expr }
	{}
	// cast to Statement_DeclareAndSetVar
	virtual Statement_DeclareAndSetVar* cast_to_Statement_DeclareAndSetVar(void) { return this; }
	// get return type
	const Type& get_type(void) const { return declaration.get_type(); }
	// visit all expression ueed in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override { affectation.visit_Expression(visitor); }
	// init a statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the declaration statement
	virtual BuildContext::NextAction build_circuit(BuildContext& ctx) const override;
};
#pragma once

// déclare and set a variable statement
class Statement_DeclareAndSetVar : public Statement {
protected:
	Statement_DeclareVar declaration;
	Statement_SetVar	 affectation;

public:
	// constructor
	Statement_DeclareAndSetVar(int line, Type* type, std::string varname, Expression* expr) :
		Statement(line),
		declaration(line, type, varname),
		affectation{ line, varname, expr }
	{}
	// get return type
	const Type& get_type(void) const { return declaration.get_type(); }
	// init a statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the declaration statement
	virtual void build_circuit(BuildContext& ctx) const override;
};
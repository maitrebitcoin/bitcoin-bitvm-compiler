#pragma once

class BuildContext;


// a variable usage in an expression
class Expression_Variable: public VariableDefinition, public Expression, public IVariableToConnexion {
public:
	// constructor
	Expression_Variable(std::string n) : VariableDefinition( (Type *)nullptr, n) {}
	// get Operand type
	const Type& get_type(void) const { assert(type != nullptr); return *type; }
	// visit all variables used in the Expression
	virtual void visit_all_used_variable(std::function<void(IVariableToConnexion& var2cnx)> visitor)  override { visitor(*this); }
	//--- IVariableToConnexion implentation :
	// get all the required connexion for the variable
	virtual std::vector<Connection*> get_var_connexion(BuildContext& ctx) override;

	// init, will grag the type from the symbol table
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the  expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
};


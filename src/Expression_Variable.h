#pragma once

class BuildContext;


// a variable usage in an expression
class Expression_Variable: public VariableDefinition, public Expression {
public:
	// constructor
	Expression_Variable(std::string n) : VariableDefinition( (Type *)nullptr, n) {}
	// cast to Expression_Variable
	virtual Expression_Variable* cast_to_Expression_Variable(void) override { return this; }
	// get Operand type
	const Type& get_type(void) const { assert(type != nullptr); return *type; }
	// visit all part used in the Expression
	virtual void visit_epression(IVisitExpression& visitor) override { visitor.onVariable(*this); }

	// init, will grag the type from the symbol table
	virtual void init(Scope& parent_scope) override;
	// get all gates used by the variable
	std::vector<Connection*> get_all_connexions(BuildContext& ctx) const;
	// build the circuit for the  expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
};


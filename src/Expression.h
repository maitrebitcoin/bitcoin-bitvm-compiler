#pragma once

#include <vector>
#include <functional>
class Literal;
class BinaryOperation;
class CodeBloc;
class BuildContext;
class Connection;
class Scope;
class Expression_Variable;

// generiqc interfac for visit_all_used_variable
class IVariableToConnexion {
public:
	// get all the required connexion for the variable
	virtual std::vector<Connection*> get_var_connexion(BuildContext& ctx) = 0;
};



// opérandd expression. "a" ,123, or a+b
class Expression {
protected:
	// true if the expression is between parentesis. ex: (a+1)
	bool has_parentesis = false;
public:
	// set if the expression is between parentesis. ex: (a+1)
	void set_parentesis(bool b) { has_parentesis = b; }

	// if the expression is a littrela, return it
	virtual Literal* cast_to_literal(void) { return nullptr; }
	// if the expression is a binairy expression, return it
	virtual BinaryOperation* cast_to_BinaryOperation(void) { return nullptr; }

	// get expression type
	virtual const Type& get_type(void) const = 0;
	// visit all variables used in the Expression
	virtual void visit_all_used_variable(std::function<void(IVariableToConnexion& var2cnx)> visitor)  = 0;

	// init
	virtual void init(Scope& parent_scope) = 0;
	// build the circuit for the expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) = 0;
};
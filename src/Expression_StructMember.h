#pragma once

#include <string>
#include "Expression.h"
#include "BuildContext.h"

// expression of the form "mystruct.member"

class Expression_StructMember : public Expression, public IVariableToConnexion {
public:
	std::string parent_name; // ex : "mystruct"
	std::string member_name; // ex : "member"

	Type* member_type = nullptr; // type the var
	int member_offest_in_bit = 0; // offset in bit of the member in the struct

public:
	// constructor
	Expression_StructMember(std::string parent_var, std::string member_name);

	// get expression type
	virtual const Type& get_type(void) const override;
	// visit all variables used in the Expression
	virtual void visit_all_used_variable(std::function<void(IVariableToConnexion& expr_var)> visitor)  override { visitor(*this); }
	//--- IVariableToConnexion implentation :
	// get all the required connexion for the variable
	virtual std::vector<Connection*> get_var_connexion(BuildContext& ctx) override;
	// init
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
};
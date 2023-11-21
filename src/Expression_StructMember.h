#pragma once

#include <string>
#include "Expression.h"
#include "BuildContext.h"

// expression of the form "mystruct.member"

class Expression_StructMember : public Expression {
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
	// visit all part used in the Expression
	virtual void visit_epression(IVisitExpression& visitor) override { visitor.onVariableInStruct(*this); }
	// init
	virtual void init(Scope& parent_scope) override;
	// get all gates used by the parent struct variable
	std::vector<Connection*> get_all_connexions_full_struct(BuildContext& ctx) const;
	// build the circuit for the expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
};
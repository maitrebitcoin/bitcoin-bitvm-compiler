#pragma once

#include <string>
#include "Expression.h"
#include "BuildContext.h"

// expression of the form "mystruct.member"

class Expression_StructMember : public Expression {
public:
	std::string parent_name; // ex : "mystruct"
	std::string member_name; // ex : "member"

	Type* type = nullptr; // type the var

public:
	// constructor
	Expression_StructMember(std::string parent_var, std::string member_name);

	// get expression type
	virtual const Type& get_type(void) const override;
	// init
	virtual void init(CodeBloc* parent_bloc) override;
	// build the circuit for the expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;
};
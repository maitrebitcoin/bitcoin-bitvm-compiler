#pragma once



// declare a struct statement
class Statement_DeclareStruct : public Statement {
protected:
	// type of the struct
	TypeStruct struct_type;

public:
	// constructor
	Statement_DeclareStruct(int line, std::string name, CodeBloc* cobe_membres);
	// get return type
	const Type& get_type(void) const { return struct_type; }
	// visit all expression ueed in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override {}
	// cast to Statement_DeclareStruct
	virtual Statement_DeclareStruct* cast_to_Statement_DeclareStruct(void) override { return this; }
	// init a statmenet
	virtual void init(Scope& parent_scope) override;
	// build the circuit for the declaration statement
	virtual NextAction build_circuit(BuildContext& ctx) const override;

protected:
	// add a member to the struct
	void _add_member(Type* type, std::string name);
};



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
	// init a statmenet
	virtual void init(CodeBloc* parent) override;
	// build the circuit for the declaration statement
	virtual void build_circuit(BuildContext& ctx) const override;
	// add a member to the struct
	void _add_member(Type* type, std::string name);
};



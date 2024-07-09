#pragma once

using FunctionCallParameter = std::vector<Expression*>;

// func(a,b) statement
class Statement_Function_Call : public Statement {
protected:
	// function name to call
	std::string function_to_call_name;
	// parameters
	FunctionCallParameter list_of_parameters;

	// function to call, set by init() :
	const Function* function = nullptr;
	// type of the function
	const Type* type = nullptr;
public:
	// constructor
	Statement_Function_Call(int line, std::string name);
	// init a statmenet
	virtual void init(Scope& parent_scope) override;

	// get function return type
	const Type& get_type(void) const;
	// visit all expression used in the statement
	virtual void visit_Expression(std::function<void(Expression& expr)> visitor) const override; //{ visitor(*expression); }

	// build the circuit for the function call statement
	virtual BuildContext::NextAction build_circuit(BuildContext& ctx) const override;
}; 

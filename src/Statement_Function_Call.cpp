// Function call statement

#include "Program.h"
#include "Error.h"

// constructor
Statement_Function_Call::Statement_Function_Call(int line, std::string name) 
	: Statement(line), function_to_call_name(name) {
}

// visit all expression used in the statement
void Statement_Function_Call::visit_Expression(std::function<void(Expression& expr)> visitor) const 
{
	// visit all parameters expressions
	for (Expression* e : list_of_parameters)
		visitor(*e);
}


// init a statmenet
void Statement_Function_Call::init(Scope& parent_scope) {
	// get the function definition by name
	function = parent_scope.find_function_by_name(function_to_call_name);
	if (function == nullptr)
		throw Error("Function not found", function_to_call_name);

	// check the number of parameters
	const Function::Definition& func_definition = function->get_definition();
	size_t nb_params = func_definition.parameters.size();
	if ( nb_params > list_of_parameters.size())
		throw Error("Function call with too many parameters", function_to_call_name);
	if (nb_params < list_of_parameters.size())
		throw Error("Function call with too few parameters", function_to_call_name);
	// check parameters type
	for (size_t i = 0; i < nb_params; i++) {
		Function::Parameter def_param_i = func_definition.parameters[i];
		Expression* param_i = list_of_parameters[i];
		const Type& type_param_i = param_i->get_type();
		if (!type_param_i.is_same_type(*def_param_i.type)) {
			std::string err_message = "Function call with wrong parameter type for parameter " + std::to_string(i + 1) + " : expected " + def_param_i.type->get_name() + " but got " + type_param_i.get_name();
			throw Error(err_message, function_to_call_name);
		}
	}


	// set the type
	const Type& returned_type = function->get_return_type();
	this->type = &returned_type;
	// init parameters
	for (Expression* e : list_of_parameters)
		e->init(parent_scope);
}

// build the circuit for the function call statement
BuildContext::NextAction Statement_Function_Call::build_circuit(BuildContext& ctx) const
{
	// build the expressions for all input parameters
	std::vector<std::vector<Connection*>> inputs;
	for (Expression* e : list_of_parameters) {
		std::vector<Connection*> input_i = e->build_circuit(ctx);
		// add input_i to the inputs
		inputs.push_back(input_i);
	}

	// create the context for the function call
	BuildContext ctx_function_call(ctx, BuildContext::Caller::function_call);
	// set the inputs
	const Function::Definition& func_definition = function->get_definition();
	int input_index = 0;
	size_t nb_params = func_definition.parameters.size();
	for (input_index = 0; input_index < nb_params; input_index++) {
		std::vector<Connection*>& input_i = inputs[input_index];
		Function::Parameter def_param_i = func_definition.parameters[input_index];
		// add var to the context
		ctx_function_call.variables().declare_param_var( def_param_i, input_i);

	
		// next input
		input_index++;
	}
	// build the circuit for the function call
	function->build_circuit(ctx_function_call);
	return BuildContext::NextAction::Continue;
}

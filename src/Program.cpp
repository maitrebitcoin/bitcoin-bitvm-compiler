// Implentation of the BtcContract class and its sub-classes

#include "Program.h"
#include <assert.h>
#include "Error.h"
#include "Circuit.h"
#include "LangageGrammar.h"
#include "BuildContext.h"

// add structures to the program as bloc before the main function
void Program::add_struct_definition(CodeBloc* bloc ) {
	// check that there are only struct definitions
	for (Statement* statement : bloc->statements)
	{
		// if the instruction is not a struct definition
		if (statement->cast_to_Statement_DeclareStruct() == nullptr)
			throw Error("Only struct definitions are allowed before functions");
	}
	// keep the bloc
	struct_definitions = bloc;
}

// add a N function to the program
void Program::add_array_function(std::vector<Function*>* tabf) {
	// add the functions vector
	for (Function* f : *tabf)
		add_function(f);
}

// add a function to the program
void Program::add_function(Function* f) {
	// si the function name is not already used
	if (find_function_by_name(f->get_name()))
	{
		throw Error("Function name already used");
	}
	// add the functions vector
	functions.push_back(f);
}
// get a function by name
Function* Program::find_function_by_name(std::string name) const  {
	for (Function* f : functions)
	{
		if (f->get_name() == name)
			return f;
	}
	return nullptr;
}
// get main function
Function* Program::main_function(void) const {
	return find_function_by_name("main");
}

// init program tree, phase 2 of compilation
void Program::init_and_check_program_tree(void) {
	// init struct
	if (struct_definitions != nullptr)
	{
		for (Statement* statement_struct : struct_definitions->statements)
			statement_struct->init(*this);
	}

	// init  check  functions
	for (Function* f : functions)
		f->init(*this);
	// check main function
	if (main_function() == nullptr)
		throw Error("No main function");
	// OK
}

// build a circuit that represents the program
void Program::build_circuit(BuildContext& build_context) {

	// getk main function
	Function& fn_main = *main_function();

	// build the circuit for the main function
	fn_main.build_circuit(build_context);

	// init id for all gates and connections of all circuits
	int connection_id = 0;
	int circuit_id    = 20000; // start a 20000 for debug purposes
	build_context.visit_circuits([&](Circuit& circuit) {
		circuit.set_id(circuit_id);
		circuit.init_id_gates_and_connexions(connection_id);
		circuit_id++;
	});
}


#include "BuildContext.h"
#include "Circuit.h"

// constructor
BuildContext::BuildContext(Caller caller) : create_caller(caller)
{
	assert(caller == Caller::main_body); 
	// alloc main circuit
	ctx_circuit = new Circuit();
}


// copy constructor
BuildContext::BuildContext(const BuildContext& source, Caller caller) : create_caller(caller)
{
	assert(caller != Caller::main_body);
	switch (caller) 
	{
		case Caller::if_statement:
			// create a new sub circuit
			ctx_circuit = new Circuit();
			for_statement = source.for_statement;
			// action to build after the if statement
			build_all_next_statements = source.build_all_next_statements;
			break;
		case Caller::for_statement:
			// share the same circuit in a for statement
			ctx_circuit = source.ctx_circuit;
			// action to build after the for statement
			build_all_next_statements = source.build_all_next_statements;

			// copy all vaiables
			ctx_variables = source.variables();
			break;
		case Caller::build_next_lambda:
			// sub conterxt to preserve build_all_next_statements
			ctx_circuit = source.ctx_circuit;
			for_statement = source.for_statement;
			parent_ctx_variables = const_cast<ScopeVariables*>(&source.variables());
			break;
		default:
			assert(false);
			throw Error("internal error : unknown caller");
	}

}


//  Init variables and If Gate for a "IF"  statmeent
void BuildContext::init_variables_if_gate(BuildContext& ctx_source, class Gate_IF* gate, bool bloc_side) {
	assert(create_caller == Caller::if_statement);

	// get all copy parameters from source
	BuildContext::InfoCopy infoCopy = ctx_source.get_info_copy();

	// init variables
	ctx_variables = infoCopy.variables_dest;
	// inits connexions to the gate
	for (Connection* connexion : infoCopy.connexions_dest) {
		gate->add_input(connexion, bloc_side);
	}
	// init circuit inputs
	circuit().set_circuit_inputs(infoCopy.nb_bits_in(), infoCopy.input_map);
}

//  get all info needed to create a new copy of acontexte; for "if statement" 
BuildContext::InfoCopy BuildContext::get_info_copy(void) const {

	InfoCopy result;
	// get all variables to copy
	variables().visit_all_variables([&](const ScopeVariable& variable_source) {

		// copy var from source to dest
		ScopeVariable* copy_var = result.variables_dest.copy_var(variable_source);
		// copy connexions for source to the gate
		for (Connection* connexion_copy : copy_var->bits) {
			result.connexions_dest.push_back(connexion_copy);
		}
	});

	// basic impleation of InterfaceInputsMap
	class Void_InterfaceInputsMap : public InterfaceInputsMap {
		// InterfaceInputsMap Implementation 
		// get a parameter info by name
		virtual InterfaceInputsMap::Info find_info_by_name(std::string name) const override {
			// used by commandline -run ony
			assert(false);
			throw Error("Internal error, not implemented");
		}
	};//InterfaceInputsMap

	// init circuit inputs
	result.input_map = new Void_InterfaceInputsMap();

	return result;

}


// visit all the circuits
void BuildContext::visit_circuits(std::function<void(Circuit&)> fnVisit) {
	// main circuit
	fnVisit(circuit());
	// sub circuits
	circuit().visit_sub_circuits(fnVisit);
}

// get the variables in the contexte
ScopeVariables& BuildContext::variables(void) const
{
	// if we share varaibles with the parent context
	if (parent_ctx_variables != nullptr) {
		assert(create_caller == Caller::build_next_lambda );
		return *parent_ctx_variables;
	}
	return const_cast<BuildContext*>(this)->ctx_variables;

}


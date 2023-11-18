
#include "CodeBloc.h"
#include "Program.h"
#include "Error.h"



// get the return statement of the bloc
Statement_Return* CodeBloc::get_return_statement(void) const {
	if (statements.size() == 0)
		return nullptr;
	// always last in bloc
	Statement* last_statement = statements.back();
	return last_statement->cast_to_Statement_Return();
}

// init a bloc
void CodeBloc::init(Scope& parent_scp) {
	// init parent function in the scope
	parent_function = parent_scp.get_parent_function();
	parent_scope	= &parent_scp;

	// reoorganize the bloc in case of "If"
	_reorganize_bloc_if_statement();

	// init statemtents
	for (Statement* statement_i : statements) {
		try {
			statement_i->init(*this);
		}
		catch (Error& e) {
			// add the line number

			e.line_number = statement_i->num_line;
			throw e;
		}
	}

	// check statements :
	if (statements.size() == 0)
		throw Error("Empty function");
	// last statement must be a return or if
	Statement* last_statement = statements.back();
	if (  last_statement->cast_to_Statement_Return() == nullptr
	   && last_statement->cast_to_Statement_If() == nullptr)
		throw Error("Last statement must be a return");


}

// reoorganize the bloc in case of "If"
void CodeBloc::_reorganize_bloc_if_statement(void) {

	// build the statements before "return"
	for (int i = 0; i < statements.size() - 1; i++) {
		Statement* statement = statements[i];
		// if it is a if
		Statement_If* if_statement = statement->cast_to_Statement_If();
		if (if_statement != nullptr)
		{
			// get the bloc after the if : else statement
			CodeBloc* bloc_after_if = new CodeBloc();
			// add the bloc after the if
			for (int k = i + 1; k < statements.size(); k++)
				bloc_after_if->statements.push_back(statements[k]);
			// remove  bloc after the if
			statements.erase(statements.begin() + i + 1);
			// set the else bloc
			if_statement->setElseBloc(bloc_after_if);
			// no more statements
			return;
		}

	}
}
// visit all variables used in the bloc
void CodeBloc::visit_all_used_variable(std::function<void(IVariableToConnexion& var2cnx)> visitor)
{
	
	for (Statement* statement_i : statements) {
		// visit expressions in the statement
		statement_i->visit_Expression([&](Expression& expression) {
			// visite variables in the expression
			expression.visit_all_used_variable([&](IVariableToConnexion& var2cnx) {
				// call the visitor
				visitor(var2cnx);
			});
		});
	}//for
}

// build a circuit that represents the bloc
void CodeBloc::build_circuit(BuildContext& ctx) {

	// build the statements before "return"
	for (int i = 0; i < statements.size(); i++) {
		Statement* statement = statements[i];
		try {
			statement->build_circuit(ctx);
		}
		catch (Error& e) {
			//add info to the error
			e.line_number = statement->num_line;
			throw e;
		}
	}
}

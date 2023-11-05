
#include "CodeBloc.h"
#include "Program.h"
#include "Error.h"



// get the return statement of the bloc
Statement_Return* CodeBloc::get_return_statement(void) const {
	// always last in bloc
	Statement* last_statement = statements.back();
	return dynamic_cast<Statement_Return*>(last_statement);
}

// init a bloc
void CodeBloc::init(Scope& parent_scp, Function* parent_fn) {
	// init parent function in the scope
	parent_function = parent_fn;
	parent_scope	= &parent_scp;

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
	// last statement must be a return
	Statement* last_statement = statements.back();
	if (!last_statement->is_return())
		throw Error("Last statement must be a return");
}
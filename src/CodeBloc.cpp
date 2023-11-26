
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
void CodeBloc::init(Scope& scope) {
	init_ex(scope, InitOption::return_must_be_present);
}
// init a bloc, extentend version used in "for" statement
void CodeBloc::init_ex(Scope& parent_scp, InitOption option) {
	// init parent function in the scope
	init_CodeBloc(parent_scp);

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

	// if return (or break) is optionnal
	if (option == InitOption::return_not_required)
		return;

	Statement* last_statement = statements.back();
	// if last statement can be a break and iti is a break
	if (option == InitOption::return_or_break_must_be_present
		&& last_statement->cast_to_Statement_Break() != nullptr)
		return; //OK

	// last statement must be a return or if
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
			statements.erase(statements.begin() + i + 1, statements.end());
			// set the else bloc
			if_statement->setElseBloc(bloc_after_if);
			// no more statements
			return;
		}

	}
}
// visit all part used in the bloc
void CodeBloc::visit_all_epressions(IVisitExpression& visitor)
{
	for (Statement* statement_i : statements) {
		// visit expressions in the statement
		statement_i->visit_Expression([&](Expression& expression) {
			// visi expression
			expression.visit_epression(visitor);
		});
	}//for
}
// visit all expression used in all the statement
void CodeBloc::visit_Expression(std::function<void(Expression& expr)> visitor) const {
	
	for (Statement* statement_i : statements) {
		// visit expressions in the statement
		statement_i->visit_Expression(visitor);
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

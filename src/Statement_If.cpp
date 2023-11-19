
#include <assert.h>
#include "Program.h"
#include "Error.h"
#include "Gate.h"
#include "Circuit.h"


// constructor
Statement_If::Statement_If(int line, Expression* condidtion, CodeBloc* if_true, CodeBloc* if_false) 
	: Statement(line)
	, expression(condidtion) 
	, bloc_if_true(if_true)
	, bloc_if_false(if_false)
{
	assert(bloc_if_true != nullptr || bloc_if_false != nullptr  );
}


// init if statmenet
void Statement_If::init(Scope& parent_scope) {
	// intialize the expression
	expression->init(parent_scope);

	// get expression type
	const Type& expresrion_type = expression->get_type();
	// must be a boolean
	if (!expresrion_type.is_bool())
		throw Error("If condition must be boolean");

	// both blocs must exists
	if (bloc_if_true == nullptr)
		throw Error("Internal error : if (true) bloc is null");
	if (bloc_if_false == nullptr)
		throw Error("Internal error : if (false) bloc is null");

	// init blocs of code
	bloc_if_true->init(parent_scope);
	bloc_if_false->init(parent_scope);
}

//  Init variables and If Gate for one side
void Statement_If::_init_variables_and_gate(BuildContext& ctx_source, ScopeVariables& variables_dest, class Gate_IF* gate, bool bloc_side) const {
	
	class Visitor : public IVisitExpression {
	protected:
		BuildContext& ctx_source;
		ScopeVariables& variables_dest;
		Gate_IF* gate_if;
		bool bloc_side;

	public:
		// constructor
		Visitor(BuildContext& c, ScopeVariables& v, Gate_IF* g, bool b ) : ctx_source(c), variables_dest(v), gate_if(g), bloc_side(b) {}

		// -- IVisitExpression implemebtatoin
		// epxression part is a literal.ex : 123
		virtual void onLiteral(Literal&) override {

		}
		// epxressison part is a varible. ex : a
		virtual void onVariable(Expression_Variable& expr_var )override {
			ScopeVariable* var_source = ctx_source.variables.find_by_name(expr_var.name);
			if (var_source == nullptr) return;
			// if the var already exists in the dest, do nothing
			if (variables_dest.find_by_name(expr_var.name) != nullptr) return;
			// copy var from source to dest
			variables_dest.copy_var(*var_source);
			// copy connexions for source to the gate
			std::vector<Connection*> connexions = expr_var.get_all_connexions(ctx_source);
			for (Connection* connexion : connexions) {
				gate_if->add_input(connexion, bloc_side);
			}
		}
		// epxressison part is a varible in a struct. ex : a.b
		virtual void onVariableInStruct(Expression_StructMember&) override {

		}


	};
	Visitor local_visitor(ctx_source, variables_dest, gate, bloc_side );

	CodeBloc* code_bloc = bloc_side ? bloc_if_true : bloc_if_false;
	// visit the bloc
	code_bloc->visit_all_epressions(local_visitor);

}

// build the circuit for the return statement
void Statement_If::build_circuit(BuildContext& ctx) const
{
	// build the condition expression
	std::vector<Connection*> expression_value = expression->build_circuit(ctx);
	assert(expression_value.size() == 1);

	// both blocs must exists
	if (bloc_if_true == nullptr)
		throw Error("Internal error : if (true) bloc is null");
	if (bloc_if_false == nullptr)
		throw Error("Internal error : if (false) bloc is null");

	// create 2 new sub-circuits 
	Circuit& circuit_if_true  = ctx.get_new_sub_circuit();
	BuildContext ctx_if_true(circuit_if_true);
	Circuit& circuit_if_false = ctx.get_new_sub_circuit();;
	BuildContext ctx_if_false(circuit_if_false);
	// créate a new gate IF
	Gate_IF* gate_if = new Gate_IF(&circuit_if_true, &circuit_if_false);

	// true case
	_init_variables_and_gate(ctx, ctx_if_true.variables, gate_if, true );
	bloc_if_true->build_circuit(ctx_if_true);
	// false case
	_init_variables_and_gate(ctx, ctx_if_false.variables, gate_if, false );
	bloc_if_false->build_circuit(ctx_if_false);

	// add the gate to the circuit
	std::array<Connection*, 1> input_1_bit = { expression_value[0] };
	std::array<Connection*, 0> void_result =
	gate_if->add_to_circuit(ctx.circuit, input_1_bit);


	// tell the ciruit outupt sizz, without real connexion.
	// the real output will be from or circuit_if_true ou ctx_if_false
	NbBit nb_bit_out = circuit_if_true.nb_bits_output();
	// the 2 circuits must have the same output size
	if (nb_bit_out != circuit_if_false.nb_bits_output())
		throw Error("If : 2 Return statements with different size");

	// connect the output of the expression to the output of the circuit
	ctx.circuit.set_output_size_child(nb_bit_out);
}



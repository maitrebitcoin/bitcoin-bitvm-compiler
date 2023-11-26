
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
// visit all expression ueed in the statement
void Statement_If::visit_Expression(std::function<void(Expression& expr)> visitor) const  {
	visitor(*expression); 
	if (bloc_if_true != nullptr)
		bloc_if_true->visit_Expression(visitor);
	if (bloc_if_false != nullptr)
		bloc_if_false->visit_Expression(visitor);
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
	if (parent_scope.is_in_for_loop())	{
		// insida a for loop, special rules : brek is ok, and the else bloc do not need a return
		bloc_if_true->init_ex( parent_scope, CodeBloc::InitOption::return_or_break_must_be_present);
		bloc_if_false->init_ex(parent_scope, CodeBloc::InitOption::return_not_required);
	}
	else {
		bloc_if_true->init( parent_scope);
		bloc_if_false->init(parent_scope);
	}
}

//  Init variables and If Gate for one side
void Statement_If::_init_variables_and_gate(BuildContext& ctx_source, ScopeVariables& variables_dest, class Gate_IF* gate, Circuit& circuit, bool bloc_side) const {
	
	class Visitor : public IVisitExpression {
	protected:
	// Closure
		BuildContext& ctx_source;
		ScopeVariables& variables_dest;
		Gate_IF* gate_if;
		bool bloc_side;
	public:
	// OUT
		// bit copied to the sub circuit
		int nb_bits_in= 0;

	public:
		// constructor
		Visitor(BuildContext& c, ScopeVariables& v, Gate_IF* g, bool b ) : ctx_source(c), variables_dest(v), gate_if(g), bloc_side(b) {}

		// -- IVisitExpression implemebtatoin
		// epxression part is a literal.ex : 123
		virtual void onLiteral(Literal&) override {
			// noting to do
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
			nb_bits_in += (int)connexions.size();
		}
		// epxressison part is a varible in a struct. ex : a.b
		virtual void onVariableInStruct(Expression_StructMember& expr_var) override {
			// get the parent variable type by name
			ScopeVariable* var_source_srruct = ctx_source.variables.find_by_name(expr_var.parent_name);
			if (var_source_srruct == nullptr) return;
			// add all the struct in dest 
			//#TODO : OPTIM, add only the struct member 
			// if the var already exists in the dest, do nothing
			if (variables_dest.find_by_name(expr_var.parent_name) != nullptr) return;
			// copy var from source to dest
			variables_dest.copy_var(*var_source_srruct);
			// copy connexions for source to the gate
			std::vector<Connection*> connexions = expr_var.get_all_connexions_full_struct(ctx_source);
			for (Connection* connexion : connexions) {
				gate_if->add_input(connexion, bloc_side);
			}
			nb_bits_in += (int)connexions.size();
		}
	protected:

	};
	Visitor local_visitor(ctx_source, variables_dest, gate, bloc_side );

	CodeBloc* code_bloc = bloc_side ? bloc_if_true : bloc_if_false;
	// visit the bloc
	code_bloc->visit_all_epressions(local_visitor);

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
	InputsMap input_map = new Void_InterfaceInputsMap();
	circuit.set_circuit_inputs(local_visitor.nb_bits_in, input_map);
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

	// create 2 contexts with 2 new sub circuits 
	BuildContext ctx_if_true(ctx,  BuildContext::Caller::if_statement );
	BuildContext ctx_if_false(ctx, BuildContext::Caller::if_statement);
	// créate a new gate IF
	Gate_IF* gate_if = new Gate_IF(&ctx_if_true.circuit(), &ctx_if_false.circuit());

	// init true case
	_init_variables_and_gate(ctx, ctx_if_true.variables, gate_if, ctx_if_true.circuit(), true);
	bloc_if_true->build_circuit(ctx_if_true);
	// init false case
	_init_variables_and_gate(ctx, ctx_if_false.variables, gate_if, ctx_if_false.circuit(), false);
	bloc_if_false->build_circuit(ctx_if_false);

	// add the gate to the circuit
	std::array<Connection*, 1> input_1_bit = { expression_value[0] };
	std::array<Connection*, 0> void_result =
	gate_if->add_to_circuit(ctx.circuit(), input_1_bit);


	// tell the ciruit outupt sizz, without real connexion.
	// the real output will be from or circuit_if_true ou ctx_if_false
	NbBit nb_bit_out = ctx_if_true.circuit().nb_bits_output();
	// the 2 circuits must have the same output size
	if (nb_bit_out != ctx_if_false.circuit().nb_bits_output())
		throw Error("If : 2 Return statements with different size");

	// connect the output of the expression to the output of the circuit
	ctx.circuit().set_output_size_child(nb_bit_out);
}



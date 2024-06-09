
#include "Expression.h"
#include "Literal.h"

//change the type of all litteral to a give type in the expression
// for the "int64 i = 0" case 
void Expression::change_all_litterals_type(const Type& new_type) {

	class Visitor : public IVisitExpression {
	protected:
		// Closure
		const Type& new_type;
	public:
		// constructor
		Visitor(const Type& t) : new_type(t) {}
		// -- IVisitExpression implemebtatoin
		// epxression part is a literal.ex : 123
		virtual void onLiteral(Literal& litteral) override {
			// check the type
			if (litteral.get_type().is_same_type(new_type))
				return; // nothing to do
			// change the litteral type
			litteral.change_type(new_type);
		}
		// expressison part is a varible. ex : a
		virtual void onVariable(Expression_Variable&) override {};
		// expressison part is a varible in a struct. ex : a.b
		virtual void onVariableInStruct(Expression_StructMember&) override {};
		// expressison part is a varible in a struct. ex : a.b
		virtual void onArrayElement(Expression_ArrayElement&) override  {};

	};
	Visitor visitor(new_type);
	// visit all part used in the Expression
	visit_epression(visitor);


}
#pragma once

#include "Expression.h"
#include "Type.h"

// ex: 123
class Literal : public Expression {
public:
	// type of the literal
	TypeBasic type;
	// value of the literal is string forme
	std::string value_str;
	// value of the literal in individuals bits
	std::vector<bool> value_bits;
public:
	// constructor
	Literal(TypeBasic t, std::string v);
	// visit all part used in the Expression
	virtual void visit_epression(IVisitExpression& visitor) override { visitor.onLiteral(*this); }
	// get Operand type
	virtual const Type& get_type(void) const override { return  type; }
	// if the expression is a littrela, return it
	virtual Literal* cast_to_literal(void) override { return this; }
	// value of the literal has int. only for int type
	int get_int_value(void);
	static int _get_int_value(std::string str_val);
	static int64_t _get_int64_value(std::string str_val);
	static uint64_t _get_uint64_value(std::string str_val);

	// init
	virtual void init(Scope& parent_scope) override;
	// change literal type. ex int8 to int256
	void change_type(const Type& new_type);
	// build the circuit for the  expression
	virtual std::vector<Connection*> build_circuit(BuildContext& ctx) override;

	// get value of litteral string as a vector of bit
	static std::vector<bool> get_bools_from_value_str(Type::Native native_type, std::string value_str);
	// convert hex string of any size to array of bits in low endian
	static std::vector<bool> hex_string_to_bits(std::string hex_string);
	// get value  of an int as a vector of connexions
	static std::vector<Connection*> get_connections_from_value_int(BuildContext& ctx, const Type& type, int value_int);


protected:
	// convert the value of the literal to a vector of bits for bool type
	static std::vector<bool> _get_bits_value_bool(std::string str_val);
	// convert the value of the literal to a vector of bits for byte type
	// convert to bits in low endian (x86 format)
	// result[0] is the least significant bit, result[7] is the most significant bit
	enum class Sign { signed_, unsigned_ };
	static std::vector<bool> _get_bits_value_int8(std::string str_val, Sign signed_) ;
	static std::vector<bool> _get_bits_value_int32(std::string str_val, Sign signed_) ;
	static std::vector<bool> _get_bits_value_int64(std::string str_val, Sign signed_) ;
	static std::vector<bool> _get_bits_value_int256(std::string str_val, Sign signed_) ;
};


#include "Program.h"
#include "Error.h"
#include "Circuit.h"


// lietral init
void Literal::init(Scope&) {
	// init value from type ans source string
	// ex: "0x12" => 0x12
	value_bits = get_bools_from_value_str(type.get_native_type(), value_str);
}

// get value of liette as a vector of bit
std::vector<bool>  Literal::get_bools_from_value_str(Type::Native native_type, std::string value_str) {
	switch (native_type)
	{
	case Type::Native::bit:
		return  _get_bits_value_bool(value_str);
	case Type::Native::int8:
		return  _get_bits_value_int8(value_str,  Sign::signed_);
	case Type::Native::uint8:
		return  _get_bits_value_int8(value_str, Sign::unsigned_);
	case Type::Native::int32:
		return  _get_bits_value_int32(value_str, Sign::signed_);
	case Type::Native::uint32:
		return  _get_bits_value_int32(value_str, Sign::unsigned_);
	case Type::Native::int64:
		return  _get_bits_value_int64(value_str, Sign::signed_);
	case Type::Native::uint64:
		return  _get_bits_value_int64(value_str, Sign::unsigned_);
	case Type::Native::int256:
		return  _get_bits_value_int256(value_str, Sign::signed_);
	case Type::Native::uint256:
		return  _get_bits_value_int256(value_str, Sign::unsigned_);
	default:
		throw Error("Invalid literal type");
	}
}

// convert the value of the literal to a vector of bits for byte type
// convert to bits in low endian (x86 format)
// result[0] is the least significant bit, result[7] is the most significant bit
std::vector<bool> Literal::_get_bits_value_int8(std::string str_val, Sign signed_)  {
	std::vector<bool> result;
	// conver to integer
	int value_int8 = _get_int_value(str_val);
	// value must be in 8bit signed range
	bool is_signed = (signed_ == Sign::signed_);
	int min = is_signed ? -128 : 0;
	int max = is_signed ? 127 : 255;
	if (value_int8 > max || value_int8 < min)
		throw Error("Invalid 8 bit value : ", str_val);
	// convert to bits in low endian (x86 format)
	for (int i = 0; i < 8; i++)
	{
		bool b = value_int8 & 1;
		result.push_back(b);
		value_int8 >>= 1;
	}
	return result;
}
// convert the value of the literal to a vector of bits for byte type
// convert to bits in low endian (x86 format)
// result[0] is the least significant bit, result[31] is the most significant bit
std::vector<bool> Literal::_get_bits_value_int32(std::string str_val, Sign signed_) {
	std::vector<bool> result;
	// conver to integer
	int64_t value_int32 = _get_int_value(str_val);
	// value must be in 8bit signed range
	bool is_signed = (signed_ == Sign::signed_);
	int64_t min = is_signed ? -2147483648 : 0;
	int64_t max = is_signed ?  0x7FFFFFFF : 0xFFFFFFFF;
	if (value_int32 > max || value_int32 < min)
		throw Error("Invalid 32 bit value : ", str_val);

	// convert to bits in low endian (x86 format)
	for (int i = 0; i < 32; i++)
	{
		bool b = value_int32 & 1;
		result.push_back(b);
		value_int32 >>= 1;
	}
	return result;
}
// convert the value of the literal to a vector of bits for byte type
// convert to bits in low endian (x86 format)
// result[0] is the least significant bit, result[61] is the most significant bit
std::vector<bool> Literal::_get_bits_value_int64(std::string str_val, Sign signed_) {
	std::vector<bool> result;
	
	// convert to integer
	uint64_t value_uint64 = 0;
	if (signed_ == Sign::signed_)
		value_uint64 = (uint64_t)_get_int64_value(str_val);
	else
		value_uint64 = _get_uint64_value(str_val);
	// convert to bits in low endian (x86 format)
	for (int i = 0; i < 32; i++)
	{
		bool b = value_uint64 & 1;
		result.push_back(b);
		value_uint64 >>= 1;
	}
	return result;
}

// check if the string is in hexa format
// ex: 0x12
bool is_hexa_format(std::string value_str) {
	return(value_str.size() > 2 && value_str[0] == '0' && value_str[1] == 'x');
}
// convert hex char to int
int hex_char_to_int(char c)
{
	// convert to int
	int value = 0;
	if (c >= '0' && c <= '9')
		value = c - '0';
	else if (c >= 'a' && c <= 'f')
		value = c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		value = c - 'A' + 10;
	else
		throw Error("Invalid hex string");
	return value;
}


// convert the value of the literal to a vector of bits for byte type
// convert to bits in low endian (x86 format)
// result[0] is the least significant bit, result[61] is the most significant bit
std::vector<bool> Literal::_get_bits_value_int256(std::string str_val, Sign signed_) {
	// only hex string is supported
	if (!is_hexa_format(str_val))
		throw Error("Invalid 256 bit value : ", str_val);
	// max len si 256 bits = 64 hexa chars
	if (str_val.size() > 66)
		throw Error("Invalid 256 bit value : ", str_val);
	// compute the number with 0
	size_t nb_zero = 64 - str_val.size() + 2;
	if (nb_zero>0)
		str_val.append(nb_zero,'0');

	// get string wihout prefix "0x"
	std::string str_hex_without_prefix = str_val.substr(2);

	// convert to bits in low endian (x86 format)
	// being woth low bits first
	std::vector<bool> result = hex_string_to_bits(str_hex_without_prefix);
	assert(result.size() == 256);
	return result;

}


// convert hex string to array of bits in low endian
std::vector<bool> Literal::hex_string_to_bits(std::string hex_string) {
	// for eah char in hex string, starting from the end
	std::vector<bool> result;
	for (int i = (int)hex_string.size() - 1; i >= 0; i--)
	{
		// get the char
		char c = hex_string[i];
		// convert to int
		int value = hex_char_to_int(c);
		// convert to bits le
		for (int j = 0; j < 4; j++)
		{
			bool b = value & 1;
			result.push_back(b);
			value >>= 1;
		}
	}
	return result;
}


// convert the value of the literal to a vector of bits for bool type
std::vector<bool> Literal::_get_bits_value_bool(std::string str_val)  {
	std::vector<bool> result;
	if (str_val == "true")
		result = { true };
	else if (str_val == "false")
		result = { false };
	else
		throw Error("Invalid boolean value : ", str_val);
	return result;
}


// value of the literal has int. only for int type
int Literal::get_int_value(void) {
	assert(get_type().is_integer());
	return _get_int_value(value_str);
}
// static version
int Literal::_get_int_value(std::string value_str) {
	// if the is hexa. ex :"0xED"
	if (value_str.size() > 2 && value_str[0] == '0' && value_str[1] == 'x')
		return std::stoi(value_str, nullptr, 16);
	// value is decimal ex; "123"
	return std::stoi(value_str);
}
int64_t Literal::_get_int64_value(std::string value_str) {
	// if the is hexa. ex :"0xED123EDA34"
	if (value_str.size() > 2 && value_str[0] == '0' && value_str[1] == 'x')
		return std::stoll(value_str, nullptr, 16);
	// value is decimal ex; "12344554768041"
	return std::stoll(value_str);

}
uint64_t Literal::_get_uint64_value(std::string str_val) {
	// if the is hexa. ex :"0xED123EDA34"
	if (str_val.size() > 2 && str_val[0] == '0' && str_val[1] == 'x')
		return std::stoull(str_val, nullptr, 16);
	// value is decimal ex; "12344554768041"
	return std::stoull(str_val);
}

std::vector<Connection*> Literal::build_circuit(BuildContext& ctx) {
	std::vector<Connection*> result;

	assert(type.size_in_bit() == value_bits.size());
	// get a 0 ou 1 connexion for each bit
	for (int i = 0; i < type.size_in_bit(); i++) {
		// get the bit value
		bool b = value_bits[i];
		// get literal value as a vector of bits
		Connection* connection_to_0or1 = ctx.circuit.get_literal_values(b);
		result.push_back(connection_to_0or1);
	}

	return result;
}

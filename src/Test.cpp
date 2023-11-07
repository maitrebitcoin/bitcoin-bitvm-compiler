// tests of the compiler
// -synxtax, grammar
// -gats logic
// -operations

#include "Compiler.h"
#include "Error.h"
#include "Circuit.h"
#include <iostream>
#include "Program.h"

// convert bits  to string
std::string bits_to_string(const std::vector<Bit>& bits) {
	std::string s = "";
	for (Bit bit : bits) {
		s += bit ? '1' : '0'; // std::to_string(bit->value);
	}
	return s;
}
// called if a test fail
void test_failed( std::string msg="") {
	std::cout << "FAILED : " << msg << "\n";
	exit(1);
}

// test 1 circuit
//<input_01> : ex : "01"
void _test_circuit(Circuit& circuit, const char* inputs, const char* expected_result) {
	// set the inputs
	CRunInputs test_input = circuit.get_run_inputs();
	test_input.set_bit_value(0, inputs[0] == '1');
	if (inputs[1]!=0) // 2 inputs ?
		test_input.set_bit_value(1, inputs[1] == '1');
	// run
	circuit.reset();
	std::vector<Bit> test_result = circuit.run(test_input);

	// check outputs
	int i = 0;
	while (expected_result[i])
	{
		bool expected_bit_i = (expected_result[i] == '1');
		if (expected_bit_i != test_result[i])
		{
			// FAILED
			test_failed();
			return;
		}
		i++;
	}
	// ok
}

// test 1 circuit, hex inputs
//<input_01> : ex : "01"
void _test_circuit_hex(Circuit& circuit, std::string hex_inputs, std::string  hex_expected_result) {
	//auto bits_b = Literal::hex_string_to_bits(hex_inputs.substr(2,2));
	std::vector<bool>  bits_expected;
	if (hex_expected_result.size()==1) // bool
		bits_expected.push_back(hex_expected_result[0] == '1');
	else
		bits_expected = Literal::hex_string_to_bits(hex_expected_result);
	// set inputs
	CRunInputs test_input = circuit.get_run_inputs();
	int i = 0;
	// convert hex string to bits
	int nb_bytes_in = (int)( hex_inputs.size() * 4);
	if (nb_bytes_in == 8) { // 8 bitd
		auto _8bits_in = Literal::hex_string_to_bits(hex_inputs);
		test_input.set_int8_value(0, _8bits_in);
	}
	else if (nb_bytes_in == 16) { // 2*8 bitd
		auto _8bits_a = Literal::hex_string_to_bits(hex_inputs.substr(0, 2));
		auto _8bits_b = Literal::hex_string_to_bits(hex_inputs.substr(2, 2));
		test_input.set_int8_value(0, _8bits_a);
		test_input.set_int8_value(1, _8bits_b);
	}
	else if (nb_bytes_in == 24) { // 3*8 bitd
		auto _8bits_a = Literal::hex_string_to_bits(hex_inputs.substr(0, 2));
		auto _8bits_b = Literal::hex_string_to_bits(hex_inputs.substr(2, 2));
		auto _8bits_c = Literal::hex_string_to_bits(hex_inputs.substr(4, 2));
		test_input.set_int8_value(0, _8bits_a);
		test_input.set_int8_value(1, _8bits_b);
		test_input.set_int8_value(2, _8bits_c);
	}
	else if (nb_bytes_in == 64) { // 2*32 bitd
		auto _32bits_a = Literal::hex_string_to_bits(hex_inputs.substr(0, 8));
		auto _32bits_b = Literal::hex_string_to_bits(hex_inputs.substr(8, 8));
		test_input.set_int32_value(0, _32bits_a);
		test_input.set_int32_value(1, _32bits_b);
	}
	else {
		assert(false);
	}

	// run the circuit
	std::vector<Bit> result;
	try {
		circuit.reset();
		result = circuit.run(test_input);
	}
	catch (Error& e) {
		test_failed(e.message);
	}

	if (result.size() != bits_expected.size())
		test_failed("output size mismatch");

	// check outputs
	for (int i = 0; i < bits_expected.size(); i++) {
		bool expected_bit_i = bits_expected[i];
		if (expected_bit_i != result[i]) {
			// FAILED
			test_failed(hex_expected_result);
			return;
		}
	}
	// OK

}

void test_not_gate(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_not.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit(result.circuit, "0", "1");
	_test_circuit(result.circuit, "1", "0");
}

void test_xor_gate(void){
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_xor.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit(result.circuit, "00", "0");
	_test_circuit(result.circuit, "01", "1");
	_test_circuit(result.circuit, "10", "1");
	_test_circuit(result.circuit, "11", "0");
}
void test_and_gate(void){
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_and.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit(result.circuit, "00", "0");
	_test_circuit(result.circuit, "01", "0");
	_test_circuit(result.circuit, "10", "0");
	_test_circuit(result.circuit, "11", "1");
}
void test_or_gate(void)
{
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_or.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit(result.circuit, "00", "0");
	_test_circuit(result.circuit, "01", "1");
	_test_circuit(result.circuit, "10", "1");
	_test_circuit(result.circuit, "11", "1");
}
void test_equal_bool(void)
{
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_equal_bool.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit(result.circuit, "00", "1");
	_test_circuit(result.circuit, "01", "0");
	_test_circuit(result.circuit, "10", "0");
	_test_circuit(result.circuit, "11", "1");
}


void test_local_var(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_local_var.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit(result.circuit, "00", "0");
	_test_circuit(result.circuit, "01", "1");
	_test_circuit(result.circuit, "10", "1");
	_test_circuit(result.circuit, "11", "0");
}
void test_local_var_and_set(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_declare_and_set.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit(result.circuit, "00", "0");
	_test_circuit(result.circuit, "01", "1");
	_test_circuit(result.circuit, "10", "1");
	_test_circuit(result.circuit, "11", "0");
}
void test_int8(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_int8.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "AA22", "22");
	_test_circuit_hex(result.circuit, "FFFF", "FF");
	_test_circuit_hex(result.circuit, "0000", "00");
	_test_circuit_hex(result.circuit, "00FF", "00");
	_test_circuit_hex(result.circuit, "AB11", "01");
	_test_circuit_hex(result.circuit, "77FC", "74");
}
void test_int32(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_int32.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit : a & b
	_test_circuit_hex(result.circuit, "1122334500000001", "00000001");
	_test_circuit_hex(result.circuit, "FFFFFFFFFFFFFFFF", "FFFFFFFF");
	_test_circuit_hex(result.circuit, "0000000000000000", "00000000");
	_test_circuit_hex(result.circuit, "000000FF00000000", "00000000");
	_test_circuit_hex(result.circuit, "123400AB12340011", "12340001");
	_test_circuit_hex(result.circuit, "77000000FC000000", "74000000");
	_test_circuit_hex(result.circuit, "0077000000FC0000", "00740000");
	_test_circuit_hex(result.circuit, "000077000000FC00", "00007400");
	_test_circuit_hex(result.circuit, "00000077000000FC", "00000074");
}

void test_literal(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_literal.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "AA", "0A");
	_test_circuit_hex(result.circuit, "FF", "1F");
	_test_circuit_hex(result.circuit, "00", "00");
	_test_circuit_hex(result.circuit, "FE", "1E");
	_test_circuit_hex(result.circuit, "11", "11");
	_test_circuit_hex(result.circuit, "EC", "0C");
}
void test_addition(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_add.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "0000", "00");
	_test_circuit_hex(result.circuit, "0001", "01");
	_test_circuit_hex(result.circuit, "0100", "01");
	_test_circuit_hex(result.circuit, "0010", "10");
	_test_circuit_hex(result.circuit, "0201", "03");
	_test_circuit_hex(result.circuit, "0202", "04");
	_test_circuit_hex(result.circuit, "0203", "05");
	_test_circuit_hex(result.circuit, "0101", "02");
	_test_circuit_hex(result.circuit, "0701", "08");
	_test_circuit_hex(result.circuit, "FE01", "FF");
	_test_circuit_hex(result.circuit, "EF01", "F0");
	_test_circuit_hex(result.circuit, "01FE", "FF");
	_test_circuit_hex(result.circuit, "14DE", "F2");
	_test_circuit_hex(result.circuit, "87E7", "6E");
	_test_circuit_hex(result.circuit, "FFFF", "FE");
	_test_circuit_hex(result.circuit, "01FF", "00");
	_test_circuit_hex(result.circuit, "FF01", "00");

}
void test_negate(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_negate.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "00", "00");
	_test_circuit_hex(result.circuit, "01", "FF");
	_test_circuit_hex(result.circuit, "FF", "01");
	_test_circuit_hex(result.circuit, "02", "FE");
	_test_circuit_hex(result.circuit, "14", "EC");
	_test_circuit_hex(result.circuit, "77", "89");
	_test_circuit_hex(result.circuit, "89", "77");
	_test_circuit_hex(result.circuit, "ED", "13");
	_test_circuit_hex(result.circuit, "13", "ED");
	_test_circuit_hex(result.circuit, "81", "7F");
	_test_circuit_hex(result.circuit, "7F", "81");
}
void test_subtraction(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_sub.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "0000", "00");
	_test_circuit_hex(result.circuit, "0100", "01");
	_test_circuit_hex(result.circuit, "0001", "FF");
	_test_circuit_hex(result.circuit, "1312", "01") ;
	_test_circuit_hex(result.circuit, "817F", "02") ;
	_test_circuit_hex(result.circuit, "DA73", "67") ;
	_test_circuit_hex(result.circuit, "FFFE", "01") ;
	_test_circuit_hex(result.circuit, "FFFF", "00") ;
}
void test_complement(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_complement.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "00", "FF");
	_test_circuit_hex(result.circuit, "01", "FE");
	_test_circuit_hex(result.circuit, "02", "FD");
	_test_circuit_hex(result.circuit, "03", "FC");
	_test_circuit_hex(result.circuit, "FF", "00");
	_test_circuit_hex(result.circuit, "13", "EC");
	_test_circuit_hex(result.circuit, "EC", "13");
	_test_circuit_hex(result.circuit, "81", "7E");
	_test_circuit_hex(result.circuit, "7E", "81");
	_test_circuit_hex(result.circuit, "DD", "22");

}
void test_parenthesis(void) {
	// ccmpile the circuit : res = ~(a&b)-(a+b);
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_parenthesis.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "0000", "FF");
	_test_circuit_hex(result.circuit, "FF00", "00");
	_test_circuit_hex(result.circuit, "FFFF", "02");
	_test_circuit_hex(result.circuit, "0102", "FC");
	_test_circuit_hex(result.circuit, "547D", "DA");

}
void test_shit_left(void) {
	// ccmpile the circuit : res = ~(a&b)-(a+b);
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_shit_left.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "00", "00");
	_test_circuit_hex(result.circuit, "01", "08");
	_test_circuit_hex(result.circuit, "03", "18");
	_test_circuit_hex(result.circuit, "07", "38");
	_test_circuit_hex(result.circuit, "10", "80");
	_test_circuit_hex(result.circuit, "40", "00");
	_test_circuit_hex(result.circuit, "9D", "E8");
	_test_circuit_hex(result.circuit, "FF", "F8");
}
void test_shit_right(void) {
	// ccmpile the circuit : res = ~(a&b)-(a+b);
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_shit_right.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "00", "00");
	_test_circuit_hex(result.circuit, "01", "00");
	_test_circuit_hex(result.circuit, "03", "00");
	_test_circuit_hex(result.circuit, "08", "01");
	_test_circuit_hex(result.circuit, "18", "03");
	_test_circuit_hex(result.circuit, "37", "06");
	_test_circuit_hex(result.circuit, "80", "10");
	_test_circuit_hex(result.circuit, "40", "08");
	_test_circuit_hex(result.circuit, "9D", "13");
	_test_circuit_hex(result.circuit, "FF", "1F");
}
void test_equal_int8(void) {
	// ccmpile the circuit : res = ~(a&b)-(a+b);
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_equal_int8.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "0000", "1");
	_test_circuit_hex(result.circuit, "0101", "1");
	_test_circuit_hex(result.circuit, "2121", "1");
	_test_circuit_hex(result.circuit, "2111", "0");
	_test_circuit_hex(result.circuit, "2112", "0");
	_test_circuit_hex(result.circuit, "5151", "1");
	_test_circuit_hex(result.circuit, "5152", "0");
	_test_circuit_hex(result.circuit, "5153", "0");
	_test_circuit_hex(result.circuit, "5154", "0");
	_test_circuit_hex(result.circuit, "DDDD", "1");
	_test_circuit_hex(result.circuit, "DD53", "0");
	_test_circuit_hex(result.circuit, "FFFF", "1");
	_test_circuit_hex(result.circuit, "FEFF", "0");
	_test_circuit_hex(result.circuit, "FFFE", "0");
}
void test_notequal_int8(void) {
	// ccmpile the circuit : res = ~(a&b)-(a+b);
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_notequal_int8.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "0000", "0");
	_test_circuit_hex(result.circuit, "0101", "0");
	_test_circuit_hex(result.circuit, "2121", "0");
	_test_circuit_hex(result.circuit, "2111", "1");
	_test_circuit_hex(result.circuit, "2112", "1");
	_test_circuit_hex(result.circuit, "FFFE", "1");
}
void test_greater_lower(void) {
	// ccmpile the circuit : res =  a>b
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_greater.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "0000", "0");
	_test_circuit_hex(result.circuit, "0100", "1");
	_test_circuit_hex(result.circuit, "0200", "1");
	_test_circuit_hex(result.circuit, "0202", "0");
	_test_circuit_hex(result.circuit, "0201", "1");
	_test_circuit_hex(result.circuit, "0102", "0");
	_test_circuit_hex(result.circuit, "6E56", "1");
	_test_circuit_hex(result.circuit, "6E6D", "1");
	_test_circuit_hex(result.circuit, "6E6E", "0");
	_test_circuit_hex(result.circuit, "6E6F", "0");
	_test_circuit_hex(result.circuit, "2112", "1");
	_test_circuit_hex(result.circuit, "FFFE", "1");
	_test_circuit_hex(result.circuit, "FFFF", "0");

	// ccmpile the circuit : res =  a>=b
	result = Compiler::compile_circuit_from_file("./sample/test_greater_or_equal.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "0000", "1");
	_test_circuit_hex(result.circuit, "0100", "1");
	_test_circuit_hex(result.circuit, "0200", "1");
	_test_circuit_hex(result.circuit, "0202", "1");
	_test_circuit_hex(result.circuit, "0201", "1");
	_test_circuit_hex(result.circuit, "0102", "0");
	_test_circuit_hex(result.circuit, "6E56", "1");
	_test_circuit_hex(result.circuit, "6E6D", "1");
	_test_circuit_hex(result.circuit, "6E6E", "1");
	_test_circuit_hex(result.circuit, "6E6F", "0");
	_test_circuit_hex(result.circuit, "2112", "1");
	_test_circuit_hex(result.circuit, "FFFE", "1");
	_test_circuit_hex(result.circuit, "FEFF", "0");
	_test_circuit_hex(result.circuit, "FFFF", "1");


	// ccmpile the circuit : res =  a<b
	result = Compiler::compile_circuit_from_file("./sample/test_lower.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	_test_circuit_hex(result.circuit, "0000", "0");
	_test_circuit_hex(result.circuit, "0100", "0");
	_test_circuit_hex(result.circuit, "0001", "1");
	_test_circuit_hex(result.circuit, "0200", "0");
	_test_circuit_hex(result.circuit, "0707", "0");
	_test_circuit_hex(result.circuit, "0102", "1");
	_test_circuit_hex(result.circuit, "6E56", "0");
	_test_circuit_hex(result.circuit, "6E6D", "0");
	_test_circuit_hex(result.circuit, "6E6E", "0");
	_test_circuit_hex(result.circuit, "6E6F", "1");
	_test_circuit_hex(result.circuit, "FFFE", "0");
	_test_circuit_hex(result.circuit, "FFFF", "0");

	// ccmpile the circuit : res  a<=b
	result = Compiler::compile_circuit_from_file("./sample/test_lower_or_equal.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	_test_circuit_hex(result.circuit, "0000", "1");
	_test_circuit_hex(result.circuit, "0100", "0");
	_test_circuit_hex(result.circuit, "0001", "1");
	_test_circuit_hex(result.circuit, "0200", "0");
	_test_circuit_hex(result.circuit, "0707", "1");
	_test_circuit_hex(result.circuit, "0102", "1");
	_test_circuit_hex(result.circuit, "6E56", "0");
	_test_circuit_hex(result.circuit, "6E6D", "0");
	_test_circuit_hex(result.circuit, "6E6E", "1");
	_test_circuit_hex(result.circuit, "6E6F", "1");
	_test_circuit_hex(result.circuit, "FFFE", "0");
	_test_circuit_hex(result.circuit, "FEFF", "1");
	_test_circuit_hex(result.circuit, "FFFF", "1");

}
void test_precedence(void) {
	// ccmpile the circuit : 
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_precedencec.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "0000", "00");
	_test_circuit_hex(result.circuit, "02FF", "10");
	_test_circuit_hex(result.circuit, "0FFF", "78");
	_test_circuit_hex(result.circuit, "EEDD", "60");
}
void test_structure(void) {
	// int8 main(Header st_ab, int8 c)
	// return st_ab.b + c; 
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_struct.bvc");
	if (!result.ok) {
		test_failed(result.error.message);
	}
	// test the circuit
	_test_circuit_hex(result.circuit, "000000", "00");
	_test_circuit_hex(result.circuit, "040102", "03");
	_test_circuit_hex(result.circuit, "F41135", "46");
}



// run all tests
void run_all_test(void) {
	std::cout << "Testing...\n";

	//test basic gates
	test_not_gate();			std::cout << " not - PASSED\n";
	test_xor_gate();			std::cout << " xor - PASSED\n";
	test_and_gate();			std::cout << " and - PASSED\n";
	test_or_gate();				std::cout << " or - PASSED\n";
	test_equal_bool();			std::cout << " equal - PASSED\n";
	// test basic langage consctuctions and operators
	test_local_var();			std::cout << " local var - PASSED\n";
	test_local_var_and_set();	std::cout << " local set var & set - PASSED\n";
	test_int8();				std::cout << " int8 - PASSED\n";
	test_int32();				std::cout << " int33 - PASSED\n";
	test_literal();				std::cout << " literal - PASSED\n";
	test_addition();			std::cout << " addition - PASSED\n";
	test_negate();				std::cout << " negate - PASSED\n";
	test_subtraction();			std::cout << " subtraction - PASSED\n";
	test_complement();			std::cout << " complement - PASSED\n";
	test_parenthesis();			std::cout << " parenthesis - PASSED\n";
	test_shit_left();			std::cout << " shit left - PASSED\n";
	test_shit_right();			std::cout << " shit right - PASSED\n";
	test_equal_int8();			std::cout << " equal int8 - PASSED\n";
	test_notequal_int8();		std::cout << " notequal int8 - PASSED\n";
	test_greater_lower();		std::cout << " greater lower - PASSED\n";
	test_precedence();			std::cout << " precedence - PASSED\n";  
	test_structure();		    std::cout << " structures - PASSED\n";

	// OK
	std::cout << "OK\n";

}
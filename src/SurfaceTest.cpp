// tests of the compiler

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
	CInputs test_input;
	while (*inputs)
	{
		if (*inputs == '1')
			test_input.add_bit(true);
		else
			test_input.add_bit(false);
		inputs++;
	}

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
	// convert hex string to bits
	auto bits_in = Literal::hex_string_to_bits(hex_inputs);
	auto bits_expected = Literal::hex_string_to_bits(hex_expected_result);
	// set inputs
	CInputs test_input;
	for (bool bit : bits_in) {
		test_input.add_bit(bit);
	}
	// run the circuit
	std::vector<Bit> test_result;
	try {
		circuit.reset();
		test_result = circuit.run(test_input);
	}
	catch (Error& e) {
		test_failed(e.message);
	}

	if (test_result.size() != bits_expected.size())
		test_failed("output size mismatch");

	// check outputs
	for (int i = 0; i < bits_expected.size(); i++) {
		bool expected_bit_i = bits_expected[i];
		if (expected_bit_i != test_result[i]) {
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
void test_byte(void) {
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_byte.bvc");
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


// run all tests
void run_all_test(void) {
	std::cout << "Testing...\n";

	//test basic gates
	test_not_gate();			std::cout << " not - PASSED\n";
	test_xor_gate();			std::cout << " xor - PASSED\n";
	test_and_gate();			std::cout << " and - PASSED\n";
	test_or_gate();				std::cout << " or - PASSED\n";
	test_local_var();			std::cout << " local var - PASSED\n";
	test_local_var_and_set();	std::cout << " local set var & set - PASSED\n";
	test_byte();				std::cout << " byte - PASSED\n";
	test_literal();				std::cout << " literal - PASSED\n";

	// OK
	std::cout << "OK\n";

}
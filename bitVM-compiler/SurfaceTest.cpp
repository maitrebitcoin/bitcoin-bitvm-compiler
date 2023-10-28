// tests of the compiler

#include "Compiler.h"
#include "Error.h"
#include "Circuit.h"
#include <iostream>

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

	// OK
	std::cout << "OK\n";

}
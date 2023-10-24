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

// test 1 circuit
//<input_01> : ex : "01"
void _test_circuit(Circuit& circuit, const char* inputs, const char* expected_result) //, const std::vector<Bit>& expected_result)
{
	// test the circuit
	std::cout << "Input : " << inputs;

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
			std::cout << " - FAILED ***\n";
			return;
		}
		i++;
	}

	// output result
	std::cout << " - PASSED : ";
	std::string s = bits_to_string(test_result);
	std::cout << s << "\n";
}



void test_xor_gate(void)
{
	// ccmpile the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file("./sample/test_xor.bvc");
	if (!result.ok) {
		std::cout << " - FAILED ***\n";
		std::cout << result.error.message << "\n";
		return;
	}
	// test the circuit
	_test_circuit(result.circuit, "00", "0");
	_test_circuit(result.circuit, "01", "1");
	_test_circuit(result.circuit, "10", "1");
	_test_circuit(result.circuit, "11", "0");
}

// run all tests
void run_all_test(void) {

	test_xor_gate();
}
// Creator: MaitreBitcoin
// Creation date: 23/10/2023
// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.

// Usage :
//   bitVM-compiler <source_file_name>

// compilee a program compatible with bitVM

#include <iostream>
#include <sstream>
#include <fstream>
#include "Circuit.h"
#include "Gate.h"
#include "Bits.h"
#include "Compiler.h"
#include "SufaceTest.h"

// main entry point
int main(int argc, char* argv[])
{

	// need help ?
	if (   argc<2 
		|| strcmp(argv[1], "-help")==0) {
		std::cout << "Usage : bitVM-compiler <source_file_name>\n";
		return 1;
	}
	// run tests ?
	if (strcmp(argv[1], "-test") == 0) {
		run_all_test();
		return 0;
	}


	// get file name from command line : 1s argument
	const char* file_name = argv[1];

	// ccmpile  the circuit
	Compiler::Result result = Compiler::compile_circuit_from_file(file_name);
	if (!result.ok) {
		std::cout << "Compilation failed.\n";
		std::cout << result.error.message << "\n";
		return 1;
	}

	// TODO
	// save the circuit in a file


}


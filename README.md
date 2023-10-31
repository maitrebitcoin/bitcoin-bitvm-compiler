# bitcoin-bitvm-compiler 

Generate a logical circuit for BitVM Bitcoin from a C like source code


Examples :
```c
// simple test program
byte main(byte a,byte b) 
{
	byte local_a_and_b = a & b;
	return local_a_and_b; 
}
```

generate :
```
    bitvmcompiler.exe source.bvc export.txt
```

output : 
```
16 16
16 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
8 17 19 21 23 25 27 29 31
1 16 1 0 8 NAND
1 17 1 16 16 NAND
1 18 1 1 9 NAND
1 19 1 18 18 NAND
1 20 1 2 10 NAND
1 21 1 20 20 NAND
1 22 1 3 11 NAND
1 23 1 22 22 NAND
1 24 1 4 12 NAND
1 25 1 24 24 NAND
1 26 1 5 13 NAND
1 27 1 26 26 NAND
1 28 1 6 14 NAND
1 29 1 28 28 NAND
1 30 1 7 15 NAND
1 31 1 30 30 NAND

```

Tested and build with Visual Studio 2022

# Notes

Bits are in low endian (x86 format)

# Current todo list :

- write the language documentation
- array support
- operators : \* / % 
- procedure calls
- if statement
- loop statement
- basic function stdlib (sha256, shnor,... )
- linux : create a makefile
- check for unused inputs instead of a "no more gates" error
- command line option to test the generated circuit
- optimise the generated circuit

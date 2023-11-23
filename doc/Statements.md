# Statements

The current statements are supported:

## local variable declaration

declare a new local var in the current scope

example :
```c
	int8 local_var;
```

## Struct type declaration

declare a new stucrture to be used as a user type

example :
```c
struct Header {
	int8 a;
	int8 b;
};
int8 main(Header st_ab) 
```


## Assignment

set a variable in the current scope

example: 
```c
	var = 1234;
```

## Increment / Decrement

Only supported as a single statement
not a operator like in C/C++

example: 
```c
	var++;
	var--;
```


## Local variable declaration and assignment

declare and set a variable in the current scope 

example: 
```c
	int8 local_var = 0;
```

## Return

set the return value of the circuit
every bloc **must** end with a return statment.

example: 
```c
	return 0;
```

## If

execute a code bloc depending of an expression that must be boolean
internally the compiler generate 2 sub circuits for each possible bloc

example :
```c
	if (val==42) {
		return 1;
	}
	return 0;
```

## For 

Loops are restricted because a new circuit must be gneerated for each step

- start and end values must be litteral int 
- step statement must be i++ or i--
- loop variable must be declared in the init statement
- loop variable must be integer
- loop variable is const, it cannot be modified in the loop

example :
```c
	int32 sum=0;
	for (int8 i=0;i<32;i++) {
		sum = sum+i;
	}
	return sum;
```

# Statements

The current statements are supported:

## local variable declaration

declare a new local var in the current scope

```c
	int8 local_var;
```

## Assign

set a variable in the current scope
example: 
```c
	var = 1234;
```

## Local variable declaration and assign

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

```c
	if (val==42) {
		return 1;
	}
	return 0;
```



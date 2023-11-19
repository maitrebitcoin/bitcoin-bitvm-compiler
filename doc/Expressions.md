# Expressions

An expression is a sequence of operators and their operands

Expressions can be composed of variables, liteals, operators and parenthesis.

ex : 
```c
	local_var + 1
	var1 - var2 + var3
	(var1 > 1) & (var3 < 2)
```

The following operators are curently supported

|operator | description    | operand type |  result      |
|---------|----------------|--------------|--------------|
|!		  |boolean not	   | int or bool  | int or bool  |
|&		  |logical and	   | int or bool  | int or bool  |
||		  |logical or	   | int or bool  | int or bool  |
|^		  |logical xor	   | int or bool  | int or bool  |
|<		  |lower		   | int		  | bool		 |
|<=		  |lower or equal  | int		  | bool		 |
|>		  |greater		   | int		  | bool		 |
|>=		  |greater or equal| int		  | bool		 |
|<<		  |bit shit left   | int		  | int			 |		
|>>		  |bit shit right  | int		  | int			 |	
|+		  |addition        | int		  | int			 |	
|-		  |substaction     | int		  | int			 |	

operator precedence is the same as C/C++


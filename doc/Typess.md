# Types

The following native types are supported :

| name		| description              | 
|-----------|--------------------------|
| bool		| boolean / bit	           |
| int8		| signed 8 bits integer    |
| uint8		| unsigned 8 bits integer  |
| int32		| signed 32 bits integer   |
| uint32	| unsigned 32 bits integer |
| int64		| signed 64 bits integer   |
| uint64	| unsigned 64 bits integer |
| int256	| signed 256 bits integer  |
| uint256	| unsigned 256 bits integer|

# Structures

Structures are supported, the following syntax is used :
```
struct Header {
	int8 a;
	int8 b;
};
Header my_header;
my_header.a = 1;
```

# Arrays

Fixed sized arrays are supported, the following syntax is used :
```
type[<size>]
type can be any of the supported types
```

example :
```
	int8[10] my_array;
	int8 sum=0;
	for (int8 i=0; i<10;i++) {
		sum = sum + my_array[i];
	}
```

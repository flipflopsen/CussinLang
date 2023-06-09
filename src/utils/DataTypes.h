#ifndef DATATYPES_H
#define DATATYPES_H

enum DataType
{
	DT_UNKNOWN,
	DT_VOID,
	DT_BOOL,
	DT_CHAR,
	DT_I8,
	DT_I32,
	DT_I64,
	DT_DOUBLE,
	DT_FLOAT,
	DT_STRUCT,

	/* Pointers */
	DT_POINTER_TO_INT,
	DT_POINTER_TO_DOUBLE,
	DT_POINTER_TO_CHAR,
	DT_POINTER_TO_STRUCT,
	DT_POINTER_TO_FLOAT,
	DT_OTHER
};

#endif
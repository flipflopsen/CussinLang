#ifndef UTIL_H
#define UTIL_H

#define VarToString(var) #var

#define ArrayCount(array) sizeof(array) / sizeof(array[0])

#include <stdio.h>
#include <cstring>

//Note: Must pass a null terminated string!


//Note: Must pass a null terminated string!
inline bool strcompare(char* first, char const* second)
{
	bool result;

	//NOTE: if they start the same but one is shorter, this still returns true
	// so we check their size (num of chars before the null terminator)
	for (int i = 0; first[i] != '\0' || second[i] != '\0'; i++)
	{
		if (first[i] != second[i])
		{
			return false;
		}
	}
	if (strlen(first) != strlen(second)) return false;

	return true;
}

inline char const* strcopy(char* destination, char const* source)
{
	//Note: very basic implementation, does not check for overflow
	// as the rest of the app is limited to char[256]
	char* temp = destination;

	while (*source != '\0')
	{
		*destination = *source;
		destination++;
		source++;
	}

	*destination = '\0';

	return temp;
}

inline void GetInput(char* dest)
{
	char buffer[256];
	gets_s(buffer, 256); //Note: gets_s returns a null terminated string
	strcopy(dest, buffer);
}

inline int strtoint(char* string)
{
	if (*string == '\0')
		return 0;

	int result = 0;
	int sign = 1;

	while (*string != '\0')
	{
		result = (result * 10) + (*string - '0');
		string++;
	}
	return result;
}

#endif
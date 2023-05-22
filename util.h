#ifndef UTIL_H
#define UTIL_H

#define VarToString(var) #var

#define ArrayCount(array) sizeof(array) / sizeof(array[0])

#include <stdio.h>
#include <cstring>

inline bool strcompare(char* first, char const* second)
{

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
	char buffer[2048];
	gets_s(buffer, 2048);
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
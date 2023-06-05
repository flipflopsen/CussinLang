#ifndef BINOPPREC_H
#define BINOPPREC_H

#include <map>

inline std::map<char, int> BinopPrecedence =
{
	{'=', 2},
	{'<', 10},
	{'+', 20},
	{'-', 20},
	{'*', 40},
};

#endif
#ifndef STRINGUTILS_H_INCLUDED
#define STRINGUTILS_H_INCLUDED

#include <string>
#include <vector>
#include <boost/tokenizer.hpp>

class StringUtils
{

	public:
		static std::vector<std::string> Split( const std::string & Msg );
		static std::vector<std::string> Split( const std::string & Msg, const std::string & Separators );
		static void StringToChar(std::string s, char* c);
};

#endif

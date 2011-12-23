#include "StringUtils.h"

#include <iostream>

using namespace std;

vector<string> StringUtils::Split( const std::string & Msg )
{
	vector<string> v;

    // utiliser le tokenizer par d�faut
    boost::tokenizer<> tok( Msg );
    // it�rer la s�quence de tokens
    for ( boost::tokenizer<>::const_iterator i = tok.begin();
          i != tok.end();
          ++i )
    {
		v.push_back(*i);
    }

	return v;
}

vector<string> StringUtils::Split( const std::string & Msg, const std::string & Separators )
{
	vector<string> v;

    // typedef pour all�ger l'�criture
    typedef boost::tokenizer<boost::char_separator<char> > my_tok;
    // s�parateur personnalis�E
    boost::char_separator<char> sep( Separators.c_str() );
    // construire le tokenizer personnalis�E
    my_tok tok( Msg, sep );
    // it�rer la s�quence de tokens
    for ( my_tok::const_iterator i = tok.begin();
          i != tok.end();
          ++i )
    {
		v.push_back(*i);
    }

	return v;
}

void StringUtils::StringToChar(std::string s, char* c)
{
	for(unsigned int i = 0 ; i < s.size() ; i++)
	{
		c[i] = s[i];
	}
	c[s.size()] = '\0';
}
#ifndef __STRINGUTIL_H__
#define __STRINGUTIL_H__

class StringUtil
{
public:
	static string toString( const char *str, size_t len );
	static string toString( float f );

	static string getExtension( const string &filename );
	static string stripQuotes( const string &str );
};

#endif	// __STRINGUTIL_H__
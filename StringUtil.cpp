#include "Common.h"
#include "StringUtil.h"

string StringUtil::toString( const char *str, size_t len )
{
	return string( str, MIN( strlen( str ), len ) );
}

string StringUtil::toString( float f )
{
	static char tmp[32];
	sprintf( tmp, "%f", f );
	return string( tmp );
}

string StringUtil::getExtension( const string &filename )
{
	size_t pos = filename.find_last_of( '.' );
	if ( pos == string::npos )
		return "";

	string ext = filename.substr( pos+1 );
	std::transform( ext.begin(), ext.end(), ext.begin(), static_cast<int(*)(int)>(std::tolower) );
	return ext;
}

string StringUtil::stripQuotes( const string &str )
{
	size_t len = str.size();

	if ( str[0] == '\"' && str[len-1] == '\"' )
		return str.substr( 1, len-2 );

	return str;
}

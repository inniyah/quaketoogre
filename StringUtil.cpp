/*
-------------------------------------------------------------------------------
Copyright (c) 2009-2010 Nico de Poel

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-------------------------------------------------------------------------------
*/
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

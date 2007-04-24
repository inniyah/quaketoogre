#include "Common.h"
#include "XmlWriter.h"

XmlWriter::XmlWriter(): 
	mIndent(0)
{
}

XmlWriter::operator string() const
{
	return mStream.str();
}

ostream &operator<<( ostream &os, const XmlWriter &xw )
{
	os << (string)xw;
	return os;
}

string XmlWriter::getIndent( int indent )
{
	string result = "";
	while ( indent-- )
		result += "    ";
	return result;
}

void XmlWriter::openTag( const string &tagName )
{
	mStream << endl << getIndent(mIndent) << "<" << tagName << ">";
	mTagStack.push_back( tagName );
	mIndent++;
}

void XmlWriter::openTag( const string &tagName, const XmlWriter::Attributes &attributes )
{
	mStream << endl << getIndent(mIndent) << "<" << tagName << " ";

	const AttributeMap &atts = attributes.getAttributes();
	AttributeMap::const_iterator i;
	for ( i = atts.begin(); i != atts.end(); ++i )
	{
		mStream << i->first << "=\"" << i->second << "\" ";
	}

	mStream << ">";
	mTagStack.push_back( tagName );
	mIndent++;
}

void XmlWriter::closeTag()
{
	mIndent--;
	mStream << endl << getIndent(mIndent) << "</" << mTagStack.back() << ">";
	mTagStack.pop_back();
}

void XmlWriter::writeData( const string &data )
{
	mStream << data;
}

void XmlWriter::writeData( const char *data, size_t len )
{
	writeData( string( data, len ) );
}

void XmlWriter::writeData( const short &data )
{
	mStream << data;
}

void XmlWriter::writeData( const int &data )
{
	mStream << data;
}

void XmlWriter::writeData( const float &data )
{
	mStream << data;
}

void XmlWriter::Attributes::set( const string &name, const string &value )
{
	mAtts[name] = value;
}

void XmlWriter::Attributes::set( const string &name, const char *value, size_t len )
{
	mAtts[name] = string( value, MIN( strlen(value), len ) );
}

void XmlWriter::Attributes::set( const string &name, const short &value )
{
	char tmp[16];
	sprintf( tmp, "%i", value );
	mAtts[name] = tmp;
}

void XmlWriter::Attributes::set( const string &name, const int &value )
{
	char tmp[32];
	sprintf( tmp, "%i", value );
	mAtts[name] = tmp;
}

void XmlWriter::Attributes::set( const string &name, const float &value )
{
	char tmp[32];
	sprintf( tmp, "%f", value );
	mAtts[name] = tmp;
}

const string &XmlWriter::Attributes::get( const string &name ) const
{
	return mAtts.find( name )->second;
}

const XmlWriter::AttributeMap &XmlWriter::Attributes::getAttributes() const
{
	return mAtts;
}

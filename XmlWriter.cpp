#include "Common.h"
#include "XmlWriter.h"

static char tmp[32];

XmlWriter::XmlWriter()
{
	mDoc.LinkEndChild( new TiXmlDeclaration( "1.0", "", "" ) );
	mNodeStack.push_back( &mDoc );
}

XmlWriter::operator string() const
{
	TiXmlPrinter printer;
	printer.SetIndent( "    " );
	mDoc.Accept( &printer );
	return printer.Str();
}

ostream &operator<<( ostream &os, const XmlWriter &xw )
{
	os << xw.mDoc;
	return os;
}

void XmlWriter::openTag( const string &tagName )
{
	TiXmlElement *element = new TiXmlElement( tagName );
	
	TiXmlNode *parent = mNodeStack.back();
	parent->LinkEndChild( element );
	mNodeStack.push_back( element );
}

void XmlWriter::openTag( const string &tagName, const XmlWriter::Attributes &attributes )
{
	TiXmlElement *element = new TiXmlElement( tagName );

	const Attributes::AttributeMap &atts = attributes.getAttributes();
	Attributes::AttributeMap::const_iterator i;
	for ( i = atts.begin(); i != atts.end(); ++i )
	{
		element->SetAttribute( i->first, i->second );
	}

	TiXmlNode *parent = mNodeStack.back();
	parent->LinkEndChild( element );
	mNodeStack.push_back( element );
}

void XmlWriter::closeTag()
{
	mNodeStack.pop_back();
}

void XmlWriter::writeData( const string &data )
{
	TiXmlNode *parent = mNodeStack.back();
	parent->LinkEndChild( new TiXmlText( data ) );
}

void XmlWriter::writeData( const char *data, size_t len )
{
	writeData( string( data, len ) );
}

void XmlWriter::writeData( const short &data )
{
	sprintf( tmp, "%i", data );
	writeData( tmp );
}

void XmlWriter::writeData( const int &data )
{
	sprintf( tmp, "%i", data );
	writeData( tmp );
}

void XmlWriter::writeData( const float &data )
{
	sprintf( tmp, "%f", data );
	writeData( tmp );
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
	sprintf( tmp, "%i", value );
	mAtts[name] = tmp;
}

void XmlWriter::Attributes::set( const string &name, const int &value )
{
	sprintf( tmp, "%i", value );
	mAtts[name] = tmp;
}

void XmlWriter::Attributes::set( const string &name, const float &value )
{
	sprintf( tmp, "%f", value );
	mAtts[name] = tmp;
}

const string &XmlWriter::Attributes::get( const string &name ) const
{
	return mAtts.find( name )->second;
}

const XmlWriter::Attributes::AttributeMap &XmlWriter::Attributes::getAttributes() const
{
	return mAtts;
}

#include "Common.h"
#include "XmlWriter.h"

static char tmp[32];

XmlWriter::XmlWriter()
{
	mDoc.LinkEndChild( new TiXmlDeclaration( "1.0", "", "" ) );
	mNodeStack.push_back( &mDoc );
}

bool XmlWriter::saveFile( const string &filename ) const
{
	return mDoc.SaveFile( filename );
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

TiXmlElement *XmlWriter::openTag( const string &tagName )
{
	TiXmlElement *element = new TiXmlElement( tagName );
	
	TiXmlNode *parent = mNodeStack.back();
	parent->LinkEndChild( element );
	mNodeStack.push_back( element );

	return element;
}

void XmlWriter::closeTag()
{
	mNodeStack.pop_back();
}

string XmlWriter::toStr( const char *str, size_t len )
{
	return string( str, MIN( strlen( str ), len ) );
}

string XmlWriter::toStr( float f )
{
	sprintf( tmp, "%f", f );
	return string( tmp );
}


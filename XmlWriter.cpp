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
#include "XmlWriter.h"

XmlWriter::XmlWriter(): mDocType( NULL )
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

void XmlWriter::cancelTag()
{
    TiXmlNode *node = mNodeStack.back();
    mNodeStack.pop_back();
    
    TiXmlNode *parent = node->Parent();
    parent->RemoveChild( node );
}

void XmlWriter::setDocType( const string &type, const string &dtd )
{
    if ( !mDocType )
    {
    	mDocType = mDoc.InsertAfterChild( mDoc.FirstChild(), TiXmlUnknown() );
	}

    stringstream ss;
    ss << "!DOCTYPE " << type << " SYSTEM \"" << dtd << "\"";

    mDocType->SetValue( ss.str() );
}


#ifndef __XMLWRITER_H__
#define __XMLWRITER_H__

class XmlWriter
{
public:
	XmlWriter();

	bool saveFile( const string &filename ) const;
	operator string() const;
	friend ostream &operator<<( ostream &os, const XmlWriter &xw );

	TiXmlElement *openTag( const string &name );
	void closeTag();
	
private:
	TiXmlDocument mDoc;
	vector<TiXmlNode*> mNodeStack;
};

#endif

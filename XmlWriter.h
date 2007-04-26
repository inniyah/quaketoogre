#ifndef __XMLWRITER_H__
#define __XMLWRITER_H__

class XmlWriter
{
public:
	XmlWriter();

	operator string() const;
	friend ostream &operator<<( ostream &os, const XmlWriter &xw );

protected:
	class Attributes
	{
	public:
		typedef map<string,string> AttributeMap;
	
		void set( const string &name, const string &value );
		void set( const string &name, const char *value, size_t len );
		void set( const string &name, const short &value );
		void set( const string &name, const int &value );
		void set( const string &name, const float &value );
		const string &get( const string &name ) const;
		const AttributeMap &getAttributes() const;
	private:
		AttributeMap mAtts;
	};

	void openTag( const string &tagName );
	void openTag( const string &tagName, const Attributes &attributes );
	void closeTag();

	void writeData( const string &data );
	void writeData( const char *data, size_t len );
	void writeData( const short &data );
	void writeData( const int &data );
	void writeData( const float &data );
	
	TiXmlDocument mDoc;
	vector<TiXmlNode*> mNodeStack;
};

#endif

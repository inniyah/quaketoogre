#ifndef __MD3XMLWRITER_H__
#define __MD3XMLWRITER_H__

#include "MD3Structure.h"
#include "XmlWriter.h"

class MD3XmlWriter: public XmlWriter
{
public:
	MD3XmlWriter( const MD3Structure &model );

private:
	void buildXml();
	void writeFrame( const MD3Frame &frame );
	void writeTag( const MD3Tag &tag );
	void writeShader( const MD3Shader &shader );
	void writeTriangle( const MD3Triangle &triangle );
	void writeTexCoord( const MD3TexCoord &texCoord );
	void writeVertex( const MD3Vertex &vertex );
	void writeMesh( const MD3Mesh &mesh );

	void writeVec2( const float vec[2] );
	void writeVec3( const float vec[3] );

	const MD3Structure &mModel;
};

#endif

/*
	This a new inventor node used for show textures for
	stereo vision

	author:		henry Portilla
	date:		january/7/2007

	Special grant 
	from:		God

	This software is under the GPL license

	The code is based on the book: the inventor toolmaker
*/
/**	
 *	This is a code for an Inventor node made thanks to God


*/
#ifndef	_SOSTEREOTEXTURE_H_
#define _SOSTEREOTEXTURE_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>				// 	GL extensions
#include <GL/glx.h>				//	Binding extensions with pointers to opengl in linux
						//	see opengl red book pag 715 in the 5 edition

#include  <Inventor/SbLinear.h>			//	vectors, planes and others
#include  <Inventor/fields/SoSFFloat.h>
#include  <Inventor/fields/SoSFUInt32.h>
#include  <Inventor/fields/SoSFImage.h>		//	for load images
#include  <Inventor/nodes/SoShape.h>
#include  <Inventor/nodes/SoPerspectiveCamera.h>
#include  <Inventor/Qt/SoQtGLWidget.h>		//	to do the swap of buffers
//	note SoSubNode.h has the macros definitions to
//	create the new nodes
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
//GLuint bufferID;				//	PBO (pixel_buffer_object) name	

class SoStereoTexture :public SoShape
{

//	macro for define methods and the static variables
SO_NODE_HEADER(SoStereoTexture);

public:

// Fields

SoSFFloat	width;		//	width of images
SoSFFloat	heigh;		//	heigh of images
SoSFImage	imageL;
SoSFImage	imageR;
unsigned char   *image_L;	//	left image in unsigned char format
unsigned char   *image_R;	//	right image
SoSFFloat	IOD;		//	inter-ocular distance

// Initialize the class
static void initClass();	//	to define the type of information of the class
				//	and initialize in the correct state
				//	the different elements of openInventor

// constructor
SoStereoTexture();

// methods

void  synchronize();		//	synchronize the two images to show
				//	using rtsp /rtp information
void  show();			//	show the two images

protected:
//	define the actions and inherit others from parent class
				
				//	render the Quads for the images
virtual void GLRender(SoGLRenderAction *action);
				//	generates triangles representing the quads
virtual void generatePrimitives(SoAction *action);
				//	compute bounding box, required
virtual void computeBBox(SoAction *action,SbBox3f &box,SbVec3f &center);

private:

//	normals of quads
SoStereoTexture *pthis;
GLuint texName;			//	texture name

//	OPENGL EXTENSIONS       

//	define procedures for PBO according to glext.h
PFNGLGENBUFFERSPROC glGenBuffersARB;	// define a glGenBufferARB according to opengl Extensions procedures
PFNGLBINDBUFFERPROC glBindBufferARB;	// define an association function for the PBO (pixel buffer object)
PFNGLBUFFERDATAPROC glBufferDataARB;	// define load of data procedure
PFNGLDELETEBUFFERSPROC glDeleteBuffersARB;// delete the object
PFNGLMAPBUFFERPROC glMapBufferARB; 	// pointer to memory of the PBO
PFNGLUNMAPBUFFERPROC glUnmapBufferARB;// releases the mapping

//	EXTENSIONS REQUEST

GLboolean isExtensionSupported(char *pExtensionName);

// destructor
virtual ~SoStereoTexture();

};

#endif

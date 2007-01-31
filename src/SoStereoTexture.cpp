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


#include <Inventor/SoPrimitiveVertex.h> 	// save information of each vertex point, 
						// normal, texture coordinates, material index
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLTextureImageElement.h>
#include <Inventor/misc/SoState.h>		// to get the actual state of elements
#include "SoStereoTexture.h"			// node definitions

//	Macro for declare the methods and start the static variables
SO_NODE_SOURCE(SoStereoTexture);

//	Initialize the class
void SoStereoTexture::initClass()
{
	//	initialize type id variables
	SO_NODE_INIT_CLASS(SoStereoTexture,SoShape,"Shape");
	//	Enable state elements
	//SO_ENABLE(SoGLRenderAction,SoGLTextureImageElement);
}
//	Constructor
SoStereoTexture::SoStereoTexture()
{
	pthis = this;		// pointer to the same object?

	SO_NODE_CONSTRUCTOR(SoStereoTexture);	// define the fields and set the default values
	//	add the fields with default values
	SO_NODE_ADD_FIELD(width,(20));		
	SO_NODE_ADD_FIELD(heigh,(20));
	SO_NODE_ADD_FIELD(imageL,(SbVec2s(0,0),3,0));
	SO_NODE_ADD_FIELD(imageR,(SbVec2s(0,0),3,0));
	//SO_NODE_ADD_FIELD(imageL,(NULL));
	//SO_NODE_ADD_FIELD(imageR,(NULL));
	SO_NODE_ADD_FIELD(IOD,(6.0));
	pthis->image_L = NULL;
	pthis->image_R = NULL;

	//	if is the first time the constructor is called
	//	setup opengl extensions
	if (SO_NODE_IS_FIRST_INSTANCE())
	{
	//	USING OPENGL EXTENSIONS       
		
	//	define procedures for PBO according to glext.h
	//	use glx.h to do the correct binding through glXGetProcAddress(GLubyte*) in linux
	//	and to use the opengl extensions
	

	// define a glGenBufferARB according to opengl Extensions procedures
		glGenBuffersARB = (PFNGLGENBUFFERSPROC)glXGetProcAddress((const unsigned char*)"glGenBuffersARB");	
	// define an association function for the PBO (pixel buffer object)
		glBindBufferARB = (PFNGLBINDBUFFERPROC)glXGetProcAddress((const unsigned char*)"glBindBufferARB");	
	// define load of data procedure
		glBufferDataARB = (PFNGLBUFFERDATAPROC)glXGetProcAddress((const unsigned char*)"glBufferDataARB");	
	// delete the object
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSPROC)glXGetProcAddress((const unsigned char*)"glDeleteBuffersARB");
	// pointer to memory of the PBO
		glMapBufferARB = (PFNGLMAPBUFFERPROC)glXGetProcAddress((const unsigned char*)"glMapBufferARB"); 	
	// releases the mapping	
		glUnmapBufferARB = (PFNGLUNMAPBUFFERPROC)glXGetProcAddress((const unsigned char*)"glUnmapBufferARB");
		
	}
}
//	Destructor
SoStereoTexture::~SoStereoTexture()
{
}
//	Methods

//	SoGLRenderAction 

void SoStereoTexture::GLRender(SoGLRenderAction *action)
{
	// acces the state from the  action
	//SoState *state = action->getState();
	const unsigned char *version;
	static GLuint texName;			//	texture name
	GLuint bufferID;			//	PBO (pixel_buffer_object) name	
	void *pboMemory;
	/*
	int c;
	static GLubyte subImage[512][512][3];
	for (int i=0;i<512;i++)
	{
		for(int j=0;j<512;j++)
		{
			c =(((i&0x04)==0)^((j&0x04)==0))*255;
			subImage[i][j][0] = (GLubyte)c;
			subImage[i][j][1] = (GLubyte)c;
			subImage[i][j][2] = (GLubyte)255;
		}
	}*/
	//imageL = (unsigned char*)subImage;
	// ask if this should be rendered
	if(!shouldGLRender(action))
	{
		return;
	}
//	beginSolidShape(action);	//	start draw object with optimizations
					//	like backculling

	
	//	openGL code for textures

	glPushMatrix();			//	save openGl state

	version = glGetString(GL_VERSION);
	printf("The openGL version is %s\n",version);
	//	see for opengl version


	//	using PBO according to the extension specification

	//	start with a null image
	//glBindBufferARB(GL_PIXEL_UNPACK_BUFFER, 0);
        // glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, 720, 576, 0,
	//                           GL_RGB, GL_UNSIGNED_BYTE, NULL);	

        // Create and bind texture image buffer object

	glGenBuffersARB(1, &bufferID);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, bufferID);
	//	reset contents of PBO
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_EXT,720*576*3,NULL,GL_STREAM_DRAW);

 	//glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, 720, 576, 0,
	//                           GL_RGB, GL_UNSIGNED_BYTE, NULL);

	// Setup texture environment
	
	glClearColor(0.0,0.0,0.0,0.0);			//	clear color to black when the buffers will be cleaned
	glShadeModel(GL_FLAT);				//	solid colors not shading
	glEnable(GL_DEPTH_TEST);			//	enable depth test?
	//	This function configure how the pixels are unpacked from memory
	//glPixelStorei(GL_UNPACK_ALIGNMENT,1);		// 	REVIEW FOR PERFORMANCE, but this use boundaries of 1 Byte
							// 	in test this is the better	
	//glGenTextures(1,&texName);			//	assigns a name for the texture from the texname array
							//	create a texture object and assigns a name  
	//glBindTexture(GL_TEXTURE_RECTANGLE_NV,texName);		
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);// how to place the texture
	//texture parameters
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_WRAP_S,GL_CLAMP);//	extend the texture in S coord
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_WRAP_T,GL_CLAMP);//	and T too
	//	REVIEW IF THIS  FILTERS ARE NEEDED WITH GL_TEXTURE_RECTANGLE_NV
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_NEAREST);	//	on view magnification
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_NEAREST); 	//	on view mimimization
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, 720, 576, 0,
                            GL_RGB, GL_UNSIGNED_BYTE, NULL);
	//	OPENGL 2 extension
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//			PBO SETUP
	/*
	glGenBuffersARB(1,&bufferID);	//	create the buffer for the PBO
	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,bufferID); 
							// allocate a chunk of memory using a PBO pixel_buffer_object 
 							// view Fast Texture Transfer and using VBOs articles
							// from nvidia developer web site
							// see also the reference to the lesson 45 NeHe web site
							// openGL tutorials dec 2006
	*/

	//	modify the data in the GPU memory

	SbVec2s size; int components;
	size.setValue(720,576);
	components=3;


	//	reset contents of PBO
	//glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_EXT,720*576*3,NULL,GL_STREAM_DRAW);
	
	//	define a pointer that maps to memory at the GPU memory card						

	pboMemory = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT,GL_WRITE_ONLY);

	
	memcpy(pboMemory,pthis->imageL.getValue(size,components),720*576*3);	

	//	Unmap the PBO buffer	

	glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT);

	//	DRAW THE TEXTURE//pthis->imageL.getValue(size,components)

	glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,720,576,GL_RGB,GL_UNSIGNED_BYTE,BUFFER_OFFSET(0));
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,0.0);
			glVertex3f(-width.getValue(), heigh.getValue()/2.0,0.0);
		glTexCoord2f(0.0,heigh.getValue());
			glVertex3f(-width.getValue(),-heigh.getValue()/2.0 ,0.0);
		glTexCoord2f(width.getValue(),heigh.getValue());
			glVertex3f( 0.0, -heigh.getValue()/2.0,0.0);
		glTexCoord2f(width.getValue(),0.0);
			glVertex3f( 0.0,  heigh.getValue()/2.0,0.0);
	glEnd();

	

	//	Delete the buffer	

	glDeleteBuffersARB(1,&bufferID);

	//glBindBufferARB(GL_PIXEL_UNPACK_BUFFER, 0);
/*
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,720,576,GL_RGB,GL_UNSIGNED_BYTE,pthis->imageR.getValue(size,components));
	glBegin(GL_QUADS);
		glTexCoord2f(0.0,0.0);
			glVertex3f( 0.0, heigh.getValue()/2.0,0.0);
		glTexCoord2f(0.0,heigh.getValue());	
			glVertex3f( 0.0,-heigh.getValue()/2.0 ,0.0);
		glTexCoord2f(width.getValue(),heigh.getValue());
			glVertex3f( width.getValue(), -heigh.getValue()/2.0,0.0);
		glTexCoord2f(width.getValue(),0.0);
			glVertex3f( width.getValue(),  heigh.getValue()/2.0,0.0);
	glEnd();
*/
	glPopMatrix();			//	restore opengl state

//	endSolidShape(action);
	
}
//	generate triangles representing texture planes
void SoStereoTexture::generatePrimitives(SoAction *action)
{
	
	
}
//	computs the bounding box and the center of the quads squares
void SoStereoTexture::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center)
{
	//	maximum extension of the box
	SbVec3f min,max;	
	//	minimum
	min.setValue(-width.getValue(),
		     -heigh.getValue(),
		     -width.getValue()/heigh.getValue());
	//	maximum
	max.setValue(width.getValue(),
		     heigh.getValue(),
		     width.getValue()/heigh.getValue());
	//	set the box limits
	box.setBounds(min,max);
	//	natural center of quad square
	center.setValue(0.0,0.0,0.0);

}
void SoStereoTexture::synchronize()
{

}
void SoStereoTexture::show()
{

}



















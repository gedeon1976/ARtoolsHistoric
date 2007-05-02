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

	//	initialise pointers to extension
	glBindBufferARB=NULL;
	glBufferDataARB=NULL;
	glDeleteBuffersARB=NULL;
	glGenBuffersARB=NULL;
	glMapBufferARB=NULL;
	glUnmapBufferARB=NULL;
	//	if is the first time the constructor is called
	//	setup opengl extensions
	if (SO_NODE_IS_FIRST_INSTANCE())
	{
	//	USING OPENGL EXTENSIONS       
		
	//	define procedures for PBO according to glext.h
	//	use glx.h to do the correct binding through glXGetProcAddress(GLubyte*) in linux
	//	and to eanabled to use the opengl extensions
	
	

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

//	PRIVATE METHODS

GLboolean SoStereoTexture::isExtensionSupported(char *pExtensionName)
{
	const unsigned char *pVersion, *pstrExtensions;
	const unsigned char *pStart;
	unsigned char *pszWhere,*pszTerminator;
	
	//glPushMatrix();
	
	//	look for the opengl version and the extensions supported
	pVersion = glGetString(GL_VERSION);
//	printf("The openGL version is %s\n",pVersion);
	//	strExtensions =glGetString(GL_EXTENSIONS);
	//printf("The openGL extensions supported are %s\n",strExtensions);

	//	look for the PBO openGL extension see pag 598 red book 5 edition
	//ExtensionName = (const GLubyte*)"GL_EXT_pixel_buffer_object";
	//EXT_SUPPORTED = gluCheckExtension(ExtensionName,strExtensions);
	
	//pExtensionName = (char*)"GL_EXT_pixel_buffer_object";

	//	Extensions names should not have spaces
	pszWhere =(unsigned char*)strchr(pExtensionName,' ');
	if(pszWhere||pExtensionName == '\0')
	{
		return GL_FALSE;	//	no extension name asked.
	}
	//	Get the extension string
	pstrExtensions = glGetString(GL_EXTENSIONS);
	
	//	Search the extensions string for an exact copy of extension queried

	pStart= pstrExtensions;
	for(;;)
	{
		// search for the extension in the extension string
		pszWhere=(unsigned char*)strstr((const char*)pStart,pExtensionName);
		if(!pszWhere)
			break;
		pszTerminator= pszWhere + strlen(pExtensionName);
		if(pszWhere == pStart || *(pszWhere-1)==' ')
		{
			if(*pszTerminator == ' ' || *pszTerminator == '\0')
			{
				return GL_TRUE;
			}
		}
	pStart = pszTerminator;
	}
	
	//glPopMatrix();

}


//	PROTECTED METHODS

//	SoGLRenderAction 

void SoStereoTexture::GLRender(SoGLRenderAction *action)
{
	// access the state from the  action
	//SoState *state = action->getState();
	
	//static GLuint texName;		//	texture name
	GLuint bufferID[2];			//	PBO (pixel_buffer_object) name	
	void *pboMemoryL,*pboMemoryR;
	GLboolean isPBO,*isStereo;		//	boolean flags
	GLXDrawable glXSurface;			//	glX variables to do the SwapBuffer
	Display *pDisplay;	
		
	//	Quad Buffer TEST
	//**********************************************************************************
	
	float depthZ = 150.0;                           // depth of the object model

      //double fovy =  45;                              // field of view in y-axis, 
	double theta = 46;				// aperture angle as FOV
	double aspect = double(720)/double(576);  	// screen aspect ratio
	double nearZ = 75.0;                            // near clipping plane
	double farZ = 400.0;                            // far clipping plane
	double screenZ =100.0;                          // screen projection plane
	//double IOD = 5.0;                             // intraocular distance
	double beta;					// vergence angle
	double l,r,b,t,K;				// limits for glfrustum delimitation	
	
	//	frustum calculation for glFrustum
	beta = 2*atan(IOD.getValue()/(2*screenZ));	// get vergence angle from IOD
	t =  nearZ*tan(theta/2);			// top coordinate
	b = -nearZ*tan(theta/2);			// bottom coordinate
	K = (0.5*IOD.getValue()*nearZ)/screenZ;		// to get l and r values
	//***********************************************************************************
	// ask if this should be rendered
	if(!shouldGLRender(action))
	{
		return;
	}
//	beginSolidShape(action);	//	start draw object with optimizations
					//	like backculling

	// 	look for Extension supported

	//isPBO = isExtensionSupported("GL_EXT_pixel_buffer_object");	//	is PBO supported?
	isPBO = GL_FALSE;
	
	//	check if stereo is supported

	glGetBooleanv(GL_STEREO,isStereo);
	//*isStereo = GL_FALSE;	

	//	openGL code for textures

	//	using PBO according to the extension specification

if (isPBO == GL_TRUE)	// if PBO is supported, use it when will be hardware supported, if this go slow
			// probably is software supported, remember PBO since opengl 2.1 in april/07

{
	//	start with a null image
	// glBindBufferARB(GL_PIXEL_UNPACK_BUFFER, 0);
        // glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, 720, 576, 0,
	//                           GL_RGB, GL_UNSIGNED_BYTE, NULL);	

	glPushMatrix();			//	save openGl state

        // Create and bind texture image buffer object

	//	PBO LEFT
	glGenBuffersARB(1, &bufferID[0]);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, bufferID[0]);
	//	reset contents of PBO
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB,720*576*3,NULL,GL_DYNAMIC_DRAW_ARB);

	//	PBO RIGHT
 	glGenBuffersARB(1, &bufferID[1]);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, bufferID[1]);
	//	reset contents of PBO
	glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB,720*576*3,NULL,GL_DYNAMIC_DRAW_ARB);

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
							// see also the reference to the lesson 45 in NeHe web site
							// openGL tutorials dec 2006
	*/

	//	modify the data in the GPU memory

	SbVec2s size; int components;
	size.setValue(720,576);
	components=3;
	
	//	check for stereo support

	if(*isStereo == GL_TRUE)	
	{
	
	//*******************************************************************************
	//	QUAD BUFFER INITIALIZATION
		glViewport (0, 0, 1000, 800);            		// sets drawing viewport
  		//glMatrixMode(GL_PROJECTION);
  		//glLoadIdentity();					// reset Projection matrix
		//glFrustum(576.0,576.0,720.0,720.0,nearZ,farZ);	// set frustum to see
  		//gluPerspective(fovy, aspect, nearZ, farZ);            // sets frustum using gluPerspective
	
		// Off-axis method for rendering

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		r = aspect*t - K;	
		l = aspect*b - K;
		glFrustum(l,r,b,t,nearZ,farZ);				// delimits the frustum perspective

  		glMatrixMode(GL_MODELVIEW);
  		glDrawBuffer(GL_BACK_LEFT);           			// draw into back left buffer
  		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear color and depth buffers
  	                             
  		glLoadIdentity();                                       // reset modelview matrix
	
	
		/*	rendering method in flat screens by Paul Borke
	
		a) toe-in method (introduces vertical parallax)
		b) off axis method (correct)
		*/
	
		gluLookAt(-IOD.getValue()/2,0.0,0.0,	//set camera position  x=-IOD/2
       		  	  -IOD.getValue()/2,0.0,screenZ,//set camera "look at" x=0.0
         				0.0,1.0,0.0);   //set camera up vector
	//*******************************************************************************

		glPushMatrix();	
		glTranslatef(0.0,0.0,depthZ);

		//	reset contents of PBO
		//glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_EXT,720*576*3,NULL,GL_STREAM_DRAW);
	
		//	define a pointer that maps to memory at the GPU memory card						
		pboMemoryL = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,GL_WRITE_ONLY);
		//	write data in the GPU RAM memory
		memcpy(pboMemoryL,pthis->imageL.getValue(size,components),720*576*3);	
		//	Unmap the PBO buffer	
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);

		//	DRAW THE LEFT IMAGE
		//	PBO usage 
		glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,720,576,GL_RGB,GL_UNSIGNED_BYTE,BUFFER_OFFSET(0));
		glBegin(GL_QUADS);
			glTexCoord2f(0.0,0.0);
				glVertex3f( width.getValue()/2.0, heigh.getValue()/2.0,screenZ);
			glTexCoord2f(0.0,heigh.getValue());	
				glVertex3f( width.getValue()/2.0,-heigh.getValue()/2.0 ,screenZ);
			glTexCoord2f(width.getValue(),heigh.getValue());
				glVertex3f( -width.getValue()/2.0, -heigh.getValue()/2.0,screenZ);
			glTexCoord2f(width.getValue(),0.0);
				glVertex3f( -width.getValue()/2.0,  heigh.getValue()/2.0,screenZ);
		glEnd();
	
		glPopMatrix();

	//**************************************************************************
	// Off-axis method for rendering
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		r = aspect*t + K;	
		l = aspect*b + K;
		glFrustum(l,r,b,t,nearZ,farZ);				// delimits the frustum perspective

	//	Draw right Image in buffer

		glDrawBuffer(GL_BACK_RIGHT);                            //draw into back right buffer
	  	glMatrixMode(GL_MODELVIEW);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear color and depth buffers
  		glLoadIdentity();      					//reset modelview matrix

	

		//as for the left buffer but with camera position at:
		gluLookAt(IOD.getValue()/2, 0.0, 0.0, 
			  IOD.getValue()/2, 0.0, screenZ,            
            			       0.0, 1.0, 0.0);

                                 
	//**************************************************************************

		glPushMatrix();	
		glTranslatef(0.0,0.0,depthZ);

		//	second PBO
		pboMemoryR = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,GL_WRITE_ONLY);
		//	write data in the GPU RAM memory
		memcpy(pboMemoryR,pthis->imageR.getValue(size,components),720*576*3);	

		//	Unmap the PBO buffer	

		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);

		//	PBO usage 
		//	DRAW THE RIGHT IMAGE
		glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,720,576,GL_RGB,GL_UNSIGNED_BYTE,BUFFER_OFFSET(0));
		glBegin(GL_QUADS);
			glTexCoord2f(0.0,0.0);
				glVertex3f( width.getValue()/2.0, heigh.getValue()/2.0,screenZ);
			glTexCoord2f(0.0,heigh.getValue());	
				glVertex3f( width.getValue()/2.0,-heigh.getValue()/2.0 ,screenZ);
			glTexCoord2f(width.getValue(),heigh.getValue());
				glVertex3f( -width.getValue()/2.0, -heigh.getValue()/2.0,screenZ);
			glTexCoord2f(width.getValue(),0.0);
				glVertex3f( -width.getValue()/2.0,  heigh.getValue()/2.0,screenZ);
		glEnd();

		glPopMatrix();
	//*******************************************************************************
	//	swap Buffers
	//	see the openGL programming Guide appendix C
	glXSurface = glXGetCurrentDrawable();

	if(glXSurface =! NULL)
	{
		pDisplay = glXGetCurrentDisplay();	//	get the current display
		//if (pDisplay  NULL)
		//{
			glXSwapBuffers(pDisplay,glXSurface);
		//}
	}
	
//	glXSwapBuffers(glXGetCurrentDisplay(),glXGetCurrentDrawable());
	//glXWaitGL();			//	wait openGl execution

	//glReadBuffer(GL_BACK);
	//glDrawBuffer(GL_FRONT);
	//glCopyPixels(0, 0, 720, 576, GL_COLOR);
	//*******************************************************************************
	}
	else{	// If not stereo support

		//	define a pointer that maps to memory at the GPU memory card						
		pboMemoryL = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,GL_WRITE_ONLY);
		//	write data in the GPU RAM memory
		memcpy(pboMemoryL,pthis->imageL.getValue(size,components),720*576*3);	
		//	Unmap the PBO buffer	
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);

		//	DRAW THE LEFT IMAGE
		//	PBO usage 
		glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,720,576,GL_RGB,GL_UNSIGNED_BYTE,BUFFER_OFFSET(0));
		glBegin(GL_QUADS);
			glTexCoord2f(0.0,0.0);
				glVertex3f( -width.getValue(), heigh.getValue()/2.0,0.0);
			glTexCoord2f(0.0,heigh.getValue());	
				glVertex3f( -width.getValue(),-heigh.getValue()/2.0 ,0.0);
			glTexCoord2f(width.getValue(),heigh.getValue());
				glVertex3f( 0.0, -heigh.getValue()/2.0,0.0);
			glTexCoord2f(width.getValue(),0.0);
				glVertex3f( 0.0,  heigh.getValue()/2.0,0.0);
		glEnd();

		//	second PBO
		pboMemoryR = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,GL_WRITE_ONLY);
		//	write data in the GPU RAM memory
		memcpy(pboMemoryR,pthis->imageR.getValue(size,components),720*576*3);	
		//	Unmap the PBO buffer	
		glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);

		//	PBO usage 
		//	DRAW THE RIGHT IMAGE
		glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,720,576,GL_RGB,GL_UNSIGNED_BYTE,BUFFER_OFFSET(0));
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
	

	}
	//	End of stereo verification
	//********************************************************************************
	glPopMatrix();			//	restore opengl states

	//	Delete the buffers

	glDeleteBuffersARB(1,&bufferID[0]);
	glDeleteBuffersARB(1,&bufferID[1]);

//	glBindBufferARB(GL_PIXEL_UNPACK_BUFFER, 0);

}else
{
	//	using gl_RectangleNV parameter for textures, very fast 

	glPushMatrix();
	
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

	//	define size of images
	
	SbVec2s size; int components;
	size.setValue(720,576);
	components=3;


	//	check for stereo support

	if(*isStereo == GL_TRUE)	
	{
		//*******************************************************************************
		//	QUAD BUFFER INITIALIZATION
		glViewport (0, 0, 1000, 800);            		// sets drawing viewport
	/*
		TOE-IN method for rendering	

 		glMatrixMode(GL_PROJECTION);
  		glLoadIdentity();
		//glFrustum(576.0,576.0,720.0,720.0,nearZ,farZ);	// set frustum to see
  		gluPerspective(fovy, aspect, nearZ, farZ);              // sets frustum using gluPerspective
	*/
		// Off-axis method for rendering

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		r = aspect*t - K;	
		l = aspect*b - K;
		glFrustum(l,r,b,t,nearZ,farZ);				// delimits the frustum perspective

  		glMatrixMode(GL_MODELVIEW);
  		glDrawBuffer(GL_BACK_LEFT);           			// draw into back left buffer
  		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear color and depth buffers
  	                             
  		glLoadIdentity();                                        // reset modelview matrix

		gluLookAt(-IOD.getValue()/2,0.0,0.0,	//set camera position  x=-IOD/2
            		  -IOD.getValue()/2,0.0,screenZ,//set camera "look at" x=0.0
            				0.0,1.0,0.0);   //set camera up vector

		//*******************************************************************************
		glPushMatrix();
		glTranslatef(0.0,0.0,depthZ);

		//	DRAW THE LEFT IMAGE


		glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,720,576,GL_RGB,GL_UNSIGNED_BYTE,pthis->imageL.getValue(size,components));
		//	modifying for stereo
		glBegin(GL_QUADS);
			glTexCoord2f(0.0,0.0);
				glVertex3f( width.getValue()/2.0, heigh.getValue()/2.0,screenZ);
			glTexCoord2f(0.0,heigh.getValue());	
				glVertex3f( width.getValue()/2.0,-heigh.getValue()/2.0 ,screenZ);
			glTexCoord2f(width.getValue(),heigh.getValue());
				glVertex3f( -width.getValue()/2.0, -heigh.getValue()/2.0,screenZ);
			glTexCoord2f(width.getValue(),0.0);
				glVertex3f( -width.getValue()/2.0,  heigh.getValue()/2.0,screenZ);
		glEnd();

		glPopMatrix();
	//**************************************************************************
	// Off-axis method for rendering

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		r = aspect*t + K;	
		l = aspect*b + K;
		glFrustum(l,r,b,t,nearZ,farZ);				// delimits the frustum perspective

	//	Draw right Image in buffer

		glDrawBuffer(GL_BACK_RIGHT);                            //draw into back right buffer
	  	glMatrixMode(GL_MODELVIEW);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear color and depth buffers
  		glLoadIdentity();    

	/*
		rendering methods
	
	1) Toe-in: the left and right cameras look at the center of vergence at the screen
			plane, this method introduces a vertical parallax

	2)Off-axis: correct rendering method for planar screen surfaces, it does not introduces 		vertical parallax for a better stereo effect, the left and right 		cameras point to two different points but that are parallel one to other.

	*/
		//as for the left buffer but with camera position at:
		gluLookAt(IOD.getValue()/2, 0.0, 0.0, 
			  IOD.getValue()/2, 0.0, screenZ,            
            			       0.0, 1.0, 0.0);
	//**************************************************************************

		glPushMatrix();
		glTranslatef(0.0,0.0,depthZ);

	//	DRAW THE RIGHT IMAGE

		glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,720,576,GL_RGB,GL_UNSIGNED_BYTE,pthis->imageR.getValue(size,components));
		glBegin(GL_QUADS);
			glTexCoord2f(0.0,0.0);
				glVertex3f( width.getValue()/2.0, heigh.getValue()/2.0,screenZ);
			glTexCoord2f(0.0,heigh.getValue());	
				glVertex3f( width.getValue()/2.0,-heigh.getValue()/2.0 ,screenZ);
			glTexCoord2f(width.getValue(),heigh.getValue());
				glVertex3f( -width.getValue()/2.0, -heigh.getValue()/2.0,screenZ);
			glTexCoord2f(width.getValue(),0.0);
				glVertex3f( -width.getValue()/2.0,  heigh.getValue()/2.0,screenZ);
		glEnd();
		glPopMatrix();
	}
	//*******************************************************************************
	//	swap Buffers
	//	see the openGL programming Guide appendix C
	
	//glXSwapBuffers(glXGetCurrentDisplay(),glXGetCurrentDrawable());
	//glReadBuffer(GL_BACK_LEFT);
	//glDrawBuffer(GL_FRONT_LEFT);
	//glCopyPixels(0, 0, 720, 576, GL_COLOR);
	//*******************************************************************************
	else{	// if stereo is not supported, draw the two videos separately

	//	DRAW THE LEFT IMAGE

		glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,720,576,GL_RGB,GL_UNSIGNED_BYTE,pthis->imageL.getValue(size,components));
		//	modifying for stereo
		glBegin(GL_QUADS);
			glTexCoord2f(0.0,0.0);
				glVertex3f( -width.getValue(), heigh.getValue()/2.0,0.0);
			glTexCoord2f(0.0,heigh.getValue());	
				glVertex3f( -width.getValue(),-heigh.getValue()/2.0 ,0.0);
			glTexCoord2f(width.getValue(),heigh.getValue());
				glVertex3f( 0.0, -heigh.getValue()/2.0,0.0);
			glTexCoord2f(width.getValue(),0.0);
				glVertex3f( 0.0,  heigh.getValue()/2.0,0.0);
		glEnd();

	//	DRAW THE RIGHT IMAGE

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
		

	}
	glPopMatrix();
		
}

	
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


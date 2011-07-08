/*
        This a new inventor node used for showing textures for
        a stereo vision system

        author:         henry Portilla (c) 2010
        date:           january/7/2007

        Special grant 
        from:           God

        This software is under the GPL license

        The code is based on the book: the inventor toolmaker
*/
/**     
 *      This is a code for an Inventor node made thanks to God


*/
#ifndef _SOSTEREOTEXTURE_H_
#define _SOSTEREOTEXTURE_H_

#include <cstdlib>
#include <cstdio>
#include <iostream>
// include common types for this project
#include "common.h"
//#include <GL/glut.h>
#if _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	
	#include <windows.h>			// to include GL headers

	
#endif
#if linux
	#include <GL/glx.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <glext.h>  
												//      GL extensions
												//      Binding extensions with pointers to opengl in linux
                                                //      see opengl red book pag 715 in the 5th edition
//#include <Cg/cgGL.h>                          //      Cg API for OpenGL
//#include <Cg/cg.h>                            //      Cg Language runtime for compiling and                                                           run cg programs
#include <assert.h>                             //      to evaluate expressions

#include  <Inventor/SbLinear.h>                 //      vectors, planes and others
#include  <Inventor/fields/SoSFFloat.h>
#include  <Inventor/fields/SoSFUInt32.h>
#include  <Inventor/fields/SoSFImage.h>         //      for load images
#include  <Inventor/nodes/SoShape.h>
#include  <Inventor/nodes/SoPerspectiveCamera.h>
#include  <Inventor/Qt/SoQtGLWidget.h>          //      to do the swap of buffers
//      note SoSubNode.h has the macros definitions to
//      create the new nodes
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
//GLuint bufferID;                              //      PBO (pixel_buffer_object) name  

#include <opencv/cv.h>				// 	include OpenCV for haptic transforms
#include <opencv/cvaux.h>			//	used in 3Dpointer calculus
#include <opencv/cxcore.h>

class SoStereoTexture :public SoShape
{

//      macro for define methods and the static variables
SO_NODE_HEADER(SoStereoTexture);

public:

// Cg interface

typedef void Cg_Callback();     //      define a callback signature function
/*
Cg_Callback* onCgError;         //      make a function pointer to call
                                //      cg error function, this is done because the signature
                                //      of the callback function doesn't fit to 
                                //      the SoStereotexture methods signature

CGcontext cgContext;            //      Cg context

CGprofile cgProfile;            //      Hardware profile see cgc -help
                                //      to view the supported profiles
CGprogram fragmentProgram;      //      Cg fragment program
CGparameter textureParam;       //      Cg parameter 

typedef const char *Image_Filter;       //      edge cg code source program

Image_Filter Filter;            //      to save filters cg code
Image_Filter Filters[7];
*/                                                                 

//unsigned char *resultCg;
//GLvoid *resultCg;
// Fields

SoSFFloat       width;          //      width of images
SoSFFloat       heigh;          //      heigh of images
SoSFImage       imageL;
SoSFImage       imageR;
int             w;              //      save image size in integers
int             h;
int				X_haptic;	// 	Haptic position values
int				Y_haptic;
int				Z_haptic;
unsigned char   *image_L;       //      left image in unsigned char format
unsigned char   *image_R;       //      right image
SoSFFloat       IOD;            //      inter-ocular distance
SoSFFloat       timetoSynchronize;
				// 	3Dpointer variables
CvMat		*K_L;				//		intrinsic matrices
CvMat		*K_R;
CvMat		*Left_WorldTransform;//		extrinsic matrices
CvMat		*Rigth_WorldTransform;
CvMat		*P_L;				//		projection matrices
CvMat		*P_R;
CvMat 		*Transform_L;
CvMat 		*Transform_R;
CvMat 		*imageL_points;
CvMat 		*imageR_points;
CvMat 		*hapticPoint;
float		currentPositionDisparity;
int 		xiL;
int 		xiR;
int 		yiL;
int 		yiR;
int 		ziL;
int 		ziR;
int 		xi_nL;
int 		yi_nL;
int 		xi_nR;
int 		yi_nR;
float		uo_L;
float		uo_R;
float		vo_L;
float		vo_R;
float 		SfL;
float 		SfR;
float		hapticSpanX;		//		range of movements on X for the Haptic device
float		hfW;				//		haptic scaling on image width
								//		where hfW = hapticSpanX/ImageWidth
float		hapticSpanY;
float		hfH;				//		haptic scaling on image height
								//		where hfH = hapticSpanY/ImageHeight
float		hapticSpanZ;
float		hfZ;				//		haptic Scaling for image depth?

                                //      time that must be waited to
                                //      synchronize the render of two video streams

float		t_temp;				//		frustrum limits for haptic movements in stereo
float		b_temp;
float		r_tempL;
float		r_tempR;
float		l_tempL;
float		l_tempR;
float		hfWL;				//		scalings factor for a stereo system
float		hfWR;
float		Xh;					//		Biswas inspired method to get the corresponding points
float		Yh;
float		Zh;
float		X1;

// Initialize the class
static void initClass();        //      to define the type of information of the class
                                //      and initialize in the correct state
                                //      the different elements of openInventor

// constructor
SoStereoTexture();

// methods

void  synchronize();            //      synchronize the two images to show
                                //      using rtsp /rtp information
void  show();                   //      show the two images

protected:
//      define the actions and inherit others from parent class
                                
                                //      render the Quads for the images
virtual void GLRender(SoGLRenderAction *action);
                                //      generates triangles representing the quads
virtual void generatePrimitives(SoAction *action);
                                //      compute bounding box, required
virtual void computeBBox(SoAction *action,SbBox3f &box,SbVec3f &center);

private:

//      normals of quads
SoStereoTexture *pthis;
GLuint texName;                 //      texture name

//      OPENGL EXTENSIONS       

//      define procedures for PBO according to glext.h

PFNGLGENBUFFERSPROC glGenBuffersARB;    // define a glGenBufferARB according to opengl Extensions procedures
PFNGLBINDBUFFERPROC glBindBufferARB;    // define an association function for the PBO (pixel buffer object)
PFNGLBUFFERDATAPROC glBufferDataARB;    // define load of data procedure
PFNGLDELETEBUFFERSPROC glDeleteBuffersARB;// delete the object
PFNGLMAPBUFFERPROC glMapBufferARB;     // pointer to memory of the PBO
PFNGLUNMAPBUFFERPROC glUnmapBufferARB; // releases the mapping

//      defines procedures for FBO according to glext.h
        
                                        //      FBO is used for offscreen rendering
                                        //      specifically to do GPU image processing 
                                        //      FBO is used to save the partial results of the filtering 
                                        //      image processing

                                        //      define the associate buffer to do offscreen rendering
PFNGLBINDFRAMEBUFFEREXTPROC     glBindFramebufferEXT;
                                        //      create a FBO
PFNGLGENFRAMEBUFFERSEXTPROC     glGenFramebuffersEXT;
                                        //      Delete the FBO
PFNGLDELETEFRAMEBUFFERSEXTPROC  glDeleteFramebuffersEXT;
                                        //      attach texture to the FBO
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC        glFramebufferTexture2DEXT;
                                        //      check the functioning of FBO
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC      glCheckFramebufferStatusEXT;

//      defines procedures for Multitexturing  according to glext.h

PFNGLACTIVETEXTUREARBPROC       pglActiveTextureARB;
                                        //      active a texture
PFNGLMULTITEXCOORD2FARBPROC     glMultiTexCoord2fARB;
                                        //      draw complex textures
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
   
                                        //      check textures?
//      EXTENSIONS MANAGEMENT 
                                        //      check if extension given is supported
GLboolean isExtensionSupported(char *pExtensionName);

//      FBOand Image process methods
GLboolean check_FBO_Status();           //      check the correct functioning of the FBO
                                        //      create texture to manage FBO
void setupTexture(float width,float height, const GLuint textID);
                                        //      load data to the texture
void dataToTexture(const unsigned char *data,int width, int height,GLuint texID);
                                        //      Cg function to be called when there is an error
void cgErrorCallback();

void checkForCgError();                 //      for debugging Cg programs and Cg initialization

public:
	imagePoints getProjectedPoints();	//		get the calculated projected points for the
										//		3D pointer


// destructor
virtual ~SoStereoTexture();

};

#endif


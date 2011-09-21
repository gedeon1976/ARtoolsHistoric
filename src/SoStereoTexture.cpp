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
  *     This is a code for an Inventor node made thanks to God

*/

#include <Inventor/SoPrimitiveVertex.h>         // save information of each vertex point, 
                                                // normal, texture coordinates, material index
#include <Inventor/actions/SoGLRenderAction.h>
#include <Inventor/elements/SoGLTextureImageElement.h>
#include <Inventor/misc/SoState.h>              // to get the actual state of elements
#include "SoStereoTexture.h"                    // node definitions

// include common types
#include "common.h"
//#if _WIN32
//	// Visual Studio Debug
//	#include "reportingHook.h"
//#endif


//#include "openvidia32.h"

//      Extra functions

void SoStereoTexture::checkForCgError()
{
/*      CGerror error;
        const char *string = cgGetLastErrorString(&error);
        
        if (error != CG_NO_ERROR) {
                printf("%s: \n", string);
                if (error == CG_COMPILER_ERROR) 
                {
                        printf("%s\n", cgGetLastListing(cgContext));
                }
        //exit(1);
        }*/
}

//static CGcontext ErrContext;

void cgCallback()
{
/*      //      this method is called in the case of a Cg error
        const char *Listing;
        CGerror lastError = cgGetError();

        if(lastError)
        {       
                //Listing = cgGetLastListing(ErrContext);
                printf("\n--------------------------------------\n");
                printf("%s\n\n",cgGetErrorString(lastError));
                //printf("%s\n",Listing);
                printf("\n--------------------------------------\n");
                printf("Cg Error!\n");
                
        }*/
        
}

//      Macro to declare the methods and start the static variables
SO_NODE_SOURCE(SoStereoTexture);

//      Initialize the class
void SoStereoTexture::initClass()
{
        //      initialize type id variables
        SO_NODE_INIT_CLASS(SoStereoTexture,SoShape,"Shape");
        //      Enable state elements
        //SO_ENABLE(SoGLRenderAction,SoGLTextureImageElement);
}
//      Constructor
SoStereoTexture::SoStereoTexture()
{
        pthis = this;           // pointer to the same object?

        SO_NODE_CONSTRUCTOR(SoStereoTexture);   // define the fields and set the default values
        //      add the fields with default values
        SO_NODE_ADD_FIELD(width,(20));          
        SO_NODE_ADD_FIELD(heigh,(20));
	    SO_NODE_ADD_FIELD(imageL,(SbVec2s(0,0),3,0));
        SO_NODE_ADD_FIELD(imageR,(SbVec2s(0,0),3,0));
        //SO_NODE_ADD_FIELD(imageL,(NULL));
        //SO_NODE_ADD_FIELD(imageR,(NULL));
        SO_NODE_ADD_FIELD(IOD,(6.0));
        SO_NODE_ADD_FIELD(timetoSynchronize,(0.0));

        pthis->image_L = NULL;
        pthis->image_R = NULL;

        //      initialise pointers to PBO extension
        
        glBindBufferARB=NULL;
        glBufferDataARB=NULL;
        glDeleteBuffersARB=NULL;
        glGenBuffersARB=NULL;
        glMapBufferARB=NULL;
        glUnmapBufferARB=NULL;

        //      initialize pointers for FBO extension

        glGenFramebuffersEXT=NULL;
        glBindFramebufferEXT=NULL;
        glDeleteFramebuffersEXT=NULL;
        glFramebufferTexture2DEXT=NULL;
        glCheckFramebufferStatusEXT=NULL;

        //      initialize pointers for Multitexture extension

        pglActiveTextureARB = NULL;
        glMultiTexCoord2fARB = NULL;
        glClientActiveTextureARB = NULL;

        //      if is the first time the constructor is called
        //      setup opengl extensions
        if (SO_NODE_IS_FIRST_INSTANCE())
        {
                
        //      CG SETUP        

        //      Cg Filters

//      Laplacian
/*
Filters[0] = 

        "half4 edges(half2 coords : TEX0,                                    \n"
        "       uniform samplerRECT Image):COLOR                             \n"              
        "{                                                                   \n"
        "    static const half offset = 1024.0 / 512.0;                      \n"  
        "    half4 color;                                                    \n"            
        "    half4 c  = texRECT(Image, coords);                              \n" 
        "    half4 bl = texRECT(Image, coords + half2(-offset, -offset));    \n" 
        "    half4 l  = texRECT(Image, coords + half2(-offset,       0));    \n" 
        "    half4 tl = texRECT(Image, coords + half2(-offset,  offset));    \n" 
        "    half4 t  = texRECT(Image, coords + half2(      0,  offset));    \n"  
        "    half4 ur = texRECT(Image, coords + half2( offset,  offset));    \n" 
        "    half4 r  = texRECT(Image, coords + half2( offset,       0));    \n" 
        "    half4 br = texRECT(Image, coords + half2( offset, -offset));    \n" 
        "    half4 b  = texRECT(Image, coords + half2(      0, -offset));    \n" 
        "   // scale by 8 to brighten the edges                              \n"
        "    return color = 8* (c + -0.125 * (bl + l + tl + t + ur + r + br + b));\n"
        "}                                                                      \n";
         

//      color segmentation      

Filters[1] =
        "float4 edges(float2 fptexCoord : TEXCOORD0,    \n"
        "float4 wpos : WPOS,                            \n"
        "uniform samplerRECT Image):COLOR               \n"
        "{                                              \n"
        "       float4   colorO = texRECT(Image, fptexCoord);   \n"
        "       if ( colorO.r > 0.95 & colorO.g > 0.95 & colorO.b > 0.95)       \n"
        "       {                                                               \n"
        "                colorO.rgb = 0.5;                                      \n"
        "       }                                                               \n"     
        "       else{                                                           \n"     
        "                colorO.rgb = 1.0;                                      \n"
        "       }                                                               \n"
        "       return colorO;                                                  \n"
        "}                                                                      \n";

// Image derivative [df/dx]²

Filters[2] =

        "half4 edges(half2 coords : TEX0,               \n"
        "uniform samplerRECT Image):COLOR               \n"
        "{                                              \n"
        "// offset to sample pixels                     \n"
        "static const half offset =1024.0/512.0;        \n"     
        "half4 dx2;                                     \n"
        "//     Does a 3x3 mask                         \n"
        "half4 c = texRECT(Image,coords);// center pixel\n"

        "half4 tl= texRECT(Image,coords + half2(-offset, offset));      \n"
        "half4 t = texRECT(Image,coords + half2(      0, offset));      \n"
        "half4 tr= texRECT(Image,coords + half2( offset, offset));      \n"
        
        "half4 bl= texRECT(Image,coords + half2(-offset,-offset));      \n"
        "half4 b = texRECT(Image,coords + half2(     0, -offset));      \n"
        "half4 br= texRECT(Image,coords + half2( offset,-offset));      \n"
        "//     calculate df/dx                                         \n"
        "half4 dx = c + ((bl + b + br) - (tl + t + tr));                \n"
        "//     calculate the term [df/dx]²                            \n"
        "dx2 = dx*dx;                                                   \n"
        "return dx2;                                                    \n"
        "}                                                              \n";

//Image derivative [df/dy]²

Filters[3]=

        "half4 edges(half2 coords : TEX0,       \n"
        "uniform samplerRECT Image): COLOR      \n"
        "{                                      \n"
        "// offset to sample pixels             \n"
        "static const half offset =1024.0/512.0;                        \n"
        "//     Does a 3x3 mask                                         \n"
        "half4 c = texRECT(Image,coords);// center pixel                \n"
        "                                                               \n"
        "half4 tl= texRECT(Image,coords + half2(-offset, offset));      \n"
        "half4 l = texRECT(Image,coords + half2(-offset,      0));      \n"
        "half4 bl= texRECT(Image,coords + half2(-offset,-offset));      \n"

        "half4 tr= texRECT(Image,coords + half2( offset, offset));      \n"
        "half4 r = texRECT(Image,coords + half2( offset,      0));      \n"
        "half4 br= texRECT(Image,coords + half2( offset,-offset));      \n"
        "//     calculate df/dy                                         \n"
        "half4 dy = c + ((tr + r + br) - (tl + l + bl));                \n"
        "//calculate the term [df/dy]²                                 \n"
        "half4 dy2 = dy*dy;                                             \n"
        "return dy2;                                                    \n"
        "}                                                              \n";

//      convolution with a 3x3 gaussian  window
//                  0.04   0.12   0.04
//      gaussian =  0.12   0.36   0.12
//                  0.04   0.12   0.04

Filters[4] =

        "half4 edges(half2 coords : TEX0,       \n"
        "uniform samplerRECT Image):COLOR       \n"
        "{                                      \n"
        "// offset to sample pixels             \n"
        "static const half offset = 1024.0 / 512.0;      \n"
        "//     Does a 3x3 gaussian mask                 \n"          
        "half4 c  = texRECT(Image, coords);// center pixel              \n"
        "half4 bl = texRECT(Image, coords + half2(-offset, -offset));   \n"
        "half4 l  = texRECT(Image, coords + half2(-offset,       0));   \n"
        "half4 tl = texRECT(Image, coords + half2(-offset,  offset));   \n"
        "half4 t  = texRECT(Image, coords + half2(      0,  offset));   \n"
        "half4 tr = texRECT(Image, coords + half2( offset,  offset));   \n"
        "half4 r  = texRECT(Image, coords + half2( offset,       0));   \n"
        "half4 br = texRECT(Image, coords + half2( offset, -offset));   \n"
        "half4 b  = texRECT(Image, coords + half2(      0, -offset));   \n"
        "// smooth the image                                            \n"
        "half4 color =(0.36*c + 0.04*bl + 0.12*l + 0.04*tl + 0.12*t + 0.04*tr + 0.12*r + 0.04*br + 0.12*b);                                             \n"
        "return color;                                                  \n"
        "}                                                              \n";

//      Image derivative crossed product of [df/dx]*[df/dy]     

Filters[5] =

        "half4 edges(half2 coords : TEX0,               \n"
        "uniform samplerRECT Image):COLOR               \n"
        "{                                              \n"
        "// offset to sample pixels                     \n"
        "static const half offset =1024.0/512.0;        \n"             
        "//     Does a 3x3 mask                         \n"
        "half4 c = texRECT(Image,coords);// center pixel\n"

        "half4 tl= texRECT(Image,coords + half2(-offset, offset));\n"
        "half4 l = texRECT(Image,coords + half2(-offset, 0     ));\n"
        "half4 t = texRECT(Image,coords + half2( 0,      offset));\n"
        "half4 bl= texRECT(Image,coords + half2(-offset,-offset));\n"

        "half4 tr= texRECT(Image,coords + half2( offset, offset));\n"
        "half4 r = texRECT(Image,coords + half2( offset, 0     ));\n"
        "half4 b = texRECT(Image,coords + half2( 0,     -offset));\n"
        "half4 br= texRECT(Image,coords + half2( offset,-offset));\n"
        "//     calculate df/dx                                   \n"
        "half4 dx = c + ((bl + b + br) - (tl + t + tr));          \n"
        "//     calculate df/dy                                   \n"
        "half4 dy = c + ((tr + r + br) - (tl + l + bl));          \n"
        "//     calculate [df/dx]*[df/dy]                         \n"
        "half4 dxdy = dx*dy;                                      \n"
        "return dxdy;                                             \n"
        "}                                                        \n";
                                                
*/

        //      Initialize the cg variables
        //g_cgProfile = CG_PROFILE_FP40;

//      onCgError = cgCallback;                 //      pointer to  a correct signature function
//      cgSetErrorCallback(onCgError);          //      enable Cg error information
        
        //g_cgProfile = CG_PROFILE_FP30;
/*
        cgContext = cgCreateContext();          //      create a container for the Cg programs
        checkForCgError();
*/              
                
                                                     


        //      USING OPENGL EXTENSIONS       
                
        //      define procedures for PBO according to glext.h
        //      use glx.h to do the correct binding through glXGetProcAddress(GLubyte*) in linux
        //      and to enable the use of opengl extensions
	//	and for windows use windows.h to do the correct binding through wglGetProcAddress(GLubyte*)
        
	#ifdef _WIN32
        //      PBO extension opengl function binding

        // define a glGenBufferARB according to opengl Extensions procedures
                glGenBuffersARB = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffersARB");      
        // define an association function for the PBO (pixel buffer object)
                glBindBufferARB = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBufferARB");      
        // define load of data procedure
                glBufferDataARB = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferDataARB");      
        // delete the object
                glDeleteBuffersARB = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffersARB");
        // pointer to memory of the PBO
                glMapBufferARB = (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBufferARB");         
        // releases the mapping 
                glUnmapBufferARB = (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBufferARB");

        //      FBO extension opengl function bindings

        // define a glGenFramebuffersEXT according to opengl extension procedure API
                glGenFramebuffersEXT =(PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
        // associate a texture for offscreen rendering using the FBO
                glBindFramebufferEXT =(PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
        // Delete the FBO
                glDeleteFramebuffersEXT =(PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
        // Associate a texture to the FBO
                glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
        // Check for errors in the FBO
                glCheckFramebufferStatusEXT =
                (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");     

        //      Multitexture Extension opengl function bindings

        // Activate textures
                pglActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
        // Draw complex textures
           //      glMultiTexCoord2fARB =(PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glActiveTextureARB");
        // Check active texture 
           //     glClientActiveTextureARB =
           //     (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");

	#endif// UNIX
	#if linux
		//      PBO extension opengl function binding

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

        //      FBO extension opengl function bindings

        // define a glGenFramebuffersEXT according to opengl extension procedure API
                glGenFramebuffersEXT =(PFNGLGENFRAMEBUFFERSEXTPROC)glXGetProcAddress((const unsigned char*)"glGenFramebuffersEXT");
        // associate a texture for offscreen rendering using the FBO
                glBindFramebufferEXT =(PFNGLBINDFRAMEBUFFEREXTPROC)glXGetProcAddress((const unsigned char*)"glBindFramebufferEXT");
        // Delete the FBO
                glDeleteFramebuffersEXT =(PFNGLDELETEFRAMEBUFFERSEXTPROC)glXGetProcAddress((const unsigned char*)"glDeleteFramebuffersEXT");
        // Associate a texture to the FBO
                glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)glXGetProcAddress((const unsigned char*)"glFramebufferTexture2DEXT");
        // Check for errors in the FBO
                glCheckFramebufferStatusEXT =
                (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)glXGetProcAddress((const unsigned char*)"glCheckFramebufferStatusEXT");     

        //      Multitexture Extension opengl function bindings

        // Activate textures
                pglActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)glXGetProcAddress((const unsigned char*)"glActiveTextureARB");
        // Draw complex textures
                glMultiTexCoord2fARB =(PFNGLMULTITEXCOORD2FARBPROC)glXGetProcAddress((const unsigned char*)"glActiveTextureARB");
        // Check active texture 
                glClientActiveTextureARB =
                (PFNGLCLIENTACTIVETEXTUREARBPROC)glXGetProcAddress((const unsigned char*)"glActiveTextureARB");
	
	#endif // end WIN32

	// OpenCV code
	
	// initializes haptic transforms (Zissermann and Biswas Approach)
	K_L = cvCreateMat(4,4,CV_32FC1);
	K_R = cvCreateMat(4,4,CV_32FC1);
	Left_WorldTransform = cvCreateMat(4,4,CV_32FC1);
	Rigth_WorldTransform = cvCreateMat(4,4,CV_32FC1);
	P_L = cvCreateMat(4,4,CV_32FC1);
	P_R = cvCreateMat(4,4,CV_32FC1);
	//double valueMatrix[] = {1,1,1,1};
	CvScalar value;
	value.val[0] = 1.0;
	
	// it is presumed that the cameras are not rotated (for now, parallel configuration)
	cvSetIdentity(K_L,value);
	cvSetIdentity(K_R,value);
	cvSetIdentity(Left_WorldTransform,value);
	cvSetIdentity(Rigth_WorldTransform,value);
	cvSetIdentity(P_L,value);
	cvSetIdentity(P_R,value);

	// initializes haptic transforms
	Transform_L = cvCreateMat(4,4,CV_32FC1);
	Transform_R = cvCreateMat(4,4,CV_32FC1);
	cvSetZero(Transform_L);
	cvSetZero(Transform_R);
	// initializes image variables
	imageL_points = cvCreateMat(4,4,CV_32FC1);
	imageR_points = cvCreateMat(4,4,CV_32FC1);
    cvSetZero(imageL_points);
	cvSetZero(imageR_points);
	hapticPoint = cvCreateMat(4,4,CV_32FC1);
	cvSetZero(hapticPoint);
	xiL = 1; xiR = 1; uo_L = 1; vo_L = 1;
	yiL = 1; yiR = 1; uo_R = 1; vo_R = 1;
	ziL = 1; ziR = 1;
	// initializes values for haptic readings
	X_haptic = 1;
	Y_haptic = 1;
	Z_haptic = 1;
	xi_nL = 1; yi_nL = 1;
	xi_nR = 1; yi_nR = 1;
	currentPositionDisparity = 0;
	// set Haptic Workspace
	hapticSpanX = 200;//764;
	hapticSpanY = 400;//593;
	hapticSpanZ = 300;//403;
	hfW = 1;
	hfH = 1;
	hfZ = 1;
	// initial value for Haptic point
	Xh = 1;
	Yh = 1;
	Zh = 100;


        }
}
//      Destructor
SoStereoTexture::~SoStereoTexture()
{
//      cgDestroyProgram(fragmentProgram);
//      cgDestroyContext(cgContext);
}
//      Methods

//      PRIVATE METHODS

GLboolean SoStereoTexture::isExtensionSupported(char *pExtensionName)
{
        const unsigned char *pVersion, *pstrExtensions;
        const unsigned char *pStart;
        unsigned char *pszWhere,*pszTerminator;
        
        //glPushMatrix();
        
        //      look for the opengl version and the extensions supported
        pVersion = glGetString(GL_VERSION);
//      printf("The openGL version is %s\n",pVersion);
        //      strExtensions =glGetString(GL_EXTENSIONS);
        //printf("The openGL extensions supported are %s\n",strExtensions);

        //      look for the PBO openGL extension see pag 598 red book 5 edition
        //ExtensionName = (const GLubyte*)"GL_EXT_pixel_buffer_object";
        //EXT_SUPPORTED = gluCheckExtension(ExtensionName,strExtensions);
        
        //pExtensionName = (char*)"GL_EXT_pixel_buffer_object";

        //      Extensions names should not have spaces
        pszWhere =(unsigned char*)strchr(pExtensionName,' ');
        if(pszWhere||pExtensionName == '\0')
        {
                return GL_FALSE;        //      no extension name asked.
        }
        //      Get the extension string
        pstrExtensions = glGetString(GL_EXTENSIONS);
        
        //      Search the extensions string for an exact copy of extension queried

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

GLboolean SoStereoTexture::check_FBO_Status()
{
///     Code based from D. göddeke GPGPU tutorial
///     This function checks if the Framebuffer object is working well
        
GLenum status;                          //  status variable
status =(GLenum)glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
switch(status)
{
        case GL_FRAMEBUFFER_COMPLETE_EXT:
                return true;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                printf("Framebuffer imcomplete, imcomplete attachment\n");
                return false;
        case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
                printf("Unsupported Framebuffer format\n");
                return false;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                printf("Framebuffer imcomplete, missing attachment\n");
                return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                printf("Framebuffer imcomplete, attached images must have the same dimensions\n");
                return false;
//        case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
//               printf("Framebuffer imcomplete duplicate attachment\n");
//                return false;
        case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                printf("Framebuffer imcomplete, attached images must have the same format\n");
                return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
                printf("Framebuffer imcomplete, missing draw buffer\n");
                return false;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
                printf("Framebuffer imcomplete, missing read buffer\n");
                return false;
}
        return false;           //      return false any other value
}
void SoStereoTexture::setupTexture(float width,float height, const GLuint texID)
{
        //      This method creates a rectangular texture
        //      that can be used for drawing an image or as 
        //      store array for intermediate steps in image
        //      processing using GPU computer power
                                                 
        glBindTexture(GL_TEXTURE_RECTANGLE_NV,texID);   
        //      how to place the texture texture parameters
        glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
        //      extend the texture in S coord
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_WRAP_S,GL_CLAMP);
        //      and T too
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_WRAP_T,GL_CLAMP);
        //      REVIEW IF THIS  FILTERS ARE NEEDED WITH GL_TEXTURE_RECTANGLE_NV
        //      on view magnification
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        //      on view mimimization
        glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_NEAREST); 
        
        //      create the texture with floating point format//GL_RGBA32F_ARB
        glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA32F_ARB , (int)width, (int)height, 0,                   
                                    GL_RGB, GL_UNSIGNED_BYTE, NULL);

}
void SoStereoTexture::dataToTexture(const unsigned char *data,int width,int height, GLuint texID)
{
        //      This method load data to the texture
        //      mainly was though for image loading
        
        glBindTexture(GL_TEXTURE_RECTANGLE_NV,texID);
        if(data!= 0)
        {
                glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,data);
        }

}


//      PROTECTED METHODS

//      SoGLRenderAction 

void SoStereoTexture::GLRender(SoGLRenderAction *action)
{
        // access the state from the  action
        //SoState *state = action->getState();
        
        static GLuint fbo;                      //      FBO
        GLuint iTexture[4];                     //      textures for GPU filtering FX3500 only support 4 textures
		
        GLuint oTexture;

        GLvoid *resultCg;
        //GLuint iTexture;                      //      textures for GPU filtering

        GLuint bufferID[2];                     //      PBO (pixel_buffer_object) name  
        void *pboMemoryL,*pboMemoryR;
        GLboolean isPBO,isFBO;                  //      boolean flags
        GLboolean isStereo;
       
#if _WIN32
	HDC Surface;				//	Windows variables to do the SwapBuffer
	HGLRC Display;						
#endif
#if linux
	GLXDrawable glXSurface;                 //      glX variables to do the SwapBuffer
        Display *pDisplay;
#endif
	
	// Haptic transforms ////////////////////////////////////////////////////////////////////////////////////////
	
	float thetaAngle = 0;
	float f = 780;//800;				// 	focal length to be found
	float Baseline = 72;//110	// 	measured in haptic coordinates (in real world correspond
								//	to a separation of 11 cm between the cameras)
	float cTheta,sTheta, delta_X;
	sTheta = sin(thetaAngle);
	cTheta = cos(thetaAngle);
	delta_X = 0.5*Baseline;
	float ZcL =  Z_haptic*cTheta - (X_haptic - delta_X)*sTheta;
	float ZcR =  Z_haptic*cTheta - (X_haptic + delta_X)*sTheta;
	if (Z_haptic != 0)
	{
	    SfL = f/ZcL;			// scale factors
	    SfR = f/ZcR;
	}else{
	    SfL = 1;
	    SfR = 1; 
	}
	float Transform_L_values[] = {cTheta, 	0, 	sTheta,	-delta_X,
				      0,	1,	0,	0,
				      -sTheta,	0,	cTheta,	0,
				      0,	0,	0,	1
				      };
	Transform_L = cvInitMatHeader(Transform_L,4,4,CV_32FC1,Transform_L_values);
	
	float Transform_R_values[] = {cTheta, 	0, 	sTheta,	delta_X,
				      0,	1,	0,	0,
				      -sTheta,	0,	cTheta,	0,
				      0,	0,	0,	1
				      };
	Transform_R = cvInitMatHeader(Transform_R,4,4,CV_32FC1,Transform_R_values);
	
	float HIP [] = {X_haptic,0,0,0,
			Y_haptic,0,0,0,
			Z_haptic,0,0,0,
			1,0,0,0};
	hapticPoint = cvInitMatHeader(hapticPoint,4,4,CV_32FC1,HIP);
	// find left transform
	cvMatMul(Transform_L,hapticPoint,imageL_points);
	// find right transform
	cvMatMul(Transform_R,hapticPoint,imageR_points);
	
	// get projected values on images
	
	xiL = SfL*cvmGet(imageL_points,0,0);
	yiL = SfL*cvmGet(imageL_points,1,0);
	
	xiR = SfR*cvmGet(imageR_points,0,0);
	yiR = SfR*cvmGet(imageR_points,1,0);
	
	ziL = SfL*cvmGet(imageL_points,2,0);
	ziR = SfR*cvmGet(imageR_points,2,0);
	
	
	
	///////////////////////////////////////////////////////////////////////////////////////////////////


        //      initializes this variables to use after in the openGL internal code
        //      this give a desired behavior at the time of configure the
        //      openGL textures
        w = (int)width.getValue();
        h = (int)heigh.getValue();
	
	// final coordinates mapped from haptic to be rendered at the textures
	xi_nL = xiR - 0.5*w;//-0.5*Baseline +  
	yi_nL = yiL;
	xi_nR = xiL + 0.5*w;// 0.5*Baseline + 
	yi_nR = yiR;

	// Calibration values for both cameras
	float Intrinsic_K_L[] ={777.7588,	 0,		288.3929,	0,
							0,		786.8474,	262.8120,	0,
							0,			 0,			1,		0,
							0,			 0,			0,		0};
	K_L = cvInitMatHeader(K_L,4,4,CV_32FC1, Intrinsic_K_L);
	uo_L = cvmGet(K_L,0,2);
	vo_L = cvmGet(K_L,1,2);
	float Intrinsic_K_R[] ={771.1128,	 0,		298.7664,	0,
							0,		780.9084,	252.5712,	0,
							0,			 0,			1,		0,
							0,			 0,			0,		0};
	K_R = cvInitMatHeader(K_R,4,4,CV_32FC1, Intrinsic_K_R);
	uo_R = cvmGet(K_R,0,2);
	vo_R = cvmGet(K_R,1,2);

	// Scaling factors haptic to images
	// It maintains the Haptic movements within the image limits
	if (hapticSpanX > w){
		hfW =  w/hapticSpanX;
		
	}else{
		hfW = w/hapticSpanX;
	}
	if (hapticSpanY > h){
	
		hfH = h/hapticSpanY;	
		
	}else{
		hfH = h/hapticSpanY;
		
	}
	int DisparityDepth = 100;
	hfZ = hapticSpanZ/DisparityDepth;

	// calculus of positions using prof. Biswas (Indian Institute) inspired method
	
	Xh = X_haptic*hfW;
	Yh = Y_haptic*hfH;
	Zh = Z_haptic*hfZ;

	// left point
	xi_nL = Xh - uo_L;//=f*Xhaptic/Zh;
	yi_nL = Yh - vo_L;//=yiL;
	// right point	
	X1 = (Zh - f)*(-xi_nL)/f;
	xi_nR = f*(Baseline - X1)/(Zh - f);
	yi_nR = yi_nL + 10;
	//currentPositionDisparity = fabs(fabs(xi_nL) - fabs(xi_nR));

	// Calculus using Zissermann and Biswas approach

	float HIP2 [] = {Xh,0,0,0,
			Yh,0,0,0,
			Zh,0,0,0,
			1,0,0,0};
	hapticPoint = cvInitMatHeader(hapticPoint,4,4,CV_32FC1,HIP2);
	
	
	// find left Projection matrix
	cvMatMul(K_L,Left_WorldTransform,P_L);
	// find right Projection Matrix
	cvmSet(Rigth_WorldTransform,0,3,Baseline);
	cvMatMul(K_R,Rigth_WorldTransform,P_R);

	// find left points
	cvMatMul(P_L,hapticPoint,imageL_points);
	// find right points
	cvMatMul(P_R,hapticPoint,imageR_points);

	// get projected values on images
	
	//xiL = cvmGet(imageL_points,0,0);
	//yiL = cvmGet(imageL_points,1,0);
	//
	//xiR = cvmGet(imageR_points,0,0);
	//yiR = cvmGet(imageR_points,1,0);

	//xi_nL = xiL;
	//yi_nL = yiL;

	//xi_nR = xiR;
	//yi_nR = yiR;


        ////////////////////////
        //      Cg Setup
/*
        //      get profile for fragment processor
        cgProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
        checkForCgError();
        //      optimize the choosen profile for this hardware
        cgGLSetOptimalOptions(cgProfile);
        checkForCgError();
*/
        //      Quad Buffer TEST
        //**********************************************************************************
        
        float depthZ = 150.0;                           // depth of the object model

        //double fovy =  45;                            // field of view in y-axis, 
        double theta = 90;                             // aperture angle as FOV
        double aspect = double(width.getValue())/double(heigh.getValue()); 
                                                        // screen aspect ratio
        double nearZ = 75.0;                            // near clipping plane
        double farZ = 400.0;                            // far clipping plane
        double screenZ =100.0;                          // screen projection plane
        //double IOD = 5.0;                             // intraocular distance
        double beta;                                    // vergence angle
        double l,r,b,t,K;                               // limits for glfrustum delimitation    
        
        //      frustum calculation for glFrustum
        beta = 2*atan(IOD.getValue()/(2*screenZ));      // get vergence angle from IOD
        t =  nearZ*tan(theta/2);                        // top coordinate
        b = -nearZ*tan(theta/2);                        // bottom coordinate
        K = (0.5*IOD.getValue()*nearZ)/screenZ;         // to get l and r values
        //***********************************************************************************
        // ask if this should be rendered
        if(!shouldGLRender(action))
        {
                return;
        }
//      beginSolidShape(action);        //      start draw object with optimizations
                                        //      like backculling

        //      look for Extension supported

        //isPBO = isExtensionSupported("GL_EXT_pixel_buffer_object");   //      is PBO supported?
        isPBO = GL_FALSE;
        
        //      check if stereo is supported

        glGetBooleanv(GL_STEREO,&isStereo);
        //isStereo = GL_FALSE;  

        //      openGL code for textures

        //      using PBO according to the extension specification

	if (isPBO == GL_TRUE)   // if PBO is supported, use it when will be hardware supported, if this go slow
				// probably is software supported, remember PBO since opengl 2.1 in april/07

	{
	    //      start with a null image
	    // glBindBufferARB(GL_PIXEL_UNPACK_BUFFER, 0);
	    // glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, 720, 576, 0,
	    //                           GL_RGB, GL_UNSIGNED_BYTE, NULL);   

	    glPushMatrix();                 //      save openGl state

	    // Create and bind texture image buffer object

	    //      PBO LEFT
	    glGenBuffersARB(1, &bufferID[0]);
	    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, bufferID[0]);
	    //      reset contents of PBO
	    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB,w*h*3,NULL,GL_DYNAMIC_DRAW_ARB);

	    //      PBO RIGHT
	    glGenBuffersARB(1, &bufferID[1]);
	    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, bufferID[1]);
	    //      reset contents of PBO
	    glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB,w*h*3,NULL,GL_DYNAMIC_DRAW_ARB);

	    //glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, 720, 576, 0,
	    //                           GL_RGB, GL_UNSIGNED_BYTE, NULL);

	    // Setup texture environment
	    
	    glClearColor(0.0,0.0,0.0,0.0);                  //      clear color to black when the buffers will be cleaned
	    glShadeModel(GL_FLAT);                          //      solid colors not shading
	    glEnable(GL_DEPTH_TEST);                        //      enable depth test?
	    //      This function configure how the pixels are unpacked from memory
	    //glPixelStorei(GL_UNPACK_ALIGNMENT,1);         //      REVIEW FOR PERFORMANCE, but this use boundaries of 1 Byte
							    //      in test this is the better      
	    //glGenTextures(1,&texName);                    //      assigns a name for the texture from the texname array
							    //      create a texture object and assigns a name  
	    //glBindTexture(GL_TEXTURE_RECTANGLE_NV,texName);               
	    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);// how to place the texture
	    //texture parameters
	    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_WRAP_S,GL_CLAMP);//  extend the texture in S coord
	    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_WRAP_T,GL_CLAMP);//  and T too
	    //      REVIEW IF THIS  FILTERS ARE NEEDED WITH GL_TEXTURE_RECTANGLE_NV
	    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_NEAREST);      //      on view magnification
	    glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_NEAREST);      //      on view mimimization
	    glEnable(GL_TEXTURE_RECTANGLE_NV);
	    glTexImage2D(GL_TEXTURE_RECTANGLE_NV,0,GL_RGBA,w,h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	    
	    //      OPENGL 2 extension
	    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
	    //                      PBO SETUP
	    /*
	    glGenBuffersARB(1,&bufferID);   //      create the buffer for the PBO
	    glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,bufferID); 
							    // allocate a chunk of memory using a PBO pixel_buffer_object 
							    // view Fast Texture Transfer and using VBOs articles
							    // from nvidia developer web site
							    // see also the reference to the lesson 45 in NeHe web site
							    // openGL tutorials dec 2006
	    */

	    //      modify the data in the GPU memory

	    SbVec2s size; int components;
	    size.setValue(w,h);
	    components=3;
	    
	    //      check for stereo support

	    if(isStereo == GL_TRUE) 
	    {
	    
	    //*******************************************************************************
	    //      QUAD BUFFER INITIALIZATION
		    glViewport (0, 0, 1000, 800);                           // sets drawing viewport
		    //glMatrixMode(GL_PROJECTION);
		    //glLoadIdentity();                                     // reset Projection matrix
		    //glFrustum(576.0,576.0,720.0,720.0,nearZ,farZ);        // set frustum to see
		    //gluPerspective(fovy, aspect, nearZ, farZ);            // sets frustum using gluPerspective
	    
		    // Off-axis method for rendering

		    glMatrixMode(GL_PROJECTION);
		    glLoadIdentity();
		    r = aspect*t - K;       
		    l = aspect*b - K;
		    glFrustum(l,r,b,t,nearZ,farZ);                          // delimits the frustum perspective

		    glMatrixMode(GL_MODELVIEW);
		    glDrawBuffer(GL_BACK_LEFT);                             // draw into back left buffer
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear color and depth buffers
					
		    glLoadIdentity();                                       // reset modelview matrix
	    
	    
		    /*      rendering method in flat screens by Paul Borke
	    
		    a) toe-in method (introduces vertical parallax)
		    b) off axis method (correct)
		    */
	    
		    gluLookAt(-IOD.getValue()/2,0.0,0.0,    //set camera position  x=-IOD/2
			      -IOD.getValue()/2,0.0,screenZ,//set camera "look at" x=0.0
					    0.0,1.0,0.0);   //set camera up vector
	    //*******************************************************************************

		    glPushMatrix(); 
		    glTranslatef(0.0,0.0,depthZ);

		    //      reset contents of PBO
		    //glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_EXT,720*576*3,NULL,GL_STREAM_DRAW);
	    
		    //      define a pointer that maps to memory at the GPU memory card                                             
		    pboMemoryL = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,GL_WRITE_ONLY);
		    //      write data in the GPU RAM memory
		    memcpy(pboMemoryL,pthis->imageL.getValue(size,components),w*h*3);       
		    //      Unmap the PBO buffer    
		    glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);

		    //      DRAW THE LEFT IMAGE
		    //      PBO usage 
		    glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,BUFFER_OFFSET(0));
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
		    glFrustum(l,r,b,t,nearZ,farZ);                          // delimits the frustum perspective

	    //      Draw right Image in buffer

		    glDrawBuffer(GL_BACK_RIGHT);                            //draw into back right buffer
		    glMatrixMode(GL_MODELVIEW);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear color and depth buffers
		    glLoadIdentity();                                       //reset modelview matrix

	   //as for the left buffer but with camera position at:
		    gluLookAt(IOD.getValue()/2, 0.0, 0.0, 
			      IOD.getValue()/2, 0.0, screenZ,            
					  0.0, 1.0, 0.0);

				    
	    //**************************************************************************

		    glPushMatrix(); 
		    glTranslatef(0.0,0.0,depthZ);

		    //      second PBO
		    pboMemoryR = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,GL_WRITE_ONLY);
		    //      write data in the GPU RAM memory
		    memcpy(pboMemoryR,pthis->imageR.getValue(size,components),w*h*3);       

		    //      Unmap the PBO buffer    

		    glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);

		    //      PBO usage 
		    //      DRAW THE RIGHT IMAGE
		    glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,BUFFER_OFFSET(0));
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
		    //      swap Buffers
		    //      see the openGL programming Guide appendix C

		    #if _WIN32
			Surface = wglGetCurrentDC();

			if(Surface)
			{
				Display = wglGetCurrentContext();      //      get the current display
				//if (pDisplay  NULL)
				//{
				SwapBuffers(Surface);
				//}
			}


		    #endif
		    #if linux
		    
		    glXSurface = glXGetCurrentDrawable();

			if(glXSurface =! NULL)
			{
				pDisplay = glXGetCurrentDisplay();      //      get the current display
				//if (pDisplay  NULL)
				//{
					glXSwapBuffers(pDisplay,glXSurface);
				//}
			}
		    #endif	    
	    //      glXSwapBuffers(glXGetCurrentDisplay(),glXGetCurrentDrawable());
		    //glXWaitGL();                  //      wait openGl execution

		    //glReadBuffer(GL_BACK);
		    //glDrawBuffer(GL_FRONT);
		    //glCopyPixels(0, 0, 720, 576, GL_COLOR);
		    //*******************************************************************************
		}
	     else{   // If not stereo support

		    //      define a pointer that maps to memory at the GPU memory card                                             
		    pboMemoryL = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,GL_WRITE_ONLY);
		    //      write data in the GPU RAM memory
		    memcpy(pboMemoryL,pthis->imageL.getValue(size,components),w*h*3);       
		    //      Unmap the PBO buffer    
		    glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);

		    //      DRAW THE LEFT IMAGE
		    //      PBO usage 
		    glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,BUFFER_OFFSET(0));
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

		    //      second PBO
		    pboMemoryR = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB,GL_WRITE_ONLY);
		    //      write data in the GPU RAM memory
		    memcpy(pboMemoryR,pthis->imageR.getValue(size,components),w*h*3);       
		    //      Unmap the PBO buffer    
		    glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);

		    //      PBO usage 
		    //      DRAW THE RIGHT IMAGE
		    glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,BUFFER_OFFSET(0));
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
        //      End of stereo verification
        //********************************************************************************
        glPopMatrix();                  //      restore opengl states

        //      Delete the buffers

        glDeleteBuffersARB(1,&bufferID[0]);
        glDeleteBuffersARB(1,&bufferID[1]);

	//      glBindBufferARB(GL_PIXEL_UNPACK_BUFFER, 0);
      
	// END OF PBO
      }else
	{
	    //	    USING GL_RECTANGLENV
	    //      using gl_RectangleNV parameter for textures, very fast 


	    glPushMatrix();

	    //      store viewport size
	    //      This is done for Cg language image filters

	    //      using FBO
							    //      is PBO supported?
	    isFBO = isExtensionSupported("GL_EXT_framebuffer_object");      
	    if (isFBO = GL_TRUE)
	    {
                //glGenFramebuffersEXT(1,&fbo);         //      create a FBO with 2 textures attachments
                //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fbo); 
                                                        //      bind offscreen render buffer
	    }

	    // Setup texture environment
	    
	    glClearColor(0.0,0.0,0.0,0.0);                  //      clear color to black when the buffers will be cleaned
	    glShadeModel(GL_FLAT);                                //      solid colors not shading
	    glEnable(GL_DEPTH_TEST);                      //      enable depth test?
	    //      This function configure how the pixels are unpacked from memory
	    //glPixelStorei(GL_UNPACK_ALIGNMENT,1);         //      REVIEW FOR PERFORMANCE, but this use boundaries of 1 Byte
							    //      in test this is the better      
	    
	    //      create textures
	    
	    glGenTextures(4,iTexture);
	    //      assigns names for the textures in the textures array

	    //      setup textures
	    
	    SbVec2s size; int components;   //      define size of images
	    size.setValue(w,h);
	    components=3;

	    for(int i=0; i<4;i++)
	    {
		    setupTexture(w,h,iTexture[i]);
	    }
                
        
	    //      check for stereo support

	    if(isStereo == GL_TRUE) 
	    {
			// Scaling the scene to the Haptic limited workspace
			float Y_delta,X_deltaL,X_deltaR;
			t_temp =  screenZ*tan(theta/2);                        // top coordinate
		    b_temp = -screenZ*tan(theta/2);                        // bottom coordinate
			r_tempL = aspect*t_temp - K;       
		    l_tempL = aspect*b_temp - K;
			r_tempR = aspect*t_temp + K;       
		    l_tempR = aspect*b_temp + K;
			
			Y_delta = (t_temp -(b_temp));
			X_deltaL = (r_tempL -(l_tempL));
			X_deltaR = (r_tempR -(l_tempR));
			
			hfWL = hapticSpanX/X_deltaL;
			hfWR = hapticSpanX/X_deltaR;

			xi_nL =  -0.5*w + xi_nL + uo_L;
			xi_nR =  -0.5*w + xi_nR + uo_R;
			yi_nL =  -0.5*h + yi_nL + vo_L;
			yi_nR =  -0.5*h + (yi_nR + vo_R);

		/*	if (hapticSpanX > X_deltaL){
				xi_nL  = X_haptic/hfWL - 0.25*X_deltaL;
				    
			}else{
				xi_nL  = X_haptic*hfWL - 0.25*X_deltaL;
				
			}
			if (hapticSpanX > X_deltaR){
				
				xi_nR  = X_haptic/hfWR - 0.25*X_deltaR;
			}else{
				
				xi_nR  = X_haptic*hfWR - 0.25*X_deltaR;
			}
			if (hapticSpanY > Y_delta){
				
				yi_nL =  Y_haptic/hfH - 0.5*Y_delta;	
				yi_nR =  Y_haptic/hfH - 0.5*Y_delta;
			}else{
				yi_nL =  Y_haptic*hfH - 0.5*Y_delta;	
				yi_nR =  Y_haptic*hfH - 0.5*Y_delta;
			}*/
			

		    //*******************************************************************************
		    //      QUAD BUFFER INITIALIZATION
		    glViewport (0, 0, 1000, 800);                           // sets drawing viewport
	    /*
		    TOE-IN method for rendering     

		    glMatrixMode(GL_PROJECTION);
		    glLoadIdentity();
		    //glFrustum(576.0,576.0,720.0,720.0,nearZ,farZ);        // set frustum to see
		    gluPerspective(fovy, aspect, nearZ, farZ);              // sets frustum using gluPerspective
	    */
		    // Off-axis method for rendering

		    glMatrixMode(GL_PROJECTION);
		    glLoadIdentity();
		    r = aspect*t - K;       
		    l = aspect*b - K;
		    glFrustum(l,r,b,t,nearZ,farZ);                          // delimits the frustum perspective

			// 3D pointer left coordinates
			/*float hfW_StereoL = (fabs(l) + fabs(r))/w;
			float hfH_StereoL = (fabs(t) + fabs(b))/h;
			float hfW_frustrumL = w/sqrt(pow((r - l),2));
			float hfH_frustrumL = h/sqrt(pow((t - b),2));*/
			xi_nL = xi_nL;
			yi_nL = yi_nL;

		    glMatrixMode(GL_MODELVIEW);
		    glDrawBuffer(GL_BACK_LEFT);                             // draw into back left buffer
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear color and depth buffers
					
		    glLoadIdentity();                                        // reset modelview matrix

		    gluLookAt(-IOD.getValue()/2,0.0,0.0,    //set camera position  x=-IOD/2
			      -IOD.getValue()/2,0.0,screenZ,//set camera "look at" x=0.0
					    0.0,1.0,0.0);   //set camera up vector

		    //*******************************************************************************
		    glPushMatrix();
		    glTranslatef(0.0,0.0,depthZ);

		    //      DRAW THE LEFT IMAGE

			dataToTexture(pthis->imageL.getValue(size,components),w,h,iTexture[0]);
			glEnable(GL_TEXTURE_RECTANGLE_NV);
		    //glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,pthis->imageL.getValue(size,components));
		    //      modifying for stereo
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
		    glDisable(GL_TEXTURE_RECTANGLE_NV);
		    // draw 3d scene
		    // move to tha actual position given by the haptic
			// use negative values to correct the real movement on the screen
			
		    glTranslatef(-xi_nL,yi_nL,screenZ);//
		    // drawing a 3D pointer
		    GLUquadric* quadL = gluNewQuadric();
		    GLdouble radius = 5;
		    GLdouble slices = 20;
		    GLdouble stacks = 20;
		    gluSphere(quadL,radius,slices,stacks);
		    glTranslatef(xi_nL,-yi_nL,-screenZ);

		    glPopMatrix();
	    //**************************************************************************
	    // Off-axis method for rendering

		    glMatrixMode(GL_PROJECTION);
		    glLoadIdentity();
		    r = aspect*t + K;       
		    l = aspect*b + K;
		    glFrustum(l,r,b,t,nearZ,farZ);                          // delimits the frustum perspective

			// 3D pointer right coordinates
			/*float hfW_StereoR = (fabs(l) + fabs(r))/w;
			float hfH_StereoR = (fabs(t) + fabs(b))/h;
			float hfW_frustrumR = sqrt(pow((r - l),2))/hapticSpanX;
			float hfH_frustrumR = sqrt(pow((t - b),2))/hapticSpanY;*/
			xi_nR = xi_nR;
			yi_nR = yi_nR;

	    //      Draw right Image in buffer

		    glDrawBuffer(GL_BACK_RIGHT);                            //draw into back right buffer
		    glMatrixMode(GL_MODELVIEW);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     // clear color and depth buffers
		    glLoadIdentity();    

	    /*
		    rendering methods
	    
	    1) Toe-in: the left and right cameras look at the center of vergence at the screen
			    plane, this method introduces a vertical parallax

	    2)Off-axis: correct rendering method for planar screen surfaces, it does not introduces                 vertical parallax for a better stereo effect, the left and right                cameras point to two different points but that are parallel one to other.

	    */
		    //as for the left buffer but with camera position at:
		    gluLookAt(IOD.getValue()/2, 0.0, 0.0, 
			      IOD.getValue()/2, 0.0, screenZ,            
					  0.0, 1.0, 0.0);
	    //**************************************************************************

		    glPushMatrix();
		    glTranslatef(0.0,0.0,depthZ);

	    //      DRAW THE RIGHT IMAGE
			dataToTexture(pthis->imageR.getValue(size,components),w,h,iTexture[1]);
			glEnable(GL_TEXTURE_RECTANGLE_NV);
		    //glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,pthis->imageR.getValue(size,components));
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
			glDisable(GL_TEXTURE_RECTANGLE_NV);
		    // draw 3d scene
		    // move to tha actual position given by the haptic
			
			//xi_nR = xi_nR - Z_haptic;//hfZ
		    glTranslatef(-xi_nR ,yi_nR,screenZ);
		    // drawing a 3D pointer
		    GLUquadric* quadR = gluNewQuadric();
		    gluSphere(quadR,radius,slices,stacks);
			glTranslatef(xi_nR,-yi_nL,-screenZ);
		    
		    glPopMatrix();
	    }
	    //*******************************************************************************
	    //      swap Buffers
	    //      see the openGL programming Guide appendix C
	    
	    //glXSwapBuffers(glXGetCurrentDisplay(),glXGetCurrentDrawable());
	    //glReadBuffer(GL_BACK_LEFT);
	    //glDrawBuffer(GL_FRONT_LEFT);
	    //glCopyPixels(0, 0, 720, 576, GL_COLOR);
	    //*******************************************************************************
	    else{   // if stereo is not supported, draw the two videos separately

	    //      DRAW THE LEFT IMAGE
        
                
	    //      attach texture to FBO offscreen render

                //      load data

                dataToTexture(pthis->imageL.getValue(size,components),w,h,iTexture[0]);
		
/*      
                //      render to screen
                glGenFramebuffersEXT(1,&fbo);
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fbo);
                //glBindTexture(GL_TEXTURE_RECTANGLE_NV,iTexture[1]);
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,
                                          GL_TEXTURE_RECTANGLE_NV,iTexture[1],0);

                if(!check_FBO_Status())
                {
                        printf("glFramebufferTexture2DEXT():\t[FAIL]\n");
                        //exit(ERROR_FBOTEXTURE);
                }else
                        {
                        //printf("glFramebufferTexture2DEXT():\t[PASS]\n");
                }
*/
/*              
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT1_EXT,
                                          GL_TEXTURE_RECTANGLE_NV,iTexture[1],0);
        
        //      check if FBO worked

                if(!check_FBO_Status())
                {
                        printf("glFramebufferTexture2DEXT():\t[FAIL]\n");
                        //exit(ERROR_FBOTEXTURE);
                }else
                        {
                        //printf("glFramebufferTexture2DEXT():\t[PASS]\n");
                }
*/              
                //      set the texture as render target
/*
                glGenFramebuffersEXT(1,&fbo);
                glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,fbo);
                glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,GL_COLOR_ATTACHMENT0_EXT,
                                          GL_TEXTURE_RECTANGLE_NV,iTexture[1],0);

                
*/      

/*      
                // create the edge detection cg program 
                fragmentProgram = cgCreateProgram(cgContext, CG_SOURCE,
                        Filters[5],cgProfile,"edges",NULL);
                checkForCgError();
                // create the texture parameter to access to the texture image read

                if (fragmentProgram != NULL)
                {
                        cgGLLoadProgram(fragmentProgram);
                        checkForCgError();
                        textureParam = cgGetNamedParameter(fragmentProgram,"Image");
                        checkForCgError();
                //printf("Cg program loaded\n");
                }else
                {
                fragmentProgram=0;
                printf("Cg program not loaded\n");
                }               

                int vp[4];
                glGetIntegerv(GL_VIEWPORT,vp);
                
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                //glPushMatrix();
                
                gluOrtho2D(0,width.getValue(),0,heigh.getValue());      //      image projection
                //glFrustum(0.0,1.0,0.0,1.0,1.0,screenZ);       // set frustum to see

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glViewport(0.0,0.0,width.getValue(), heigh.getValue());

                //      Cg filter: run the cg program
                cgGLBindProgram(fragmentProgram);
                checkForCgError();
                
                //      Bind the scene texture as input to the filter

                cgGLEnableProfile(cgProfile);
                checkForCgError();

                //glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

                cgGLSetTextureParameter(textureParam,iTexture[0]);
                checkForCgError();
                cgGLEnableTextureParameter(textureParam);
                checkForCgError();

                //      execute the filter

                glPolygonMode(GL_FRONT,GL_FILL);                

                glBegin(GL_QUADS);
                        glTexCoord2f(0.0,0.0);
                                glVertex3f( 0.0, heigh.getValue(),0.0);
                        glTexCoord2f(0.0,heigh.getValue());     
                                glVertex3f( 0.0, 0.0 , 0.0);
                        glTexCoord2f(width.getValue(),heigh.getValue());
                                glVertex3f( width.getValue(), 0.0,0.0);
                        glTexCoord2f(width.getValue(),0.0);
                                glVertex3f( width.getValue(),heigh.getValue(),0.0);
                glEnd();

                //glPopMatrix();
                //      Disable the filter

                cgGLDisableTextureParameter(textureParam);
                checkForCgError();
                cgGLDisableProfile(cgProfile);
                checkForCgError();

                glViewport(vp[0],vp[1],vp[2],vp[3]);
                //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,0);
                //glDeleteFramebuffersEXT(1,&fbo);
                
*/              
        //      DRAW THE RIGHT IMAGE
/*
                glTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,pthis->imageR.getValue(size,components));
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
        //      set to the default window framebuffer
                
        //glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
        //glReadPixels(0,0,720,576,GL_RGB,GL_FLOAT,resultCg);

		// RENDERING 3D POINTER PIXELS POSITIONS
		//  Scaling factor for showing the 2 images for left and right
		//  it is the case where no Stereo has been selected

		if (hapticSpanX > w){
			xi_nL  = - w + (xi_nL + uo_L );
			xi_nR  =   xi_nR + uo_R;
		}else{
			xi_nL  = - w + (xi_nL + uo_L );
			xi_nR  =   xi_nR + uo_R;
		}
		if (hapticSpanY > h){
		
			yi_nL =  -0.5*h + (yi_nL + vo_L);	
			yi_nR =  -0.5*h + (yi_nR + vo_R );
		}else{
			yi_nL =  -0.5*h + (yi_nL + vo_L);	
			yi_nR =  -0.5*h + (yi_nR + vo_R );
		}
	
	#if _WIN32
		// check for support for 
		bool isNV_RECTANGLE = false;
		bool isARB_MultiTexture = false;
		isNV_RECTANGLE= isExtensionSupported("GL_NV_texture_rectangle");
		isARB_MultiTexture = isExtensionSupported("GL_ARB_multitexture");
		if (isNV_RECTANGLE & isARB_MultiTexture){			
			
			//pglActiveTextureARB(GL_TEXTURE0_ARB);
			// LEFT IMAGE
			glBindTexture(GL_TEXTURE_RECTANGLE_NV,iTexture[0]);

			glEnable(GL_TEXTURE_RECTANGLE_NV);

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
			glDisable(GL_TEXTURE_RECTANGLE_NV);
		}
	#endif
	#if linux
	
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_NV,iTexture[0]);
	#endif

		// draw 3d scene
		// move to tha actual position given by the haptic
		// use negative values to correct the real movement on the screen
		glTranslatef(xi_nL,yi_nL,5.0);
		// drawing a 3D pointer
		GLUquadric* quadL = gluNewQuadric();
		GLdouble radius = 3;
		GLdouble slices = 20;
		GLdouble stacks = 20;
		gluSphere(quadL,radius,slices,stacks);
		// drift to compensate the before traslation, it keep at the final
		// of the transform the correct frame reference
		glTranslatef(-xi_nL,-yi_nL,-5.0);		
		// draw the right texture
		dataToTexture(pthis->imageR.getValue(size,components),w,h,iTexture[1]);

	#if _WIN32
		//pglActiveTextureARB(GL_TEXTURE1_ARB);
		// RIGHT IMAGE
		glBindTexture(GL_TEXTURE_RECTANGLE_NV,iTexture[1]);			
		glEnable(GL_TEXTURE_RECTANGLE_NV);

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
		glDisable(GL_TEXTURE_RECTANGLE_NV);	
		
	#endif
	#if linux
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glBindTexture(GL_TEXTURE_RECTANGLE_NV,iTexture[1]);
		
		glEnable(GL_TEXTURE_RECTANGLE_NV);
	#endif
			

		// draw 3d scene
		// move to tha actual position given by the haptic
		glTranslatef(xi_nR,yi_nR,5.0);
		// drawing a 3D pointer
		GLUquadric* quadR = gluNewQuadric();
		gluSphere(quadR,radius,slices,stacks);
		// drift to compensate the before traslation, it keep at the final
		// of the transform the correct frame reference
		glTranslatef(-xi_nR,-yi_nR,-5.0);	

	    }

        //      restore viewport

        // copy the results
        //glBindTexture(GL_TEXTURE_RECTANGLE_NV,iTexture[0]);
        //glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV,0,0,0,0,0,720,576);
        
  
                
      }		//    USING GL_RECTANGLENV
		//glDeleteFramebuffersEXT(1,&fbo);
		glDeleteTextures(4,iTexture);
			
		glPopMatrix();
        
     // endSolidShape(action);
        
}
//      generate triangles representing texture planes
void SoStereoTexture::generatePrimitives(SoAction *action)
{
        
        
}
//      computs the bounding box and the center of the quads squares
void SoStereoTexture::computeBBox(SoAction *, SbBox3f &box, SbVec3f &center)
{
        //      maximum extension of the box
        SbVec3f min,max;        
        //      minimum
        min.setValue(-width.getValue(),
                     -heigh.getValue(),
                     -width.getValue()/heigh.getValue());
        //      maximum
        max.setValue(width.getValue(),
                     heigh.getValue(),
                     width.getValue()/heigh.getValue());
        //      set the box limits
        box.setBounds(min,max);
        //      natural center of quad square
        center.setValue(0.0,0.0,0.0);

}
void SoStereoTexture::synchronize()
{

}
void SoStereoTexture::show()
{

}
/// get the projected points from the haptic to the stereo images
/// at the rendering scene and the center of cameras at image coordinates
imagePoints SoStereoTexture::getProjectedPoints()
{
	imagePoints points;
	points.xiL = xi_nL;
	points.xiR = xi_nR;
	points.yiL = yi_nL;
	points.yiR = yi_nR;
	// center of cameras
	points.uo_L = uo_L;
	points.vo_L = vo_L;
	points.uo_R = uo_R;
	points.vo_R = vo_R;
	return points;
}

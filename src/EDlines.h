/* *******************************************************************************
//              EDLines DETECTOR CLASS

//      Author:         Henry Portilla
//      Date:           November/2011
//      Modified:       

//      Thanks:         

//      Description:    This class  detect the edges and line segments from an image
						using the technique called Edge Drawings.
						The code is inspired by the paper from C. Akinlar and C. Tipal
						(Pattern Recognition Letters Vol 32 2011 pp 1633-1642)
						and information from their website
						http://ceng.anadolu.edu.tr/CV/EdgeDrawing/ visited on 09/11/2011

		License:		This code can be used freely for research purposes                                                                                */


#ifndef EDLINES_H
#define EDLINES_H

// include common types
#include "common.h"
#include <vector>
// openCV headers
#include <opencv/cv.h>
#include <opencv/cvaux.h>			
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

enum Route {LEFT_ROUTE,UP_ROUTE,RIGHT_ROUTE,DOWN_ROUTE};
enum Orientation {HORIZONTAL, VERTICAL=90};
typedef std::vector<EdgePoint> PixelChain;

class EDlines
{
public:
	EDlines();
	EDlines(IplImage *GrayImage);
	~EDlines(void);
	// methods
	// Edge Drawing
	void SmoothImage(int WindowsSize,float sigma);
	void ApplyImageGradient(int WindowsSize,double gradient_Threshold);
	std::vector<Anchors_ED> FindEdgeAnchors(int ScanInterval,float AnchorThreshold);
	void EdgeLinking(std::vector<Anchors_ED> Anchors);
	IplImage* EdgeDrawing(IplImage* edgeImage,int WindowSize,double gradient_Threshold);
	void GoTroughRoute(Route path,Orientation EdgeDirection,int x,int y,double currentPixelGradient,double currentPixelDirection,double currentPixelEdgel);
	// ED lines
	std::vector<lineParameters> LineFit(PixelChain pixels,int MinLineLength,double &leastSquareError,Orientation lineType);
	void LeastSquareLineFit(PixelChain pixels,int MinLineLength,lineParameters &lineEquation,double &lineFitError,Orientation lineType, int startPixel);
	double ComputePointDistance2Line(lineParameters lineEquation,EdgePoint point);
	std::vector<lineParameters> EdgeFindLines(int MinimalLength); 


	CvMat* getSmoothImage();
	CvMat* getGradientImage();
	Gradient_Properties GetImagePointGradient(CvMat *image,int x,int y);
	
private:
	IplImage *InputImage;
	IplImage *imageToProcess;
	CvMat *SmoothedImage;
	CvMat *GradientImage;
	CvMat *AnchorsImage;
	CvMat *EdgeImage;
	CvMat *GradientMap;
	CvSize imgSize;
	std::vector<PixelChain> FoundLineSegments;

};

#endif //EDLINES.H
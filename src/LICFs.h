/* *******************************************************************************
//              LICFS DETECTOR CLASS

//      Author:         Henry Portilla
//      Date:           September/2011
//      Modified:       

//      Thanks:         

//      Description:    This class  detect the Line Intersection Context Features
						in an image. The code is inspired by the papers from Kim 
						and Lee (ICRA'10, ICRA'11, IROS'10)

		License:		This code can be used freely for research purposes

                                                                                */
#pragma once

// include common types
#include "common.h"

// openCV headers
#include <opencv/cv.h>
#include <opencv/cvaux.h>			
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

enum IMAGE_TYPE{LEFT = 1, RIGHT = 2};
using namespace std;


class LICFs
{
public:
	LICFs(IplImage *image);
	~LICFs(void);

	// methods to process the image
	void GetSubImage(imagePoints actualImages_Points, float percentage, IMAGE_TYPE imageType);
	void ApplyCannyEdgeDetector(double CannyWindowSize,
		double thresholdCannyLow,double thresholdCannyHigh);
	CvSeq* ApplyHoughLineDetection(int HoughThreshold,
		double HoughMinLengthDetection,double HoughMaxGapBetweenLines);
	vector<LICFs_Structure> ApplyLICF_Detection(CvSeq *imageHoughLines,int LICF_MaxDistanceBetweenLines);
	vector<Matching_LICFs> ApplyMatchingLICFs(IplImage *SubImageToMatch,float threshold, int Windowsize);
	// methods to obtain internal variables
	SubArea_Structure GetSubAreaBoundaries(void);
	IplImage* GetSubImageGray(void);


private:
	// OpenCV variables
	IplImage *imageOriginal;
	IplImage *SubImage;		
	IplImage *SubImageGray;	
	IplImage *HoughSubImage;	
	IplImage *EdgeSubImage;
	CvMat *LICF_feature;
	IplImage *grayImageToMatch;

	CvSize imgSize;
	CvSize SubImageSize;
	CvPoint2D32f SubImageCenter;
	
	CvPoint UpperLeft,LowerRight;
	CvMemStorage* HoughStorage;
	CvSeq* lines;
	imagePoints actualImages_Points;
	vector<LICFs_Structure> Actual_LICFs;
	IMAGE_TYPE imgType; 
	SubArea_Structure SubAreaLimits;
	double CannyWindowSize;
	double thresholdCannyLow;
	double thresholdCannyHigh;
	int HoughThreshold;
	double HoughMinLengthDetection;
	double HoughMaxGapBetweenLines;
	int LICF_MaxDistanceBetweenLines;

	// matching variables
	CvMat *matchOnImageResults;
	CvPoint2D32f LICF_FeatureCenter;
	int LICFs_matchCounter;
	vector<Matching_LICFs> Actual_Matched_LICFs;
	double minVal;
    double maxVal;
    CvPoint* minLoc;
    CvPoint* maxLoc;
	int I_height;int I_width;
    CvPoint pt;
};

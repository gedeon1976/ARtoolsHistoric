/* *******************************************************************************
//              LICFS DETECTOR CLASS

//      Author:         Henry Portilla
//      Date:           September/2011
//      Modified:       

//      Thanks:         

//      Description:    This class  detect the Line Intersection Context Features
						in an image. The code is inspired by the papers from Kim 
						and Lee (ICRA'10, ICRA'11, IROS'10)

		License:		This code can be used freely for research(no military) purposes

**********************************************************************************/
#ifndef LICFS_H
#define LICFS_H

// include common types
#include "common.h"
#include <algorithm>
#include <list>

//#include EDlines class
#include "EDlines.h"
// openCV headers
//#include <opencv/cv.h>
//#include <opencv/cvaux.h>			
#include <opencv/cxcore.h>
#include <opencv/highgui.h>

enum IMAGE_TYPE{LEFT = 1, RIGHT = 2};
using namespace std;

struct LICFs_EpipolarConstraintResult{
	// this structure save the found epipolar lines
	// a similar structure also lives in Pointer_3D.h
	// for now we'll have repeated by in the future all
	// must go in the common.h header file
	float  errorValue;
    CvMat* EpilineL;
    CvMat* EpilineR;
};

//class LICFs:public QObject
//{
//Q_OBJECT
class LICFs{
public:
	LICFs();
	LICFs(IplImage *image);
	~LICFs(void);

	// methods to process the image
	void GetSubImage(imagePoints actualImages_Points, float percentage, IMAGE_TYPE imageType);
	void ApplyCannyEdgeDetector(double CannyWindowSize,
		double thresholdCannyLow,double thresholdCannyHigh);

	vector<lineParameters> ApplyEdgeDrawingEdgeDetector(int MinLineLength);
	CvSeq* ApplyHoughLineDetection(int HoughThreshold,
		double HoughMinLengthDetection,double HoughMaxGapBetweenLines);

	lineIntersection GetLineIntersection(lineParameters L1, lineParameters L2);
	vector<LICFs_Structure> ApplyLICF_Detection(CvSeq *imageHoughLines,int LICF_MaxDistanceBetweenLines);
	vector<LICFs_Structure> ApplyLICF_Detection(vector<lineParameters> LineSegments,int LICF_MaxDistanceBetweenLines);
	double GetLICFs_NCC(CvMat *LICF_feature,CvMat *LICF_featureOtherImage);
	vector<Matching_LICFs> ApplyMatchingLICFs(IplImage *SubImageToMatch,vector<LICFs_Structure> LICFs_otherImage,
		float threshold, int Windowsize);
	vector<Matching_LICFs> RefineMatchingLICFs(CvMat* F_matrix,vector<Matching_LICFs> actualMatchingLICFs,
		SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR, float maxError);

	LICFs_EpipolarConstraintResult GetEpipolarConstraintError(vector<Matching_LICFs> matchedPoints,CvMat* F_matrix,
		SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR);

	cv::Mat FindLICF_BasedHomography(vector<Matching_LICFs> matchedPoints,cv::Mat F_matrix, cv::Mat epipole,
		cv::Mat epipole_prim,SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR);

	cv::Mat FindLICF_BasedHomographyZissermman(vector<Matching_LICFs> matchedPoints,cv::Mat F_matrix, cv::Mat epipole,
		cv::Mat epipole_prim,SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR);

	double CheckHomographyMatrix(vector<cv::Point2f> x, vector<cv::Point2f> x_prim, cv::Mat H_matrix);
	// methods to obtain internal variables
	SubArea_Structure GetSubAreaBoundaries(void);
	IplImage* GetSubImageGray(void);
	// fast testing do the Occlusion methods here
	Visibility_Status visibility(IplImage *leftGrayImage,imagePoints &actualImages_Points, IplImage *OcclusionSubImage);
	void MatchCensusTemplate(CvMat *ImageToSearch,CvMat *feature,CvMat *Results);
	CvMat* CensusTransform(CvMat* Image);


//signals:
	//void SendEpipolarErrorConstraint(float ErrorValue);


private:
	// OpenCV variables
	IplImage *imageOriginal;
	IplImage *SubImage;		
	IplImage *SubImageGray;	
	IplImage *HoughSubImage;	
	IplImage *EdgeSubImage;
	CvMat *LICF_feature;
	CvMat *LICF_featureOtherImage;
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
	float EpipolarErrorValue;

	// matching variables
	CvMat *matchOnImageResults;
	CvPoint2D32f LICF_FeatureCenter;
	CvPoint2D32f LICF_FeatureCenterOtherImage;
	int LICFs_matchCounter;
	vector<Matching_LICFs> Actual_Matched_LICFs;
	double minVal;
    double maxVal;
    CvPoint* minLoc;
    CvPoint* maxLoc;
	int I_height;int I_width;
    CvPoint pt;
	// epipolar variables
	CvMat* EpilinesL;
	CvMat* EpilinesR;
};
#endif  //LICFS.H

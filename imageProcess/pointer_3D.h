/* ////////////////////////////////////////////////////////

Pointer_3D class

description:

	      This class is used to process images from an
	      general scene using scenes features,
	      Epipolar geometry and triangulation methods

date: 	08/oct/2010 //modified 27/07/2011
author: henry portilla

/////////////////////////////////////////////////////////*/
#pragma once

// OpenCV
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <stdio.h>

// strings and c-strings
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <cstdio>
#include <cmath>
#include <vector>
using namespace std;
const int MAX_CORNERS = 500;
// Data structures
struct MatchedPoints{
    int pointsCount;
    cv::Mat pointsL;
    cv::Mat pointsR;
};
struct vectorMatchedPoints{                     
	vector<cv::Point2f> pointsL;
    vector<cv::Point2f> pointsR;
};
struct EpipolarLines{
	cv::Mat EpilineL;
    cv::Mat EpilineR;
};                
struct Epipoles{
	cv::Mat e;
	cv::Mat e_prim;
};
struct ProjectiveMatrices{
	cv::Mat P;
	cv::Mat P_prim;
};

struct F_new_values{
    float a;
    float b;
    float c;
    float d;
    float f1;
    float f2;
};
struct Homogeneous_vector{
    float x;
    float y;
    float w;

};


class pointer_3D
{
public:
    private:
   
    float A,B,C,D,E,F,G;                                    // coefficients for sextic equation
    float global_minimum;                                   // save the minimum value for t in the sextic
    float global_minimum_root;                              // equation


    // general variables
    IplImage* imgL;
    IplImage* imgR;
    IplImage* img3Dpointer;
    CvSize imgSizeL;
    CvSize imgSizeR;
    CvPoint2D32f* cornersL;
    CvPoint2D32f* cornersR;
    int matchCounter;
    MatchedPoints matchedPointsFound;
    EpipolarLines StereoLines;
    CvPoint matchingCornersL[MAX_CORNERS];
    CvPoint matchingCornersR[MAX_CORNERS];
    double minVal;
    double maxVal;
    int I_height;int I_width;
    CvPoint pt;
    CvPoint* minLoc;
    CvPoint* maxLoc;

	cv::Mat F_matrix;
	cv::Mat e;
	cv::Mat e_prim;
	cv::Mat P;
	cv::Mat P_prim;

	// SURF variables
	Epipoles CamEpipoles;
	vector<cv::KeyPoint> KeyPointsL,KeyPointsR;
	vector<cv::DMatch> good_matches;
	vector<cv::Point2f> pointsL_SURF;
	vector<cv::Point2f> pointsR_SURF;
	vector<cv::Point2f> Normalized_PointsL_SURF;
	vector<cv::Point2f> Normalized_PointsR_SURF;
	cv::Mat F_matrix_SURF;
	
	

    // Variables for triangulation
    CvPoint2D32f point_uLeft;
    CvPoint2D32f point_uRight;
    CvMat *u_primTranspose;
    CvMat *u_Left;


public:
    pointer_3D(void);
    ~pointer_3D(void);
    // methods
    // epipolar geometry
    void LoadImages(IplImage *LeftImage, IplImage *RigthImage);
    void LoadImages(const char* filenameL,const char* filenameR);
    IplImage* GetLeftImage(void);
    IplImage* GetRightImage(void);
    int MatchPoints(float threshold);	
    MatchedPoints GetMatchPoints(void);
	int MatchPointsSURF(void);
	float GetVerticalShifmentSURF(void);
	void GetMatchPointsSURF(void);
	vector<cv::Point2f> GetLeftMatchPointsSURF(void);
	vector<cv::Point2f> GetRightMatchPointsSURF(void);
	vector<cv::Point2f> GetNormalizedLeftMatchPoints(void);
	vector<cv::Point2f> GetNormalizedRightMatchPoints(void);
	vector<cv::Point2f> NormalizePointsSURF(vector<cv::Point2f> OriginalPoints, cv::Mat &T);
    //void FindFundamentalMatrix(void);
	void FindFundamentalMatrixSURF(void);
	float CheckFundamentalMatrix(vector<cv::Point2f> LeftPoints, vector<cv::Point2f> RightPoints, cv::Mat F_matrix);
    //CvMat* GetFundamentalMatrix(void);
	cv::Mat GetFundamentalMatrixSURF(void);
    //void FindEpipoles(void);
	void FindEpipolesSURF(void);
	cv::Mat Get_e_epipole(void);
	cv::Mat Get_e_prim_epipole(void);
    void FindEpipolarLines(void);//CvMat* F_matrix, MatchedPoints points
	void DrawEpipolarLines(cv::Mat &Image,cv::Mat F_matrix,vector<cv::Point2f> points,int pointsFrom);
    EpipolarLines GetEpipolarLines(void);
    //void FindProjectiveMatrices(void);

    //float FindMinorValue(CvMat* Matrix,int i, int j);
    //CvMat* FindInverseMatrix(CvMat* Matrix);
    void StereoRectification();
    void get3Dpoint();
    void SetSelectedPointLeft(CvPoint2D32f leftPoint);
    void SetSelectedPointRight(CvPoint2D32f rightPoint);



};
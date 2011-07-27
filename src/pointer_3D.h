/* ////////////////////////////////////////////////////////

Pointer_3D class

description:

	      This class is used to know a 3D point from an
	      general scene from 2 pictures using scenes features,
	      Epipolar geometry and triangulation methods

date: 	08/oct/2010
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
    CvMat* pointsL;
    CvMat* pointsR;
};
struct EpipolarLines{
    CvMat* EpilineL;
    CvMat* EpilineR;
};
struct Epipoles{
    CvMat* e;
    CvMat* e_prim;
};
struct ProjectiveMatrices{
    CvMat* P;
    CvMat* P_prim;
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

    CvMat* F_matrix;
    CvMat* e;
    CvMat* e_prim;
    CvMat* P;
    CvMat* P_prim;

    // Variables for triangulation
    CvPoint2D32f point_uLeft;
    CvPoint2D32f point_uRight;
    CvMat *u_primTranspose;
    CvMat *u_Left;
    // Hartley'97 method
    double square_distance_equation_part1[3];
    double square_distance_equation_part2[3];
    double square_distance_equation_part3[3];
    double square_distance_equation_part4[3];
    struct T_Transform{
        float values[];
    };
    struct Found_roots{
	int real_roots;
	double real_roots_value[6];
	double global_minimum_root;
    };

    CvMat* RigidTransform;
    CvMat* TransformL;
    CvMat* TransformR;


public:
    pointer_3D(void);
    ~pointer_3D(void);
    // methods
    // epipolar geometry
    void LoadImages(IplImage *LeftImage, IplImage *RigthImage);
    void LoadImages(const char* filenameL,const char* filenameR);
    IplImage* GetLeftImage(void);
    IplImage* GetRightImage(void);
    void MatchPoints(float threshold);
    MatchedPoints GetMatchPoints(void);
    void FindFundamentalMatrix(void);
    CvMat* GetFundamentalMatrix(void);
    void FindEpipoles(void);
    CvMat* Get_e_epipole(void);
    CvMat* Get_e_prim_epipole(void);
    void FindEpipolarLines(void);//CvMat* F_matrix, MatchedPoints points
    EpipolarLines GetEpipolarLines(void);
    void FindProjectiveMatrices(void);
    float FindMinorValue(CvMat* Matrix,int i, int j);
    CvMat* FindInverseMatrix(CvMat* Matrix);
    void StereoRectification();
    void get3Dpoint();
    void SetSelectedPointLeft(CvPoint2D32f leftPoint);
    void SetSelectedPointRight(CvPoint2D32f rightPoint);



};
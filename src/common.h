#ifndef COMMON_H
#define COMMON_H

// include common headers

// openCV headers
//#include <opencv/cv.h>
//#include <opencv/highgui.h>

//using namespace cv;

typedef enum Visibility_Status{UNKNOWN,VISIBLE,INVISIBLE};
typedef enum Points_Origin{LEFT_ORIGIN=1,RIGHT_ORIGIN=2};

struct VisibilityColor{
	// openGL visibility color
	float r;
	float g;
	float b;
	float a;
};

struct imagePoints{
  //	matching image  points
  float xiL;
  float yiL;
  float xiR;
  float yiR;
  float disparityShifment;
  float xiR_occlusion_limit;
  //	center of the cameras from calibration methods
  float uo_L;
  float vo_L;
  float uo_R;
  float vo_R;
};
struct Gradient_Properties{
	// this structure save information about the
	// gradient properties of a given pixel
	float x;
	float y;
	float gradientMagnitude;
	float gradientMagnitudeX;
	float gradientMagnitudeY;
	float direction;	
};
struct Anchors_ED{
	// this structure save information about the
	// anchors of the EDlines method
	float x;
	float y;
	bool is_Anchor;
	bool is_inEdgel_Map;
};
struct EdgePoint{
	// this structure save a pixel belonging to
	// one line segment
	float x;
	float y;
};


struct SubArea_Structure{
	// this structure save the limits points for
	// subareas selected by the 3Dpointer
	int heigh;
	int width;
	float x_AreaCenter;
	float y_AreaCenter;
	
};
struct lineIntersection{
	// this structure save the intersection point between 2 lines
	float xi;
	float yi;
};
struct lineParameters{
	//  this structure save parameters of a line
	// parameter of the equation
	float a;
	float b;
	float c;
	// slope of line
	float m;
	// end points within the image
	float x1;
	float x2;
	float y1;
	float y2;
	float thetaAngle;
	float x_farthest;
	float y_farthest;
	float length;
};
struct LICFs_Structure{
	// LICFs features in an image
	// position of intersection
	float x_xK;
	float y_xK;
	float angleBetweenLines;
	float crossRatio;
	// extreme corners of the area patch
	float x_UpperL_Pk;
	float y_UpperL_Pk;
	float x_LowerR_Pk;
	float y_LowerR_Pk;
	// the associated lines
	lineParameters L_1;
	lineParameters L_2;
};
struct Matching_LICFs{
	// This structure save the matching set
	// for LICFs features for the two subimages
	// being analyzed
	LICFs_Structure MatchLICFs_L;
	LICFs_Structure MatchLICFs_R;
	float epipolar_error;
	float line_matching_error;
};



#endif
#ifndef COMMON_H
#define COMMON_H

struct imagePoints{
  //	matching image  points
  float xiL;
  float yiL;
  float xiR;
  float yiR;
  //	center of the cameras from calibration methods
  float uo_L;
  float vo_L;
  float uo_R;
  float vo_R;
};

struct SubArea_Structure{
	// this structure save the limits points for
	// subareas selected by the 3Dpointer
	int heigh;
	int width;
	float x_AreaCenter;
	float y_AreaCenter;
	
};
struct lineParameters{
	//  this structure save parameters of a line
	// parameter of the equation
	float a;
	float b;
	float c;
	// slope opf line
	float m;
	// end points within the image
	float x1;
	float x2;
	float y1;
	float y2;
};
struct LICFs_Structure{
	// LICFs features in an image
	// position of intersection
	float x_xK;
	float y_xK;
	// extreme corners of the area patch
	float x_UpperL_Pk;
	float y_UpperL_Pk;
	float x_LowerR_Pk;
	float y_LowerR_Pk;
	// the associated lines
	lineParameters L_1;
	lineParameters L_2;
};

#endif
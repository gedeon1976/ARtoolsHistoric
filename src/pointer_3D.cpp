#include "pointer_3D.h"


/////////////////////////////////////////////////////////////
pointer_3D::pointer_3D(void)
{
  // variables initialization

  global_minimum = 0;
  // algebra coefficients
  A = 1;
  B = 1;
  C = 1;
  D = 1;
  E = 1;
  F = 1;
  G = 1;

  // matching
  minVal = 0;
  maxVal = 0;
  I_height = 620;
  I_width = 440;
  pt = cvPoint( I_height/4, I_width/2 );
  minLoc = &pt;
  maxLoc = &pt;
  // Fundamental matrix
  F_matrix = cvCreateMat(3,3,CV_32FC1);
  cvSetZero(F_matrix);
  // epipoles
  e = cvCreateMat(1,3,CV_32FC1);
  e_prim = cvCreateMat(3,1,CV_32FC1);
  cvSetZero(e);
  cvSetZero(e_prim);
  // Projective matrices
  P = cvCreateMat(3,4,CV_32FC1);
  P_prim = cvCreateMat(3,4,CV_32FC1);
  // Hartley'97 method
  RigidTransform = cvCreateMat(3,3,CV_32FC1);
  cvSetZero(RigidTransform);
  TransformL = cvCreateMat(3,3,CV_32FC1);
  cvSetZero(TransformL);
  TransformR = cvCreateMat(3,3,CV_32FC1);
  cvSetZero(TransformR);
  // Selected 3D point
  point_uLeft.x = 1;
  point_uLeft.y = 1;
  point_uRight.x = 2;
  point_uRight.y = 2;

}

pointer_3D::~pointer_3D(void)
{
}
/////////////////////////////////////////////////////////////
void pointer_3D::LoadImages(IplImage *LeftImage, IplImage *RigthImage)
{
  // load gray images
   imgL = LeftImage;
   imgR = RigthImage;
}
/////////////////////////////////////////////////////////////
void pointer_3D::LoadImages(const char* filenameL,const char* filenameR){

  imgL = cvLoadImage(filenameL,CV_LOAD_IMAGE_GRAYSCALE);
  imgR = cvLoadImage(filenameR,CV_LOAD_IMAGE_GRAYSCALE);
/*  cvNamedWindow("Left Image",0);
  cvShowImage("Left Image",imgL);
  cvNamedWindow("Right Image",0);
  cvShowImage("Right Image",imgR);
*/
}
// Get loaded Images
IplImage* pointer_3D::GetLeftImage(void )
{
    IplImage* image;
    image = this->imgL;
    return image;
}
IplImage* pointer_3D::GetRightImage(void){
    IplImage* image;
    image = this->imgR;
    return image;
}


void pointer_3D::MatchPoints(float threshold)
{
  imgSizeL = cvGetSize(imgL);
  imgSizeR = cvGetSize(imgR);
  // get the more relevant features on both images
  int numCorners = MAX_CORNERS;
  int pointCount;
  cornersL = new CvPoint2D32f[MAX_CORNERS];
  cornersR = new CvPoint2D32f[MAX_CORNERS];
  double qualityLevel=0.05;double minDistance=30;

  IplImage* eig_imageL= cvCreateImage(imgSizeL, IPL_DEPTH_32F ,1);
  IplImage* tmp_imageL= cvCreateImage(imgSizeL, IPL_DEPTH_32F, 1);
  CvPoint maxCornersL[MAX_CORNERS];

  IplImage* eig_imageR= cvCreateImage(imgSizeR, IPL_DEPTH_32F ,1);
  IplImage* tmp_imageR= cvCreateImage(imgSizeR, IPL_DEPTH_32F, 1);
  CvPoint maxCornersR[MAX_CORNERS];
  // Find Robust corners on the image

  cvGoodFeaturesToTrack(imgL,eig_imageL,tmp_imageL,cornersL,&numCorners,
			qualityLevel,minDistance);
  cvGoodFeaturesToTrack(imgR,eig_imageR,tmp_imageR,cornersR,&numCorners,
		      qualityLevel,minDistance);
  // Refine the corner locations

  // cvFindCornerSubPix(imgL,cornersL,numCorners,cvSize(11,11),cvSize(-1,-1),
  //		   cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,30,0.01));

  // cvFindCornerSubPix(imgR,cornersR,numCorners,cvSize(11,11),cvSize(-1,-1),
  //		   cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS,30,0.01));
  //show the features

   CvPoint cornerXY_L;
   CvPoint cornerXY_R;

   // init fonts

   CvFont font;
   double hScale = 1.0;
   double vScale = 1.0;
   int lineWidth = 2;
   cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX,hScale,vScale,0,lineWidth);
   char bufferL[50];
   char bufferR[50];
   char bufferMatch[50];
   string strL("left");
   string strR = "right";
   string strMatch("match");
   int width = 1;
   int height = 1;
   // find matching features using 15x15 window size

   CvMat *FeatureSubImgR = cvCreateMat(15,15,CV_8UC1);
   //cvNamedWindow("FeatureSubImgR", 0);
   //cvShowImage("FeatureSubImgR",FeatureSubImgR);

   // find correspond match in L image

   width = (imgL->width - FeatureSubImgR->width + 1);
   height =  (imgL->height - FeatureSubImgR->height +1);
   CvMat *matchOnL = cvCreateMat(height,width,CV_32FC1);
   matchCounter = 0;
   for (int i =0; i<numCorners;i++){

     // get corner positions
     sprintf(bufferL,"%d",i);strL.clear();
     sprintf(bufferR,"%d",i);strR.clear();
     strL.assign(bufferL);
     strR.assign(bufferR);
     cornerXY_L = cvPoint(cvRound(cornersL[i].x),cvRound(cornersL[i].y));

     //cvCircle(imgL,cornerXY_L,7,CV_RGB(255,0,0),3,8,0);
     //sprintf(bufferL,"%d %d %d",i,cornerXY_L.x,cornerXY_L.y);strL.clear();strL.assign(bufferL);
     //cvPutText(imgL,strL.data(),cornerXY_L,&font,cvScalar(255,255,0)); // set name

     cornerXY_R = cvPoint(cvRound(cornersR[i].x),cvRound(cornersR[i].y));

     //        cvCircle(imgT,cornerXY_R,7,CV_RGB(255,0,0),3,3,0);

     //sprintf(bufferR,"%d %d %d",i,cornerXY_R.x,cornerXY_R.y);strR.clear();strR.assign(bufferR);

//     cvPutText(imgT,strR.data(),cornerXY_R,&font,cvScalar(255,255,255));
     // set name
     // extract features subimages from R image
     cvGetRectSubPix(imgR,FeatureSubImgR,cornersR[i]);
     cvMatchTemplate(imgL,FeatureSubImgR,matchOnL,CV_TM_CCORR_NORMED);

     // find place of matching in the left image
     cvMinMaxLoc(matchOnL,&minVal,&maxVal,minLoc,maxLoc);

     // save match point
     if (maxVal > threshold){

      matchCounter = matchCounter + 1;
      matchingCornersL[matchCounter]= *maxLoc;
      matchingCornersR[matchCounter] = cornerXY_R;
      sprintf(bufferMatch,"%f",maxVal);strMatch.clear();
      strMatch.assign(bufferMatch);

      // cvCircle(imgMatch,*maxLoc,7,CV_RGB(255,0,0),3,3,0);
//      cvPutText(imgMatch,strR.data(),*maxLoc,&font,cvScalar(255,255,255));
      //set name

     }

   }

   // print number of matches
   printf("matches number: %d \n",matchCounter);
   pointCount = matchCounter;
   CvMat* pointsL;
   CvMat* pointsR;
   CvMat* status;
   // initialize arrays
   pointsL = cvCreateMat(1,pointCount,CV_32FC2);
   pointsR = cvCreateMat(1,pointCount,CV_32FC2);
   status = cvCreateMat(1,pointCount,CV_8UC1);

   // fill the matching points
   CvScalar Value;
   for (int m =1;m<matchCounter;m++){
     Value.val[0] = matchingCornersL[m].x;
     Value.val[1] = matchingCornersL[m].y;
     cvSet2D(pointsL,0,m,Value);
     Value.val[0] = matchingCornersR[m].x;
     Value.val[1] = matchingCornersR[m].y;
     cvSet2D(pointsR,0,m,Value);
   }

   //cvShowImage("RightImageColor",imgT);

   ////create the window container

   //cvNamedWindow("CornersL", 0);

   //cvNamedWindow("CornersR", 0);

   //cvShowImage("CornersL",imgL);

   //cvShowImage("CornersR",imgR);

   matchedPointsFound.pointsCount = matchCounter;
   matchedPointsFound.pointsL = pointsL;
   matchedPointsFound.pointsR = pointsR;


}
MatchedPoints pointer_3D::GetMatchPoints(void){
  MatchedPoints pointsFound;
  pointsFound = matchedPointsFound;
  return pointsFound;
}
void pointer_3D::FindFundamentalMatrix(void)
{
  //find fundamental matrix
  int F_matrix_count;
  CvMat* pointsL = matchedPointsFound.pointsL;
  CvMat* pointsR = matchedPointsFound.pointsR;

  // find fundamental matrix using Hartley 8 points algorithm
  F_matrix_count =  cvFindFundamentalMat(pointsL,pointsR,F_matrix,CV_FM_8POINT);
  // print F
  printf("\n 8 POINTS\n\n");
  for (int k = 0;k<3;k++){
    printf("%4.4E %4.4E%4.4E\n",cvmGet(F_matrix,k,0),cvmGet(F_matrix,k,1),cvmGet(F_matrix,k,2));

  }

}

CvMat* pointer_3D::GetFundamentalMatrix(void){
  // get the Fundamental Matrix
  CvMat *F;
  F = F_matrix;
  return F;

}


void pointer_3D::FindEpipoles(void)
{
  // Use SVD (Singular Value Decomposition) to find the epipoles e' and e
  // Find the Epipoles using F*e = 0 and e'transpose*F = 0
  // so Null(F) finds e and Null(Ftranspose) finds e'
  // see matrix analysis book (from carl meyer)
  // titled : Matrix Analysis and Applied Linear Algebra
  Epipoles CamEpipoles;
  CvMat* U;
  CvMat* W;
  CvMat* V;
  CvMat* V_Transpose;
  U = cvCreateMat(3,3,CV_32FC1);
  V = cvCreateMat(3,3,CV_32FC1);
  V_Transpose = cvCreateMat(3,3,CV_32FC1);
  W = cvCreateMat(3,3,CV_32FC1);

  cvSVD(F_matrix,W,U,V);
  cvTranspose(V,V_Transpose);
  cvGetRow(V_Transpose,e,2);        // get epipole e from Vtranspose because  SVD gets only V, so by that
  // we take the last column that is the last  row of Vtranspose
  // that is the kernel(F)
  cvGetCol(U,e_prim,2);            // get epipole e'

  // print U
  printf("\n U matrix\n\n");

  for (int k = 0;k<3;k++){

    printf("%E %E %E\n",cvmGet(U,k,0),cvmGet(U,k,1),cvmGet(U,k,2));
  }

  // print W
  printf("\n W matrix\n\n");

  for (int k = 0;k<3;k++){

    printf("%E %E %E\n",cvmGet(W,k,0),cvmGet(W,k,1),cvmGet(W,k,2));
  }
  // print V
  printf("\n V matrix\n\n");

  for (int k = 0;k<3;k++){

    printf("%E %E %E\n",cvmGet(V_Transpose,k,0),cvmGet(V_Transpose,k,1),cvmGet(V_Transpose,k,2));
  }

  // print e 1 row
  printf("\n Epipole e:\n\n");
  printf("%E %E %E\n",cvmGet(e,0,0),cvmGet(e,0,1),cvmGet(e,0,2));

  // print e' 1 column
  printf("\n Epipole e':\n\n");
  printf("%E %E %E\n",cvmGet(e_prim,0,0),cvmGet(e_prim,1,0),cvmGet(e_prim,2,0));

  CamEpipoles.e = e;
  CamEpipoles.e_prim = e_prim;

}

// recovers the epipoles found to the actual geometry
CvMat* pointer_3D::Get_e_epipole(void){

  CvMat *epipole;
  epipole = e;
  return epipole;
}

CvMat* pointer_3D::Get_e_prim_epipole(void){

  CvMat *epipole;
  epipole = e_prim;
  return epipole;
}
void pointer_3D::FindEpipolarLines(void){

  // Find the correspond epipolar lines
  // it serves to check if the matrix F is correct
  int pointsNumber = matchedPointsFound.pointsCount;
  CvMat* pointsL = matchedPointsFound.pointsL;
  CvMat* pointsR = matchedPointsFound.pointsR;

  CvMat* EpilinesL;
  CvMat* EpilinesR;
  EpilinesL = cvCreateMat(3,pointsNumber,CV_32FC1);
  EpilinesR = cvCreateMat(3,pointsNumber,CV_32FC1);
  // compute epipolar lines in the images points in one image correspond to lines in the second image
  // as described in the epipolar geometry
  cvComputeCorrespondEpilines(pointsR,2,F_matrix,EpilinesL);
  cvComputeCorrespondEpilines(pointsL,2,F_matrix,EpilinesR);
  StereoLines.EpilineL = EpilinesL;
  StereoLines.EpilineR = EpilinesR;

}

EpipolarLines pointer_3D::GetEpipolarLines(void )
{
  EpipolarLines lines;
  lines = StereoLines;
  return lines;
}

void pointer_3D::FindProjectiveMatrices(void)
{
  // Find the P and P matrices from e' using result 9.14 from Multiple view  geometry book
  ProjectiveMatrices ProjMatrices;
  CvMat* skew_e_prim;
  CvMat* tempMul;
  tempMul = cvCreateMat(3,3,CV_32FC1);
  skew_e_prim =cvCreateMat(3,3,CV_32FC1);
  cvSetZero(P);
  cvSetZero(P_prim);
  cvSetZero(skew_e_prim);
  // fill skew symetric matrix [e']x
  cvmSet(skew_e_prim,0,1,-1*cvmGet(e_prim,2,0));//-a3
  cvmSet(skew_e_prim,1,0,cvmGet(e_prim,2,0));//a3
  cvmSet(skew_e_prim,2,0,-1*cvmGet(e_prim,1,0));//-a2
  cvmSet(skew_e_prim,0,2,cvmGet(e_prim,1,0));//a2
  cvmSet(skew_e_prim,1,2,-1*cvmGet(e_prim,0,0));//-a1
  cvmSet(skew_e_prim,2,1,cvmGet(e_prim,0,0));//a1

  // print [e`]x = 0
  printf("\n [e']x \n\n");
  for (int k = 0;k<3;k++){

    printf("%E %E %E\n",cvmGet(skew_e_prim,k,0),cvmGet(skew_e_prim,k,1),cvmGet(skew_e_prim,k,2));
  }
  // build P
  cvSetIdentity(P);
  // build P' = [[e']x*F |e']
  cvMatMul(skew_e_prim,F_matrix,tempMul);
  //cvRepeat(tempMul,P_prim);
  for(int k=0;k<3;k++){
    cvmSet(P_prim,k,0,cvmGet(tempMul,k,0));
    cvmSet(P_prim,k,1,cvmGet(tempMul,k,1));
    cvmSet(P_prim,k,2,cvmGet(tempMul,k,2));

  }
  // build last column
  cvmSet(P_prim,0,3,cvmGet(e_prim,0,0));//a1
  cvmSet(P_prim,1,3,cvmGet(e_prim,1,0));//a2
  cvmSet(P_prim,2,3,cvmGet(e_prim,2,0));//a3

  printf("\n P: \n\n");
  for (int k = 0;k<3;k++){

    printf("%E %E %E %E\n",cvmGet(P,k,0),cvmGet(P,k,1),cvmGet(P,k,2),cvmGet(P,k,3));
  }
  printf("\n P': \n\n");
  for (int k = 0;k<3;k++){

    printf("%E %E %E %E\n",cvmGet(P_prim,k,0),cvmGet(P_prim,k,1),cvmGet(P_prim,k,2),cvmGet(P_prim,k,3));
  }

  ProjMatrices.P = P;
  ProjMatrices.P_prim = P_prim;

}

// Find Minor values used on finding inverse matrix
float pointer_3D::FindMinorValue(CvMat* Matrix,int i, int j){

  float final_value;
  CvMat *Minor = cvCreateMat(2,2,CV_32FC1);
  int counter = 0;
  float minor_Values[4];
  for (int m=0;m<3;m++){
    for (int n=0;n<3;n++){
      if ((m != i)&(n != j))
      {
	minor_Values[counter] = cvmGet(Matrix,m,n);
	counter = counter + 1;

      }
    }
  }
  float matrix_values[] = {minor_Values[0],minor_Values[1],
	 minor_Values[2],minor_Values[3]};
	 Minor = cvInitMatHeader(Minor,2,2,CV_32FC1,matrix_values);
	 final_value = cvDet(Minor);
	 return final_value;

}
// Find inverse matrix using Ainv = Aadj/det(A) from Wolfram Mathworld
CvMat* pointer_3D::FindInverseMatrix(CvMat *Matrix){

  float det_Value;
  CvMat *InvMat = cvCreateMat(3,3,CV_32FC1);
  CvMat *Adj = cvCreateMat(3,3,CV_32FC1);
  det_Value = cvDet(Matrix);
  float cofactors[]
  = {FindMinorValue(Matrix,0,0)/det_Value,
    -FindMinorValue(Matrix,1,0)/det_Value,
     FindMinorValue(Matrix,2,0)/det_Value,
    -FindMinorValue(Matrix,0,1)/det_Value,
     FindMinorValue(Matrix,1,1)/det_Value,
    -FindMinorValue(Matrix,2,1)/det_Value,
     FindMinorValue(Matrix,0,2)/det_Value,
    -FindMinorValue(Matrix,1,2)/det_Value,
     FindMinorValue(Matrix,2,2)/det_Value};
  Adj = cvInitMatHeader(Adj,3,3,CV_32FC1,cofactors);
  InvMat = Adj;
  return InvMat;

}
void pointer_3D::FindOrthogonalProjectedPoints(F_new_values Values, float t, Homogeneous_vector point)
{
    // Orthogonal point for epipolar line 1
    float a1 = pow(t,2)*pow(Values.f1,2);
    float a2 = t*Values.f1;
    float a3 = pow(t,2)*Values.f1;
    float a4 = pow(t,2);
    float d1 = a4*(Values.f1 + 1) + 1;
    // Orthogonal point for epipolar line
    float a5 = pow(Values.c*t + Values.d,2);
    float a6 = Values.c*t + Values.d ;
    float a7 = Values.a*t + Values.b;
    float a8 = a6*a7;
    float a9 = Values.f2;
    float d2 = pow(a9*a6,2) + pow(a7,2) + a5;

    CvMat* u_point = cvCreateMat(3,3,CV_32FC1);
    CvMat* u_prim_point = cvCreateMat(3,3,CV_32FC1);
    CvMat* OrthoResultL = cvCreateMat(3,3,CV_32FC1);
    CvMat* OrthoResultR = cvCreateMat(3,3,CV_32FC1);
    CvMat* tempMul3 = cvCreateMat(3,3,CV_32FC1);
    CvMat* tempMul4 = cvCreateMat(3,3,CV_32FC1);
    CvMat* transformL_Inv = cvCreateMat(3,3,CV_32FC1);
    CvMat* transformR_Inv = cvCreateMat(3,3,CV_32FC1);
    CvMat* P_OrthoL = cvCreateMat(3,3,CV_32FC1);
    CvMat* P_OrthoR = cvCreateMat(3,3,CV_32FC1);

    Homogeneous_vector x,x_prim;

    cvSetZero(u_point);
    cvSetZero(u_prim_point);
    cvSetZero(OrthoResultL);
    cvSetZero(OrthoResultR);
    cvSetZero(transformL_Inv);
    cvSetZero(transformR_Inv);
    cvSetZero(tempMul3);
    cvSetZero(tempMul4);
    // orthogonal projection onto left epipolar line
    float P_Ortho_line1[] = { a1/d1, a2/d1, -a3/d1,
			      a2/d1,  1/d1, -t/d1,
			     -a3/d1, -t/d1,  a4/d1};
    P_OrthoL = cvInitMatHeader(P_OrthoL,3,3,CV_32FC1,P_Ortho_line1);
    // orthogonal projection onto right epipolar line
    float P_Ortho_line2[] = { (pow(a9,2)*a5)/d2, -(a9*a8)/d2, -(a9*a5)/d2,
			            -(a9*a8)/d2,pow(a7,2)/d2,     (a8)/d2,
			             (a5*a9)/d2,     (a8)/d2,       a5/d2};
    P_OrthoR = cvInitMatHeader(P_OrthoR,3,3,CV_32FC1,P_Ortho_line2);

    //  u = P_Ortho_line1*point
    cvmSet(u_point,0,0,point.x);
    cvmSet(u_point,1,0,point.y);
    cvmSet(u_point,2,0,point.w);
    cvMatMul(P_OrthoL,u_point,OrthoResultL);
     // print results
    printf("\n x final point homogeneus: \n\n");
    for (int k = 0;k<3;k++){

      printf("%E %E %E\n",cvmGet(OrthoResultL,k,0),cvmGet(OrthoResultL,k,1),cvmGet(OrthoResultL,k,2));

    }
    // find the corresponding points on the left image
    cvInvert(TransformL,transformL_Inv,CV_SVD);
    cvmSet(OrthoResultL,2,2,1);
    cvMatMul(transformL_Inv,OrthoResultL,tempMul3);
    cvMatMul(transformL_Inv,u_point,tempMul4);
    printf("\n x final point image coordinates: \n\n");
    for (int k = 0;k<3;k++){

      printf("%E %E %E\n",cvmGet(tempMul3,k,0),cvmGet(tempMul3,k,1),cvmGet(tempMul3,k,2));
      printf("%E %E %E\n",cvmGet(tempMul4,k,0),cvmGet(tempMul4,k,1),cvmGet(tempMul4,k,2));

    }

    cvMatMul(P_OrthoR,u_point,OrthoResultR);
     // print results
    printf("\n x' final point: \n\n");
    for (int k = 0;k<3;k++){

      printf("%E %E %E\n",cvmGet(OrthoResultR,k,0),cvmGet(OrthoResultR,k,1),cvmGet(OrthoResultR,k,2));

    }
   // find the corresponding points on the right image
    cvInvert(TransformR,transformR_Inv,CV_SVD);
    cvMatMul(transformR_Inv,OrthoResultR,tempMul3);
    cvmSet(OrthoResultR,2,2,1);
    cvMatMul(transformR_Inv,u_point,tempMul4);
    printf("\n x' final point image coordinates: \n\n");
    for (int k = 0;k<3;k++){

      printf("%E %E %E\n",cvmGet(tempMul3,k,0),cvmGet(tempMul3,k,1),cvmGet(tempMul3,k,2));
      printf("%E %E %E\n",cvmGet(tempMul4,k,0),cvmGet(tempMul4,k,1),cvmGet(tempMul4,k,2));

    }
}
// Find the coordinates for the images where is
// supposed to be the correct points that projected
// the 3D point that we want to know
void pointer_3D::Find3DimagePoints(F_new_values Values, float t)
{
    Homogeneous_vector uL,uR;
    // points are mapped to origin homogeneous coordinates
    uL.x = 0;uR.x = 0;
    uL.y = 0;uR.y = 0;
    uL.w = 1;uR.w = 1;
    FindOrthogonalProjectedPoints(Values, t, uL);

}
void pointer_3D::SetSelectedPointLeft(CvPoint2D32f leftPoint)
{
  point_uLeft = leftPoint;
}
void pointer_3D::SetSelectedPointRight(CvPoint2D32f rightPoint)
{
  point_uRight = rightPoint;
}

// Mouse interfacing for testing
void OnMouse(int event, int x, int y, int flags, void* param){

  // get the current states from the actual pointer_3D instance
  CvPoint2D32f leftPoint;
  CvPoint2D32f rightPoint;
  pointer_3D *cursor3D = (pointer_3D*)param;
  // draw epipolar line in the right image
  // using the ax + by + c = 0 equation
  IplImage *img= cursor3D->GetRightImage();
  CvMat *F_matrix = cursor3D->GetFundamentalMatrix();
  CvMat *selectedPoint = cvCreateMat(3,3,CV_32FC1);
  CvMat *EpipolarLine = cvCreateMat(3,3,CV_32FC1);
  cvSetZero(selectedPoint);
  cvSetZero(EpipolarLine);
  int width = img->width;
  int height = img->height;
  // get the image point selected
  cvmSet(selectedPoint,0,0,x);
  cvmSet(selectedPoint,1,0,y);
  cvmSet(selectedPoint,2,0,1);
  // set the global 3D point
  leftPoint.x = x;
  leftPoint.y = y;
  cursor3D->SetSelectedPointLeft(leftPoint);

  switch(event){

    case CV_EVENT_LBUTTONDOWN:
      cvMatMul(F_matrix,selectedPoint,EpipolarLine);
      printf("\n Epipolar line R \n\n");
      for (int k = 0;k<3;k++){

	printf("%E %E %E\n",cvmGet(EpipolarLine,k,0),cvmGet(EpipolarLine,k,1),cvmGet(EpipolarLine,k,2));
      }
      float A = cvmGet(EpipolarLine,0,0);
      float B = cvmGet(EpipolarLine,1,0);
      float C = cvmGet(EpipolarLine,2,0);
      CvPoint Po = cvPoint(1, -(A + C)/B);
      CvPoint P1 = cvPoint(width,-(A*width + C)/B);
      cout<<"point 0 "<<Po.x<<" "<<Po.y<<endl;
      cout<<"point 1 "<<P1.x<<" "<<P1.y<<endl;
      cvLine(img,Po,P1,CV_RGB(255,0,0));
      cvCircle(img,cvPoint(x,y),3,CV_RGB(255,255,255));
      // draw initial match point
      int Xline = -(y*B + C)/A;
      int Ypoint = y;
      // set the global 3D point
      rightPoint.x = Xline;
      rightPoint.y = Ypoint;
      cursor3D->SetSelectedPointRight(rightPoint);
      cout<<"point Interception "<<Xline<<" "<<Ypoint<<endl;
      cvCircle(img,cvPoint(Xline,Ypoint),5,CV_RGB(255,255,255));

      cvNamedWindow("Right Image Epipolar Line",0);
      cvShowImage("Right Image Epipolar Line",img);
      //cvReleaseImage(&img);

      // Find the 3D point
      float a,b,c,d,e,f1,f2;
      F_new_values F_values;
      F_values = cursor3D->Find_new_F();
      a = F_values.a;
      b = F_values.b;
      c = F_values.c;
      d = F_values.d;
      f1 = F_values.f1;
      f2 = F_values.f2;
      cursor3D->set_equations(a,b,c,d,f1,f2);
      float minor_root = cursor3D->find_global_minimum();
      cursor3D->Find3DimagePoints(F_values, minor_root);

      break;

  }


}


////////////////////////////////////////////////////////////
//int main(int argc, char**argv)
//{
//
//    // testing from real camera i.e laptop webcam
//    cvNamedWindow("Left Camera",0);
//    cvNamedWindow("Right Camera",0);
//    int device = 1;
//    int device2 = 2;
//     // read the camera
//    CvCapture *capture = cvCaptureFromCAM(device);
//    IplImage *imageFromCAM_L = cvQueryFrame(capture);
//    CvSize imgSize = cvSize(imageFromCAM_L->width,imageFromCAM_L->height);
//    IplImage *imageFromCAM_R;
//    IplImage *GrayImageL = cvCreateImage(imgSize, IPL_DEPTH_8U,1);
//    IplImage *GrayImageR = cvCreateImage(imgSize, IPL_DEPTH_8U,1);
//    cvCvtColor(imageFromCAM_L,GrayImageL,CV_RGB2GRAY);
//    cvShowImage("Left Camera",GrayImageL);
//    cvReleaseCapture(&capture);
//    bool KeyPressed = false;
//    while (!KeyPressed){
//    int key = cvWaitKey(0);
//      if (key == 27){
//	//cvGrabFrame(capture);
//	CvCapture *capture = cvCaptureFromCAM(device2);
//	imageFromCAM_R = cvQueryFrame(capture);
//	cvCvtColor(imageFromCAM_R,GrayImageR,CV_RGB2GRAY);
//	cvShowImage("Right Camera",GrayImageR);
//	KeyPressed = true;
//      }
//    }
//
//    //char* fileL ="/home/henry/projects/3Dpointer/pictures/leftMatching.jpg";
//    //char* fileR ="/home/henry/projects/3Dpointer/pictures/rightMatching.jpg";
//
//    pointer_3D cursor3D;
//    cursor3D.LoadImages(GrayImageL,GrayImageR);
//    //cursor3D.LoadImages(fileL,fileR);
//    cvNamedWindow("Left Image",0);
//    cvShowImage("Left Image",cursor3D.GetLeftImage());
//    cvNamedWindow("Right Image",0);
//    cvShowImage("Right Image",cursor3D.GetRightImage());
//    cursor3D.MatchPoints(0.995);
//    cursor3D.FindFundamentalMatrix();
//    cursor3D.FindEpipoles();
//    cursor3D.FindEpipolarLines();
//    cursor3D.FindProjectiveMatrices();
//
//
//
//    // choose a point using the haptic or the mouse for testing
//    cvSetMouseCallback("Left Image",OnMouse,&cursor3D);
//
//
//    // openCV loop
//    cvWaitKey(0);
//
//    return 0;
//}

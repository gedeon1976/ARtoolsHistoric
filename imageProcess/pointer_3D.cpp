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
  F_matrix_SURF = cv::Mat(3,3,CV_32FC1,cv::Scalar::all(0));
  // epipoles
  e = cv::Mat(1,3,CV_32FC1,cv::Scalar::all(0));
  e_prim = cv::Mat(3,1,CV_32FC1,cv::Scalar::all(0));
  //cvSetZero(e);
  //cvSetZero(e_prim);
  // Projective matrices
  P = cv::Mat(3,4,CV_32FC1,cv::Scalar::all(0));
  P_prim = cv::Mat(3,4,CV_32FC1,cv::Scalar::all(0));
 
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


int pointer_3D::MatchPoints(float threshold)
{
  imgSizeL = cvGetSize(imgL);
  imgSizeR = cvGetSize(imgR);
  // get the more relevant features on both images
  int numCorners = MAX_CORNERS;
  int pointCount;
  cornersL = new CvPoint2D32f[MAX_CORNERS];
  cornersR = new CvPoint2D32f[MAX_CORNERS];
  double qualityLevel=0.05;double minDistance=30;

  IplImage* imgMatch = cvCloneImage(imgL);
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
	 //cvNormalize(matchOnL,matchOnL,0,1,CV_NORM_MINMAX,-1);
     // find place of matching in the left image
     cvMinMaxLoc(matchOnL,&minVal,&maxVal,minLoc,maxLoc);

     // save match point
     if (maxVal > threshold){

      matchCounter = matchCounter + 1;
      matchingCornersL[matchCounter]= *maxLoc;
      matchingCornersR[matchCounter] = cornerXY_R;
      sprintf(bufferMatch,"%f",maxVal);strMatch.clear();
      strMatch.assign(bufferMatch);
	  // put the matches on a test image 	
      cvCircle(imgMatch,*maxLoc,7,CV_RGB(255,0,0),3,3,0);
      cvPutText(imgMatch,strL.data(),*maxLoc,&font,cvScalar(255,255,255));
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
   cvNamedWindow("Matched Points",0);
   cvShowImage("Matched Points",imgMatch);

   //cvNamedWindow("CornersL", 0);

   //cvNamedWindow("CornersR", 0);

   //cvShowImage("CornersL",imgL);

   //cvShowImage("CornersR",imgR);

   matchedPointsFound.pointsCount = matchCounter;
   matchedPointsFound.pointsL = pointsL;
   matchedPointsFound.pointsR = pointsR;
   return pointCount;


}
MatchedPoints pointer_3D::GetMatchPoints(void){
  MatchedPoints pointsFound;
  pointsFound = matchedPointsFound;
  return pointsFound;
}
// find matches using SURF detection
int pointer_3D::MatchPointsSURF(void){
	try{
	// copy images fom loaded IplImages
	cv::Mat matL,matR;
	matL = cv::cvarrToMat(imgL,true);
	matR = cv::cvarrToMat(imgR,true);
	// detecting L and R keypoints
	
	cv::SurfFeatureDetector detector(500);	
	detector.detect(matL,KeyPointsL);
	detector.detect(matR,KeyPointsR);
	// compute L and R descriptors
	cv::SurfDescriptorExtractor SURFDescriptor;
	cv::Mat descriptorsL,descriptorsR;
	SURFDescriptor.compute(matL,KeyPointsL,descriptorsL);
	SURFDescriptor.compute(matR,KeyPointsR,descriptorsR);
	// using descriptors to find matches
	// use brute force matcher one to many search as TemplateMatching
	
	//cv::BruteForceMatcher<cv::L2<float>> matcher;
	//vector<cv::DMatch> matches;
	//matcher.match(descriptorsL,descriptorsR,matches);

	// use flann (Fast Approximate Nearest Neighbor Search Library)
	// to find good matches
	good_matches.clear();	
	cv::FlannBasedMatcher matcher;
	vector<cv::DMatch> matches;
	matcher.match(descriptorsL,descriptorsR,matches);
	printf("FLANN matches: %i\n",matches.size());
	printf("SURF descriptorsL: %i\n",descriptorsL.rows);

	// Calculation of distances of max and min distances among keypoints
	double max_dist = 0; double min_dist = 100;
	double dist = 1;
	for (int i =0; i < matches.size();i++){
		dist = matches[i].distance;
		if (dist < min_dist){ min_dist = dist;}
		if (dist > max_dist){ max_dist = dist;}
	}
	printf(" Flann max dist: %f\n",max_dist);
	printf(" Flann min dist: %f\n",min_dist);

	// get good matches, which distance < 2*min_dist	
	for(int i=0; i < matches.size();i++){
		if(matches[i].distance < 3.5*min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	// draw the found matches
	cv::namedWindow("SURF Matches",1);
	//cv::namedWindow("SURF Left Image",1);
	//cv::namedWindow("SURF Right Image",1);
	cv::Mat SURFmatches;
	cv::drawMatches(matL,KeyPointsL,matR,KeyPointsR,good_matches,SURFmatches,cv::Scalar::all(-1),
		cv::Scalar::all(-1),vector<char>(),cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	cv::imshow("SURF Matches",SURFmatches);
	printf("SURF_FLANN matches: %i\n",good_matches.size());
	//cv::drawKeypoints(matL,KeyPointsL,matL);
	//cv::drawKeypoints(matR,KeyPointsR,matR);
	//cv::imshow("SURF Left Image",matL);
	//cv::imshow("SURF Right Image",matR);

	return good_matches.size();
	}
	catch(...){
		return 0;
	}

}
// Get matches found trough SURF detection
void pointer_3D::GetMatchPointsSURF(void){
	try{
		int sizeMatches;
		cv::Point2f pointL,pointR;
		sizeMatches = good_matches.size();
		for (int i = 0;i < sizeMatches;i++){
			pointL = KeyPointsL[good_matches[i].queryIdx].pt;
			pointR = KeyPointsR[good_matches[i].trainIdx].pt;
			// get the matching points
			pointsL_SURF.push_back(pointL);
			pointsR_SURF.push_back(pointR);			
		}
		printf("\n Getting SURF matching Points %i \n\n",pointsL_SURF.size());
	}
	catch(...)
	{
	}

}
// get normal matched points
vector<cv::Point2f> pointer_3D::GetLeftMatchPointsSURF(void){
	try{
		vector<cv::Point2f> Points;
		// normalized points to get better results
		// and invariance to the image origin and scaling
		Points = pointsL_SURF;
		return Points;
	}
	catch(...){
	}
}

// get normal matched points
vector<cv::Point2f> pointer_3D::GetRightMatchPointsSURF(void){
	try{
		vector<cv::Point2f> Points;
		// normalized points to get better results
		// and invariance to the image origin and scaling
		Points = pointsR_SURF;
		return Points;
	}
	catch(...){
	}
}

// get the normalized left points
vector<cv::Point2f> pointer_3D::GetNormalizedLeftMatchPoints(void){
	try{
		vector<cv::Point2f> Points;
		// normalized points to get better results
		// and invariance to the image origin and scaling
		Points = Normalized_PointsL_SURF;
		return Points;
	}
	catch(...){
	}
}
// get the normalized right points
vector<cv::Point2f> pointer_3D::GetNormalizedRightMatchPoints(void){
	try{
		vector<cv::Point2f> Points;
		// normalized points to get better results
		// and invariance to the image origin and scaling
		Points = Normalized_PointsR_SURF;
		return Points;
	}
	catch(...){
	}
}
// get the vertical shifment for the right image respect to the left
float pointer_3D::GetVerticalShifmentSURF(void){
	try{
		int sizeMatches;
		float Shifment = 0;
		float FinalShifment = 0;
		cv::Point2f pointL,pointR;
		sizeMatches = good_matches.size();
		for (int i = 0;i < sizeMatches;i++){
			pointL = KeyPointsL[good_matches[i].queryIdx].pt;
			pointR = KeyPointsR[good_matches[i].trainIdx].pt;
			// get the vertical shifment
			Shifment = pointL.y - pointR.y;
			FinalShifment = FinalShifment + Shifment;
		}
		FinalShifment = abs(FinalShifment)/sizeMatches;
		return FinalShifment;
	}
	catch(...){
		return 0;
	}
}
/*
void pointer_3D::FindFundamentalMatrix(void)
{
  //find fundamental matrix
  int F_matrix_count;
  CvMat* pointsL = matchedPointsFound.pointsL;
  CvMat* pointsR = matchedPointsFound.pointsR;
  int limit = pointsL->cols;
  CvScalar dataL,dataR;
  printf("Matching using Template Matching: \n", limit);
  for(int j=0;j<limit;j++){
	  dataL = cvGet2D(pointsL,0,j);
	  dataR = cvGet2D(pointsR,0,j);
	  printf("match %i\t L(%f,%f) R(%f,%f)\n",j,dataL.val[0],dataL.val[1],
												dataR.val[0],dataR.val[1]);
  }
  //cv::Mat pointsL2 = cv::cvarrToMat(pointsL,true);
  //cv::Mat pointsR2 = cv::cvarrToMat(pointsR,true);
  //cv::Mat OutputArray;
  //cv::Mat Matrix_F(3,3,CV_32FC1);
  //Matrix_F = cv::findFundamentalMat(cv::Mat(pointsL2),cv::Mat(pointsR2),OutputArray,cv::FM_RANSAC,1,0.99); 

  // find fundamental matrix using Hartley 8 points algorithm
  F_matrix_count =  cvFindFundamentalMat(pointsL,pointsR,F_matrix,CV_FM_RANSAC,1.0,0.99);
  // print F
  printf("\n RANSAC C API\n\n");
  for (int k = 0;k<3;k++){
    printf("%4.4E %4.4E %4.4E\n",cvmGet(F_matrix,k,0),cvmGet(F_matrix,k,1),cvmGet(F_matrix,k,2));
  }  
  double detValue = floor(cvDet(F_matrix));
  printf("Normal TemplateMatching Det: %f\n\n",abs(detValue));

}*/

// normalize points according to advises from Multiple view Geometry book
// section 4.4 and web resources
vector<cv::Point2f> pointer_3D::NormalizePointsSURF(vector<cv::Point2f> OriginalPoints, cv::Mat &T){
	try{
		vector<cv::Point2f> NormalizedPoints;
		//cv::Mat T(3,3,CV_32FC1,cv::Scalar::all(0));
		cv::Point2f normalizedPoint;
		float Scale = 1;
		float averageDistance = 0;
		float Xmean = 0;float Ymean = 0;
		float Xbiased,Ybiased;
		// Calculate the centroid for all points at the left and right image
		int Count = OriginalPoints.size();
		for(int i=0;i<Count;i++){
			Xmean = OriginalPoints.at(i).x + Xmean;
			Ymean = OriginalPoints.at(i).y + Ymean;			
		}
		Xmean = Xmean/Count;
		Ymean = Ymean/Count;
		// compute the average distance for all points
		for(int i=0;i<Count;i++){
			Xbiased = OriginalPoints.at(i).x - Xmean;
			Ybiased = OriginalPoints.at(i).y - Ymean;
			averageDistance += sqrt(Xbiased*Xbiased + Ybiased*Ybiased);
		}
		// set Scale
		Scale = sqrt(2.0)*Count/averageDistance;
		// fill the Transform T
		// diagonal
		T = T*0;
		T.ptr<double>(0)[0] = Scale;
		T.ptr<double>(1)[1] = Scale;
		T.ptr<double>(2)[2] = 1.0;
		// traslation
		T.ptr<double>(0)[2] = -Scale*Xmean;
		T.ptr<double>(1)[2] = -Scale*Ymean;
		// Now apply the transform to the points
		for (int i=0;i < Count; i++){
			normalizedPoint.x = Scale*(OriginalPoints.at(i).x - Xmean);
			normalizedPoint.y = Scale*(OriginalPoints.at(i).y - Ymean);
			//normalizedPoint.z = 1;
			NormalizedPoints.push_back(normalizedPoint);
		}
		return NormalizedPoints;

	}
	catch(...)
	{
	}
}
void pointer_3D::FindFundamentalMatrixSURF(void){
	try{

		// get the found matched points in the left and right images
		GetMatchPointsSURF();
		// normalized points to get better results
		// and invariance to the image origin and scaling
		cv::Mat T = cv::Mat(3,3,CV_32FC1,cv::Scalar::all(0));
		cv::Mat T_prim = cv::Mat(3,3,CV_32FC1,cv::Scalar::all(0));
		Normalized_PointsL_SURF = NormalizePointsSURF(pointsL_SURF,T);
		Normalized_PointsR_SURF = NormalizePointsSURF(pointsR_SURF,T_prim);

		cv::Mat F_tmp = cv::Mat(3,3,CV_32FC1,cv::Scalar::all(0));

				
		for (int i= 0; i < pointsL_SURF.size();i++){
			printf("match \t %i L(%f,%f) R(%f,%f)\n",i,pointsL_SURF.at(i).x,pointsL_SURF.at(i).y,
													pointsR_SURF.at(i).x,pointsR_SURF.at(i).y);
		//	printf("Normalized match %i L(%f,%f) R(%f,%f)\n",i,Normalized_PointsL_SURF.at(i).x,Normalized_PointsL_SURF.at(i).y,
		//											Normalized_PointsR_SURF.at(i).x,Normalized_PointsR_SURF.at(i).y);
		}
		

		F_matrix_SURF = cv::findFundamentalMat(pointsL_SURF,pointsR_SURF,
					cv::FM_RANSAC,1,0.99);
		double detValue = cv::determinant(F_matrix_SURF);
		
		printf("\n F_matrix RANSAC from SURF\n\n");
		//printf(" F_matrix Error: %f\n",Ferror);
		printf("\n F_matrix determinant from SURF %f\n\n",detValue);
		
		// show results
		for (int i = 0;i <F_matrix_SURF.rows;i++){
			printf(" RANSAC :: %4.4E; %4.4E; %4.4E \n",F_matrix_SURF.ptr<double>(i)[0],
				   F_matrix_SURF.ptr<double>(i)[1],F_matrix_SURF.ptr<double>(i)[2]);		
		}
		// check matrix results for good fundamental matrix values
		// x't*F*x = 0
		double Ferror = CheckFundamentalMatrix(pointsL_SURF,pointsR_SURF,F_matrix_SURF);
		// print matrix F
		std::cout<<"F_matrix RANSAC from SURF"<<endl<< F_matrix_SURF<<endl<<endl;
/*		std::cout<<"T matrix"<<endl<<T<<endl<<endl;
		std::cout<<"T' matrix"<<endl<<T_prim<<endl<<endl;
		std::cout<<"T' inverted matrix"<<endl<<T_prim.inv()<<endl<<endl;
		// Denormalize F_matrix
		cv::Mat T1 = cv::Mat(3,3,F_matrix_SURF.type());
		cv::Mat T1_prim = cv::Mat(3,3,F_matrix_SURF.type());
		T.convertTo(T1,F_matrix_SURF.type());
		T_prim.convertTo(T1_prim,F_matrix_SURF.type());
		std::cout<<"T1 matrix"<<endl<<T1<<endl<<endl;
		std::cout<<"T1' matrix"<<endl<<T1_prim<<endl<<endl;
		F_tmp = T1_prim.inv()*F_matrix_SURF*T1;//T_prim.inv(cv::DECOMP_SVD)*
		F_matrix_SURF = F_tmp;
		detValue = cv::determinant(F_matrix_SURF);
		// print denormalized matrix F
		std::cout<<"F_matrix RANSAC from SURF"<<endl<< F_matrix_SURF<<endl<<endl;
		printf("\n F_matrix denormalized determinant from SURF %f\n\n",detValue);*/
	}
	catch(...)
	{
	}
}


/*
CvMat* pointer_3D::GetFundamentalMatrix(void){
  // get the Fundamental Matrix
  CvMat *F;
  F = F_matrix;
  return F;

}*/

// check validity of matrix F through X'T*F*X = 0
// points of right x' must lie on the corresponding lines Fx
float pointer_3D::CheckFundamentalMatrix(vector<cv::Point2f> LeftPoints, vector<cv::Point2f> RightPoints, cv::Mat F_matrix){
	try{
		// convert points to Homogeneus
		double Error = 0;
		double tmpError = 0;
		double Epsilon = 0.01;
		double distance_toFx = 0;
		double a,b,c;
		int correctCount = 0;
		cv::Mat F_Error = cv::Mat(1,1,CV_32FC1,cv::Scalar::all(0));
		cv::Mat tmpMat = cv::Mat(3,3,CV_32FC1,cv::Scalar::all(0));
		cv::Mat tmpMat2 = cv::Mat(3,3,CV_32FC1,cv::Scalar::all(0));
		cv::Mat x = cv::Mat(3,3,F_matrix.type(),cv::Scalar::all(0));
		cv::Mat x_prim = cv::Mat(3,3,F_matrix.type(),cv::Scalar::all(0));
		int size = LeftPoints.size();
		cv::Point3f leftPoint,rightPoint;
		vector<cv::Point3f> HomogeneousLeftPoints;
		vector<cv::Point3f> HomogeneousRightPoints;
		cv::convertPointsToHomogeneous(LeftPoints,HomogeneousLeftPoints);
		cv::convertPointsToHomogeneous(RightPoints,HomogeneousRightPoints);
		
		for (int i=0; i <size; i++){
			rightPoint = HomogeneousRightPoints.at(i);
			leftPoint = HomogeneousLeftPoints.at(i);
			x.ptr<double>(0)[0] = leftPoint.x;x_prim.ptr<double>(0)[0] = rightPoint.x;
			x.ptr<double>(1)[0] = leftPoint.y;x_prim.ptr<double>(1)[0] = rightPoint.y;
			x.ptr<double>(2)[0] = leftPoint.z;x_prim.ptr<double>(2)[0] = rightPoint.z;
					
			tmpMat = F_matrix*x;
			a = tmpMat.ptr<double>(0)[0];
			b = tmpMat.ptr<double>(1)[0];
			c = tmpMat.ptr<double>(2)[0]; 
			F_Error = x_prim.t()*F_matrix*x;			
			Error = F_Error.ptr<double>(0)[0] + Error;
			tmpError = F_Error.ptr<double>(0)[0];
			//if (fabs(tmpError) < Epsilon){
				correctCount = correctCount + 1;
				distance_toFx = abs(a*rightPoint.x + b*rightPoint.y + c)/sqrt(pow(a,2)+pow(b,2));
			//	std::cout<<"x't :"<<endl<<x_prim<<endl;
				//std::cout<<"x't:"<<endl<<x_prim<<endl;
			//	std::cout<<"F*x :"<<endl<<tmpMat<<endl;
			//	std::cout<<"x't*F*x :"<<endl<<F_Error<<endl;
			//	printf("match %i error: %E\n",i,F_Error.ptr<double>(0)[0]);				
			//	printf("match %i distance: %E\n",i,distance_toFx);
			//}
		}
		// printf error
		Error = Error/size; // Ideal error = 0 max = # of matches = 1 pixel per match
		
		std::cout<<"x't*F*x error :"<<endl<<Error<<endl;
		std::cout<<"correct Matches :"<<endl<<correctCount<<endl;
		return Error;

	}
	catch(...){
	}
}

// get F from SURF points
cv::Mat pointer_3D::GetFundamentalMatrixSURF(void){
  // get the Fundamental Matrix
	try{
		cv::Mat F;
		F = F_matrix_SURF;
		return F;
	}
	catch(...){
	}
}

/*
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

*/
void pointer_3D::FindEpipolesSURF(void)
{
  // Use SVD (Singular Value Decomposition) to find the epipoles e' and e
  // Find the Epipoles using F*e = 0 and e'transpose*F = 0
  // so Null(F) finds e and Null(Ftranspose) finds e'
  // see matrix analysis book (from carl meyer)
  // titled : Matrix Analysis and Applied Linear Algebra
  
  cv::Mat e(1,3,CV_32FC1);
  cv::Mat e_prim(3,1,CV_32FC1);
  cv::Mat U(3,3,CV_32FC1);
  cv::Mat W(3,3,CV_32FC1);
  cv::Mat V(3,3,CV_32FC1);
  cv::Mat V_Transpose(3,3,CV_32FC1);

  cv::SVD::compute(F_matrix_SURF,W,U,V_Transpose,cv::SVD::FULL_UV);
  
							        // get epipole e from Vtranspose, so by that
  V_Transpose.row(2).copyTo(e);		// we take the last column that is the last  row of Vtranspos.
									// that is the kernel(F)
  U.col(2).copyTo(e_prim);			// get epipole e'
  
  // print U
  printf("\n U matrix\n\n");

  for (int k = 0;k<3;k++){

	  printf("%E %E %E\n",U.ptr<double>(k)[0],U.ptr<double>(k)[1],U.ptr<double>(k)[2]);
  }

  // print W
  printf("\n W matrix\n\n");

  for (int k = 0;k<3;k++){

    printf("%E %E %E\n",W.ptr<double>(k)[0],W.ptr<double>(k)[1],W.ptr<double>(k)[2]);
  }
  // print V
  printf("\n V matrix\n\n");

  for (int k = 0;k<3;k++){

    printf("%E %E %E\n",V_Transpose.ptr<double>(k)[0],V_Transpose.ptr<double>(k)[1],
		V_Transpose.ptr<double>(k)[2]);
  }
  // print e 1 row
  printf("\n Epipole e:\n\n");
  printf("%E %E %E\n",e.ptr<double>(0)[0],e.ptr<double>(0)[1],e.ptr<double>(0)[2]);

  // print e' 1 column
  printf("\n Epipole e':\n\n");
  printf("%E %E %E\n",e_prim.ptr<double>(0)[0],e_prim.ptr<double>(1)[0],e_prim.ptr<double>(2)[0]);

  CamEpipoles.e = e;
  CamEpipoles.e_prim = e_prim;

}
// recovers the epipoles found to the actual geometry
cv::Mat pointer_3D::Get_e_epipole(void){
	try{
		cv::Mat epipole;
		epipole = CamEpipoles.e.clone();
		return epipole;
	}
	catch(...){
	}
}

cv::Mat pointer_3D::Get_e_prim_epipole(void){
	try{
		cv::Mat epipole;
		epipole = CamEpipoles.e_prim.clone();
		return epipole;
	}
	catch(...){
	}
}
void pointer_3D::FindEpipolarLines(void){
	try{
	  // Find the correspond epipolar lines
	  // it is good to check if the matrix F is correct
		int pointsNumber = pointsL_SURF.size();
		vector<cv::Point2f> pointsL = pointsL_SURF;
		vector<cv::Point2f> pointsR = pointsR_SURF;
	 
		cv::Mat EpilinesL = cv::Mat(3,pointsNumber,CV_32FC1);
		cv::Mat EpilinesR = cv::Mat(3,pointsNumber,CV_32FC1);

		// compute epipolar lines in the images points in one image correspond to lines in the second image
		// as described in the epipolar geometry
		cv::computeCorrespondEpilines(pointsR,2,F_matrix_SURF,EpilinesL);
		cv::computeCorrespondEpilines(pointsL,1,F_matrix_SURF,EpilinesR);
		StereoLines.EpilineL = EpilinesL;
		StereoLines.EpilineR = EpilinesR;
	}
	catch(...){
	}

}

// Draw the current epipolar Lines
void pointer_3D::DrawEpipolarLines(cv::Mat &Image, cv::Mat F_matrix, vector<cv::Point2f> points,int pointsFrom){
	try{
		int pointsNumber = points.size();
		double a,b,c,a1,b1,c1;
		float yLmin,yLmax,Limit;
		vector<cv::Point2f> p;
		cv::Point2f currentPoint;
		CvPoint2D32f ImageIntersections[4];
		cv::Mat point = cv::Mat(3,1,F_matrix.type(),cv::Scalar::all(1));
		cv::Mat currentLine = cv::Mat(3,1,F_matrix.type(),cv::Scalar::all(0));

		vector<cv::Point3f> Epilines;// = cv::Mat(3,pointsNumber,CV_32FC1);
		// compute epipolar lines in the images points in one image correspond to lines in the second image
		// as described in the epipolar geometry
		cv::computeCorrespondEpilines(points,pointsFrom,F_matrix,Epilines);
		for (int i=0;i<pointsNumber;i++){
			// epipolar lines
			// lines parameters ax + by + c =0
			
			//point.ptr<double>(0)[0] = points.at(i).x;
			//point.ptr<double>(1)[0] = points.at(i).y;
			//currentLine = F_matrix*point;
			//a1 = currentLine.ptr<double>(0)[0];
			//b1 = currentLine.ptr<double>(1)[0];
			//c1 = currentLine.ptr<double>(2)[0];
			
			a = Epilines.at(i).x;
			b = Epilines.at(i).y;
			c = Epilines.at(i).z;
			// draw epipolar lines
			// on limits  x1 = 0, x2 = width, y3 = 0, y4 = heigh
			// and its intersections on the image rectangle
			yLmin =0 ; yLmax = Image.cols + Image.rows;
			// right
			ImageIntersections[0].x = 0;ImageIntersections[0].y = -c/b;//left side
			ImageIntersections[1].x = Image.cols - 1;ImageIntersections[1].y = (-a*ImageIntersections[1].x)/b -c/b;//right side
			ImageIntersections[2].x = -c/a;ImageIntersections[2].y = 0;//top side
			ImageIntersections[3].x = (-b*(Image.rows-1))/a - c/a;ImageIntersections[3].y = Image.rows - 1;// bottom side

			for (int k =0;k<4;k++){
				Limit = ImageIntersections[k].x + ImageIntersections[k].y;
				// find line points
				if ((Limit >= yLmin)&(Limit <= yLmax)){
						currentPoint.x = ImageIntersections[k].x;
						currentPoint.y = ImageIntersections[k].y;
						p.push_back(currentPoint);
				}
			}
			// draw lines on the right image
			if (p.size()>=2){
				//cvPutText(rightImageBGR_Aligned,matchNumber.data(),pR.at(0),&font,CV_RGB(255,255,255));
				cv::line(Image,p.at(0),p.at(1),CV_RGB(255,255,0),1);
				p.clear();
				//cvLine(rightImageBGR_Aligned,pR.at(0),pR.at(1),CV_RGB(255,255,0),1);	 
			}
		}


	}
	catch(...){
	}
}
EpipolarLines pointer_3D::GetEpipolarLines(void )
{
  EpipolarLines lines;
  lines = StereoLines;
  return lines;
}
/*
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

*/
/*
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

}*/

void pointer_3D::SetSelectedPointLeft(CvPoint2D32f leftPoint)
{
  point_uLeft = leftPoint;
}
void pointer_3D::SetSelectedPointRight(CvPoint2D32f rightPoint)
{
  point_uRight = rightPoint;
}

/*
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
    

      break;

  }


}
*/

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

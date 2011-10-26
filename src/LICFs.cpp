#include "LICFs.h"
// constructors

LICFs::LICFs(void){

}

LICFs::LICFs(IplImage *image)
{
	// Do a copy of the image to be analyzed
	lines = 0;
	HoughStorage = cvCreateMemStorage(0);
	imageOriginal = cvCloneImage(image);
	imgType = LEFT;
	LICFs_matchCounter = 0;
	EpipolarErrorValue = 100;
	 // matching
	minVal = 0;
	maxVal = 0;
	I_height = this->SubImageSize.height;
	I_width = this->SubImageSize.width;
	pt = cvPoint( I_height/2, I_width/2 );
	minLoc = &pt;
	maxLoc = &pt;
	// epipolar constraints
	EpilinesL = cvCreateMat(3,3,CV_32FC1);
	EpilinesR = cvCreateMat(3,3,CV_32FC1);
	cvSetIdentity(EpilinesL);
	cvSetIdentity(EpilinesR);
	

}

LICFs::~LICFs(void)
{
	// release memory
	
	cvReleaseImage(&imageOriginal);
	cvReleaseImage(&SubImage);		
	cvReleaseImage(&SubImageGray);	
	cvReleaseImage(&HoughSubImage);	
	cvReleaseImage(&EdgeSubImage);
	//cvReleaseImage(&LICF_feature);
	//cvReleaseImage(&LICF_featureOtherImage);
	cvReleaseMemStorage(&HoughStorage);
	//cvReleaseMat(&matchOnImageResults);
	//cvReleaseImage(&grayImageToMatch);
}

// Get a subImage from an Image to be analyzed
void LICFs::GetSubImage(imagePoints actualImages_Points, float percentage, IMAGE_TYPE imageType){
	
	try{
		// the value chosen was percentage % of image size
		imgType = imageType;
		CvPoint UpperLeft,LowerRight;
		if (imgType == LEFT){
			UpperLeft = cvPoint((actualImages_Points.xiL - abs((percentage*imageOriginal->width/2))),actualImages_Points.yiL - abs((percentage*imageOriginal->height/2)));
			LowerRight = cvPoint((actualImages_Points.xiL + abs((percentage*imageOriginal->width/2))),actualImages_Points.yiL + abs((percentage*imageOriginal->height/2)));
			// set center of the area to analyze
			SubImageCenter.x = actualImages_Points.xiL;
			SubImageCenter.y = actualImages_Points.yiL;
		}
		else{
			UpperLeft = cvPoint((actualImages_Points.xiR - abs((percentage*imageOriginal->width/2))),actualImages_Points.yiR - abs((percentage*imageOriginal->height/2)));
			LowerRight = cvPoint((actualImages_Points.xiR + abs((percentage*imageOriginal->width/2))),actualImages_Points.yiR + abs((percentage*imageOriginal->height/2)));
			// set center of the area to analyze
			SubImageCenter.x = actualImages_Points.xiR;
			SubImageCenter.y = actualImages_Points.yiR;
		}
		// Rectangle size for cropping the image
		imgSize = cvGetSize(imageOriginal);
		SubImageSize = cvSize((LowerRight.x - UpperLeft.x),(LowerRight.y - UpperLeft.y));
		SubImage = cvCreateImage(SubImageSize,IPL_DEPTH_8U,3);	
		SubImageGray = cvCreateImage(SubImageSize,IPL_DEPTH_8U,1);
		HoughSubImage = cvCreateImage(SubImageSize,IPL_DEPTH_8U,3);	
	    EdgeSubImage = cvCreateImage(SubImageSize,IPL_DEPTH_8U,1);
		// get subImage
		cvGetRectSubPix(imageOriginal,SubImage,SubImageCenter);
		cvCvtColor(SubImage,SubImageGray,CV_BGR2GRAY);
	}
	catch(...){
	}
}

// Get the characteristics of the chosen subarea
SubArea_Structure LICFs::GetSubAreaBoundaries(void){
	
	try{
		// save size of area
		SubAreaLimits.heigh = SubImageSize.height;
		SubAreaLimits.width = SubImageSize.width;
		// save upper left corner
		SubAreaLimits.x_AreaCenter = SubImageCenter.x;
		SubAreaLimits.y_AreaCenter = SubImageCenter.y;
	
		return SubAreaLimits;
	}
	catch(...){
	}

}

// Get the Edges from the image
void LICFs::ApplyCannyEdgeDetector(double CannyWindowSize, double thresholdCannyLow, double thresholdCannyHigh){
	try{
		
		cvCanny(SubImageGray,EdgeSubImage,thresholdCannyLow,thresholdCannyHigh,CannyWindowSize);
		// Show image
		if (imgType == LEFT){
			cvNamedWindow("Edge subImage L");
			cvShowImage("Edge subImage L",EdgeSubImage);
		}else{
			cvNamedWindow("Edge subImage R");
			cvShowImage("Edge subImage R",EdgeSubImage);
		}
		
	}
	catch(...){
	}
}
// Get the lines from the image
CvSeq* LICFs::ApplyHoughLineDetection(int HoughThreshold, double HoughMinLengthDetection, double HoughMaxGapBetweenLines){
	try{
		
		lines = cvHoughLines2(EdgeSubImage,HoughStorage,CV_HOUGH_PROBABILISTIC,1,CV_PI/180,
			HoughThreshold,HoughMaxGapBetweenLines,HoughMaxGapBetweenLines);
		cvCvtColor(EdgeSubImage, HoughSubImage, CV_GRAY2BGR );

		for (int i=0;i<lines->total;i++){
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			cvLine(HoughSubImage,line[0],line[1],CV_RGB(255,0,0));
		}
		if (imgType == LEFT){
			cvNamedWindow("Hough subImage L");
			cvShowImage("Hough subImage L",HoughSubImage);
		}else{
			cvNamedWindow("Hough subImage R");
			cvShowImage("Hough subImage R",HoughSubImage);
		}
		return lines;
	}
	catch(...){
	}
}

// get the intersection point between two lines
lineIntersection LICFs::GetLineIntersection(lineParameters Line1, lineParameters Line2){
	try{
		lineIntersection crossPoint;
		// using line intersection method by Paul Borke's web page
		float U_a,U_b = 1;		
		float numerator_U_a = 1, numerator_U_b = 1;
		float denominator_U_a = 1, denominator_U_b = 1;
		float xi=1,yi=1;
		 // calculate intersection check 						 
		 numerator_U_a = (Line2.x2 - Line2.x1)*(Line2.y1 - Line1.y1) -
			 (Line2.x1 - Line1.x1)*(Line2.y2 - Line2.y1);
		 denominator_U_a = (Line2.x2 - Line2.x1)*(Line1.y2 - Line1.y1) -
			 (Line2.y2 - Line2.y1)*(Line1.x2 - Line1.x1);
		 numerator_U_b = (Line2.y1 - Line1.y1)*(Line1.x2 - Line1.x1) -
			 (Line2.x1 - Line1.x1)*(Line1.y2 - Line1.y1);
		 denominator_U_b = denominator_U_a;
		 U_a = numerator_U_a/denominator_U_a;
		 U_b = numerator_U_b/denominator_U_b;
		 // check line status: Parallel, Coincident, not intersecting, intersecting

		 // intersecting case
		 if ((denominator_U_a != 0.0f) & (numerator_U_a != 0.0f) & (numerator_U_b != 0.0f)){

			// intersecting point between line segments
			 //if (((U_a >= 0.0f) & (U_a <= 1.0f) & (U_b >= 0.0f) & (U_b <= 1.0f))){
				
				yi = (Line1.y1 + U_a*(Line1.y2 - Line1.y1));
				xi = (Line1.x1 + U_a*(Line1.x2 - Line1.x1));
			//}
		 }
		 crossPoint.xi = xi;
		 crossPoint.yi = yi;
		 return crossPoint;

	}
	catch(...){
	}


}


// get the LICFs of the image
vector<LICFs_Structure> LICFs::ApplyLICF_Detection(CvSeq *imageHoughLines, int LICF_MaxDistanceBetweenLines){
	try{
		// Detect LICFs features		
		// minimum intersection distance in pixels
		float dth_threshold = LICF_MaxDistanceBetweenLines;	
		lineParameters currentLine;
		LICFs_Structure currentLICF,tmp_currentLICF;
		
		vector<lineParameters> Actual_Lines;	

		float d_betweenLines1 = 10;
		float d_betweenLines2 = 10;
		float d_betweenLines3 = 10;
		float d_betweenLines4 = 10;
		float d_Min_betweenLines,d_Min1,d_Min2 = 10;
		lineParameters currentLine1;
		lineParameters currentLine2;
		float thetaAngle_L1 = 0, thetaAngle_L2 = 0;

		for( int i = 0; i < imageHoughLines->total; i++ )
        {
            CvPoint* line = (CvPoint*)cvGetSeqElem(imageHoughLines,i);
          
			// save line parameters
			currentLine.x1 = line[1].x;
			currentLine.x2 = line[0].x;
			currentLine.y1 = line[1].y;
			currentLine.y2 = line[0].y;
			Actual_Lines.push_back(currentLine);
		
        }
		// Find LICFs intersections		
		// using line intersection method by Paul Borke's web page
		float U_a,U_b = 1;		
		float numerator_U_a = 1, numerator_U_b = 1;
		float denominator_U_a = 1, denominator_U_b = 1;
		float xi,yi;
		float d1_L1,d2_L1,d1_L2,d2_L2;

		for(int i=0; i < Actual_Lines.size(); i++){
			for(int j=0; j < Actual_Lines.size(); j++){
				// check that the checking lines are not the same line
				// at the vector structure
				if(i != j){
					currentLine1 = Actual_Lines.at(i);
					currentLine2 = Actual_Lines.at(j);
					// check the distance among the 4 line end points of the 2 lines
					// using distance in 2D   
					d_betweenLines1 = sqrt(pow((currentLine1.x1 - currentLine2.x1),2)+ pow((currentLine1.y1 - currentLine2.y1),2));
					d_betweenLines2 = sqrt(pow((currentLine1.x1 - currentLine2.x2),2)+ pow((currentLine1.y1 - currentLine2.y2),2));
					d_betweenLines3 = sqrt(pow((currentLine1.x2 - currentLine2.x1),2)+ pow((currentLine1.y2 - currentLine2.y1),2));
					d_betweenLines4 = sqrt(pow((currentLine1.x2 - currentLine2.x2),2)+ pow((currentLine1.y2 - currentLine2.y2),2));
					// check min value
					d_Min1 = min(d_betweenLines1, d_betweenLines2);
					d_Min2 = min(d_betweenLines3, d_betweenLines4);
					d_Min_betweenLines = min(d_Min1,d_Min2);

					if (d_Min_betweenLines < dth_threshold ){
					 // calculate intersection check 						 
					 numerator_U_a = (currentLine2.x2 - currentLine2.x1)*(currentLine2.y1 - currentLine1.y1) -
						 (currentLine2.x1 - currentLine1.x1)*(currentLine2.y2 - currentLine2.y1);
					 denominator_U_a = (currentLine2.x2 - currentLine2.x1)*(currentLine1.y2 - currentLine1.y1) -
						 (currentLine2.y2 - currentLine2.y1)*(currentLine1.x2 - currentLine1.x1);
					 numerator_U_b = (currentLine2.y1 - currentLine1.y1)*(currentLine1.x2 - currentLine1.x1) -
						 (currentLine2.x1 - currentLine1.x1)*(currentLine1.y2 - currentLine1.y1);
					 denominator_U_b = denominator_U_a;
					 U_a = numerator_U_a/denominator_U_a;
					 U_b = numerator_U_b/denominator_U_b;

					 // check line status: Parallel, Coincident, not intersecting, intersecting

					 // intersecting case
					 if ((denominator_U_a != 0.0f) & (numerator_U_a != 0.0f) & (numerator_U_b != 0.0f)){

						// intersecting point between line segments
						 if (((U_a >= 0.0f) & (U_a <= 1.0f) & (U_b >= 0.0f) & (U_b <= 1.0f))){
							
							yi = (currentLine1.y1 + U_a*(currentLine1.y2 - currentLine1.y1));
							xi = (currentLine1.x1 + U_a*(currentLine1.x2 - currentLine1.x1));
							// calculate the inclination of the lines
							thetaAngle_L1 = atan((currentLine1.y2 - currentLine1.y1)/(currentLine1.x2 - currentLine1.x1))*180/CV_PI;
							thetaAngle_L2 = atan((currentLine2.y2 - currentLine2.y1)/(currentLine2.x2 - currentLine2.x1))*180/CV_PI;
							currentLine1.thetaAngle = thetaAngle_L1;
							currentLine2.thetaAngle = thetaAngle_L2;
							// calculate the farthest end line points for each LICF line
							// for L1
							d1_L1 = sqrt(pow((xi-currentLine1.x1),2) + pow((yi-currentLine1.y1),2));
							d2_L1 = sqrt(pow((xi-currentLine1.x2),2) + pow((yi-currentLine1.y2),2));
							if (d1_L1 > d2_L1){
								currentLine1.x_farthest = currentLine1.x1;
								currentLine1.y_farthest = currentLine1.y1;
							}else{
								currentLine1.x_farthest = currentLine1.x2;
								currentLine1.y_farthest = currentLine1.y2;
							}
							// for L2
							d1_L2 = sqrt(pow((xi-currentLine2.x1),2) + pow((yi-currentLine2.y1),2));
							d2_L2 = sqrt(pow((xi-currentLine2.x2),2) + pow((yi-currentLine2.y2),2));
							if (d1_L2 > d2_L2){
								currentLine2.x_farthest = currentLine2.x1;
								currentLine2.y_farthest = currentLine2.y1;
							}else{
								currentLine2.x_farthest = currentLine2.x2;
								currentLine2.y_farthest = currentLine2.y2;
							}
							// save LICF
							currentLICF.x_xK = xi;
							currentLICF.y_xK = yi;
							currentLICF.L_1 = currentLine1;
							currentLICF.L_2 = currentLine2;
							// save found LICF
							Actual_LICFs.push_back(currentLICF);
						
						 }
					 }						 
		
					}
					// show last found LICF
				/*	if (Actual_LICFs.size() != 0){
						tmp_currentLICF = Actual_LICFs.back();
						cvLine(LICF_LeftSubImage, cvPoint(tmp_currentLICF.L_1.x1,tmp_currentLICF.L_1.y1), cvPoint(tmp_currentLICF.L_1.x2,tmp_currentLICF.L_1.y2), CV_RGB(0,128,255), 1, 8 );
						cvLine(LICF_LeftSubImage, cvPoint(tmp_currentLICF.L_2.x1,tmp_currentLICF.L_2.y1), cvPoint(tmp_currentLICF.L_1.x2,tmp_currentLICF.L_1.y2), CV_RGB(0,255,128), 1, 8 );
						cvCircle(LICF_LeftSubImage,cvPoint(xi,yi),2,CV_RGB(0,0,255),1,3,0);
						
					}*/

				}
			}
			
		}
		return Actual_LICFs;

	}
	catch(...){
	}
}
// Get current subImageGray
IplImage* LICFs::GetSubImageGray(void){
	try{
		grayImageToMatch = cvCloneImage(SubImageGray);
		return grayImageToMatch;
	}
	catch(...){
	}
}
// Get Normalized cross correlation between two images
double LICFs::GetLICFs_NCC(CvMat *LICF_feature,CvMat *LICF_featureOtherImage){
	try{
		double correlationValue = 0;
		double sumNum = 0,tmpVal = 0,tmpVal2 = 0;
		double sumDen = 0,sumDen1 = 0, sumDen2 = 0;
		CvScalar I_Avg = cvAvg(LICF_feature,NULL);
		int size = LICF_feature->width;
		
		CvMat *L_subImage = cvCloneMat(LICF_feature);
		CvMat *R_subImage = cvCloneMat(LICF_featureOtherImage);
		// get NCC value
		double center,wSize = size/2;
		double centerFrac = modf(wSize,&center);
		CvPoint testPoint = cvPoint((int)center,(int)center);
		for (int i=0;i<size;i++){
			for (int j=0;j<size;j++){
				//
				tmpVal = cvmGet(L_subImage,testPoint.y,testPoint.x);
				tmpVal2 = cvmGet(R_subImage,i,j);
				sumNum = sumNum + tmpVal*tmpVal2;
				sumDen1 = sumDen1 + pow(tmpVal,2);
				sumDen2 = sumDen2 + pow(tmpVal2,2);
			}
		}
		// get final value of NCC for these images
		sumDen = sumDen1*sumDen2;
		correlationValue = sumNum/sqrt(sumDen);
		cvReleaseMat(&L_subImage);
		cvReleaseMat(&R_subImage);
		return correlationValue;
	}
	catch(...){
	}
}

// Get the current matching points for this image 
vector<Matching_LICFs> LICFs::ApplyMatchingLICFs(IplImage *SubImageToMatch,vector<LICFs_Structure> LICFs_otherImage,float threshold, int Windowsize){
	try{
		// for all the LICFS found
		CvSize LICF_imageSize = cvSize(Windowsize,Windowsize);
		LICF_feature = cvCreateMat(LICF_imageSize.height,LICF_imageSize.width,CV_32FC1);
		LICF_featureOtherImage = cvCreateMat(LICF_imageSize.height,LICF_imageSize.width,CV_32FC1);
		// Create a matrix for the correspond match results
		//int widthLICF = (SubImageGray->width - LICF_feature->width + 1);
		//int heighLICF =  (SubImageGray->height - LICF_feature->height +1);
		//matchOnImageResults = cvCreateMat(heighLICF,widthLICF,CV_32FC1);
		Matching_LICFs currentMatched_LICFs;
		LICFs_matchCounter = 0;
		double minThetaAngle = 1;
		double currentAngleDifference_L1 = 15;
		double currentAngleDifference_L2 = 15;

		// find the number of features to compare and apply NCC
		for (int i=0;i < Actual_LICFs.size();i++){
			for (int j=0;j< LICFs_otherImage.size();j++){
				// get LICF position
				LICF_FeatureCenter.x = Actual_LICFs.at(i).x_xK;
				LICF_FeatureCenter.y = Actual_LICFs.at(i).y_xK;
				// get LICF position of features in the other image
				LICF_FeatureCenterOtherImage.x = LICFs_otherImage.at(j).x_xK;
				LICF_FeatureCenterOtherImage.y = LICFs_otherImage.at(j).y_xK;
				// crop the LICF as an subimage to look for a match in the other image
				cvGetRectSubPix(SubImageGray,LICF_feature,LICF_FeatureCenter);
				cvGetRectSubPix(SubImageToMatch,LICF_featureOtherImage,LICF_FeatureCenterOtherImage);
				//cvMatchTemplate(LICF_featureOtherImage,LICF_feature,matchOnImageResults,CV_TM_CCORR_NORMED);
				// find NCC between the two LICF subimages
				maxVal = GetLICFs_NCC(LICF_feature,LICF_featureOtherImage);
				// find place of matching in the other image
				//cvMinMaxLoc(matchOnImageResults,&minVal,&maxVal,minLoc,maxLoc);
				// save matching points
				if (maxVal > threshold){
					// check line matching
					//currentAngleDifference_L1 = LICFs_otherImage.at(j).L_1.thetaAngle - Actual_LICFs.at(i).L_1.thetaAngle;
					//currentAngleDifference_L2 = LICFs_otherImage.at(j).L_2.thetaAngle - Actual_LICFs.at(i).L_2.thetaAngle;

					//if ((abs(currentAngleDifference_L1) < minThetaAngle)&(abs(currentAngleDifference_L2) < minThetaAngle)){
						LICFs_matchCounter = LICFs_matchCounter + 1;
						// right LICF
						currentMatched_LICFs.MatchLICFs_R.x_xK = LICFs_otherImage.at(j).x_xK;
						currentMatched_LICFs.MatchLICFs_R.y_xK = LICFs_otherImage.at(j).y_xK;
						currentMatched_LICFs.MatchLICFs_R.L_1 = LICFs_otherImage.at(j).L_1;
						currentMatched_LICFs.MatchLICFs_R.L_2 = LICFs_otherImage.at(j).L_2;
						// left LICF
						currentMatched_LICFs.MatchLICFs_L.x_xK = Actual_LICFs.at(i).x_xK;
						currentMatched_LICFs.MatchLICFs_L.y_xK = Actual_LICFs.at(i).y_xK;
						currentMatched_LICFs.MatchLICFs_L.L_1 = Actual_LICFs.at(i).L_1;
						currentMatched_LICFs.MatchLICFs_L.L_2 = Actual_LICFs.at(i).L_2;

						// save on vector
						Actual_Matched_LICFs.push_back(currentMatched_LICFs);
					//}
				}
			}// end for j
		}
		return Actual_Matched_LICFs;
	}
	catch(...){
	}
}

// Get the epipolar constraint value
// this is an indicator of the level of coplanarity of the 
// intersection LICFs found
LICFs_EpipolarConstraintResult LICFs::GetEpipolarConstraintError(CvMat* F_matrix,SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR){
	try{
		// variables
		LICFs_EpipolarConstraintResult finalResults;
		float ConstraintError = 0;
		float tmpConstraintError = 0;
		int Ntrans = Actual_Matched_LICFs.size();
		int Xo_L,Yo_L,Xo_R,Yo_R;
		CvPoint upperLeft_L,upperLeft_R;
		// origin points of the subimages respect to the original images
		
		upperLeft_L = cvPoint(SubAreaImageL.x_AreaCenter - abs(0.5*SubAreaImageL.width),
			SubAreaImageL.y_AreaCenter - abs(0.5*SubAreaImageL.heigh));
		upperLeft_R = cvPoint(SubAreaImageR.x_AreaCenter - abs(0.5*SubAreaImageR.width),
			SubAreaImageR.y_AreaCenter - abs(0.5*SubAreaImageR.heigh));
		Xo_L = upperLeft_L.x;
		Yo_L = upperLeft_L.y;
		Xo_R = upperLeft_R.x;
		Yo_R = upperLeft_R.y;

		// check number of matches
		// minimum are 3 to get the epipolar lines
		if (Ntrans >= 3){
			EpilinesL = cvCreateMat(3,Ntrans,CV_32FC1);
			EpilinesR = cvCreateMat(3,Ntrans,CV_32FC1);
			CvMat *LICFs_pointsL = cvCreateMat(1,Ntrans,CV_32FC2);
			CvMat *LICFs_pointsR = cvCreateMat(1,Ntrans,CV_32FC2);
			// fill the matching points
			CvScalar Value;
			Value.val[2] = 0;
			Value.val[3] = 0;
			for (int i =0;i < Ntrans;i++){

				Value.val[0] = Actual_Matched_LICFs.at(i).MatchLICFs_L.x_xK + Xo_L;
				Value.val[1] = Actual_Matched_LICFs.at(i).MatchLICFs_L.y_xK + Yo_L;
				cvSet2D(LICFs_pointsL,0,i,Value);
				Value.val[0] = Actual_Matched_LICFs.at(i).MatchLICFs_R.x_xK + Xo_R;
				Value.val[1] = Actual_Matched_LICFs.at(i).MatchLICFs_R.y_xK + Yo_R;
				cvSet2D(LICFs_pointsR,0,i,Value);
			}

			// Get epipolar lines for the right image
			cvComputeCorrespondEpilines(LICFs_pointsL,1,F_matrix,EpilinesR);
			// Get epipolar lines for the left image
			cvComputeCorrespondEpilines(LICFs_pointsR,2,F_matrix,EpilinesL);

			// APPLY THE EPIPOLAR CONSTRAINT TO ALL MATCHED POINTS

			// lines parameters ax + by + c =0
			double aL,bL,cL,aR,bR,cR;
			double xoL,yoL,xoR,yoR;
			for (int i=0; i < Ntrans; i++){

				aL = cvmGet(EpilinesL,0,i);aR = cvmGet(EpilinesR,0,i);
				bL = cvmGet(EpilinesL,1,i);bR = cvmGet(EpilinesR,1,i);
				cL = cvmGet(EpilinesL,2,i);cR = cvmGet(EpilinesR,2,i);
				xoL = Actual_Matched_LICFs.at(i).MatchLICFs_L.x_xK + Xo_L;
				xoR = Actual_Matched_LICFs.at(i).MatchLICFs_R.x_xK + Xo_R;
				yoL = Actual_Matched_LICFs.at(i).MatchLICFs_L.y_xK + Yo_L;
				yoR = Actual_Matched_LICFs.at(i).MatchLICFs_R.y_xK + Yo_R;
				// find the error according to the equation:

				tmpConstraintError = pow((abs(aL*xoL + bL*yoL + cL)/sqrt(pow(aL,2) + pow(bL,2))),2) 
					+ pow((abs(aR*xoR + bR*yoR + cR)/sqrt(pow(aR,2)+ pow(bR,2))),2);
				ConstraintError = ConstraintError + tmpConstraintError;
			}
			
			ConstraintError = ConstraintError/Ntrans;
		
		}
		else{// Ntrans <3
				ConstraintError = -1; // not enough matching found
		}

		// emit the corresponding signal
		//emit SendEpipolarErrorConstraint(ConstraintError);
		// return results
		finalResults.errorValue = ConstraintError;
		finalResults.EpilineL = EpilinesL;
		finalResults.EpilineR = EpilinesR;
		return finalResults;
	}
	catch(...)
	{
	}
}
// Get the homography between the left and right images
CvMat* LICFs::FindLICF_BasedHomography(vector<Matching_LICFs> matchedPoints,CvMat* F_matrix, CvMat *epipole, CvMat *epipole_prim,
								  SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR){
	try{
		CvMat *H_matrix = cvCreateMat(3,3,CV_32FC1);
		cvSetIdentity(H_matrix);
		// matrices for saving epilolar lines corresponding to points ex1 and ex2
		// L1' = Fex1 ; L2' = Fex2
		CvMat *L1_prim = cvCreateMat(3,3,CV_32FC1);
		CvMat *L2_prim = cvCreateMat(3,3,CV_32FC1);
		CvMat *ex1 = cvCreateMat(3,3,CV_32FC1);
		CvMat *ex2 = cvCreateMat(3,3,CV_32FC1);

		CvMat *Epilines = cvCreateMat(3,4,CV_32FC1);
		cvSetIdentity(L1_prim);	cvSetIdentity(L2_prim);
		cvSetZero(ex1);	cvSetZero(ex2);
		lineParameters Line1,Line1_prim;
		lineParameters Line2,Line2_prim;
		lineIntersection lineCrossPointLine1,lineCrossPointLine2;
		float a1,b1,c1,a2,b2,c2;
		float aR1,bR1,cR1,aR2,bR2,cR2;
		CvPoint ex1_prim, ex2_prim;
		float dL1,dL2,dL3,dL4;

		int matchSize = matchedPoints.size();
		double Xo_L,Yo_L,Xo_R,Yo_R;
		CvPoint upperLeft_L,upperLeft_R;
		// origin points of the subimages respect to the original images		
		upperLeft_L = cvPoint(SubAreaImageL.x_AreaCenter - abs(0.5*SubAreaImageL.width),
			SubAreaImageL.y_AreaCenter - abs(0.5*SubAreaImageL.heigh));
		upperLeft_R = cvPoint(SubAreaImageR.x_AreaCenter - abs(0.5*SubAreaImageR.width),
			SubAreaImageR.y_AreaCenter - abs(0.5*SubAreaImageR.heigh));
		Xo_L = upperLeft_L.x;
		Yo_L = upperLeft_L.y;
		Xo_R = upperLeft_R.x;
		Yo_R = upperLeft_R.y;

		// we need 4 matches to calculate the homography matrix
		// here we use the limit points from each LICF to get the 4 points
		// so with one LICF match we have enough information to calculate
		// the searched homography
		if (matchSize >=1){
			
			CvMat *LICFs_pointsL = cvCreateMat(1,4,CV_32FC2);
			CvMat *LICFs_pointsR = cvCreateMat(1,4,CV_32FC2);
			CvScalar Value;
			Value.val[2] = 0;
			Value.val[3] = 0;
			for (int i =0;i < 1;i++){

				Value.val[0] = 251;//matchedPoints.at(i).MatchLICFs_L.x_xK + Xo_L;
				Value.val[1] = 209;//matchedPoints.at(i).MatchLICFs_L.y_xK + Yo_L;
				cvSet2D(LICFs_pointsL,0,0,Value);
				cvmSet(ex1,0,0,Value.val[0]);
				cvmSet(ex1,1,0,Value.val[1]);
				
				Value.val[0] = 307;//matchedPoints.at(i).MatchLICFs_L.L_1.x_farthest + Xo_L;
				Value.val[1] = 223;//matchedPoints.at(i).MatchLICFs_L.L_1.y_farthest + Yo_L;
				cvSet2D(LICFs_pointsL,0,1,Value);
				cvmSet(ex1,0,0,Value.val[0]);
				cvmSet(ex1,1,0,Value.val[1]);
				cvmSet(ex1,2,0,1.0f);

				Value.val[0] = 254;//matchedPoints.at(i).MatchLICFs_L.L_2.x_farthest + Xo_L;
				Value.val[1] = 191;//matchedPoints.at(i).MatchLICFs_L.L_2.y_farthest + Yo_L;
				cvSet2D(LICFs_pointsL,0,2,Value);
				cvmSet(ex2,0,0,Value.val[0]);
				cvmSet(ex2,1,0,Value.val[1]);
				cvmSet(ex2,2,0,1.0f);

				// epipole value: e // normalize this values
				Value.val[2] = cvmGet(epipole,0,2);
				Value.val[0] = 311;//cvmGet(epipole,0,0)/Value.val[2];
				Value.val[1] = 170;//cvmGet(epipole,0,1)/Value.val[2];
				Value.val[2] = 0.0f;
				cvSet2D(LICFs_pointsL,0,3,Value);

				//cvmGet(epipole,0,2));			

				// find the corresponding lines on the right image
				cvComputeCorrespondEpilines(LICFs_pointsL,1,F_matrix,Epilines);
				cvMatMul(F_matrix,ex1,L1_prim);
				cvMatMul(F_matrix,ex2,L2_prim);

				// find line intersections that correspond to ex1' and ex2'
				aR1 = cvmGet(Epilines,0,1);aR2 = cvmGet(Epilines,0,2);
				bR1 = cvmGet(Epilines,1,1);bR2 = cvmGet(Epilines,1,2);
				cR1 = cvmGet(Epilines,2,1);cR2 = cvmGet(Epilines,2,2);

				a1 = cvmGet(L1_prim,0,0);a2 = cvmGet(L2_prim,0,0);
				b1 = cvmGet(L1_prim,1,0);b2 = cvmGet(L2_prim,1,0);
				c1 = cvmGet(L1_prim,2,0);c2 = cvmGet(L2_prim,2,0);

				Line1.x1 = matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest + Xo_R;
				Line1.y1 = matchedPoints.at(i).MatchLICFs_R.L_1.y_farthest + Yo_R;
				Line1.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Line1.y2 = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;

				Line1_prim.x1 = matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest + Xo_R;
				Line1_prim.y1 = -(aR1*Line1_prim.x1)/bR1 - cR1/bR1 + Yo_R;
				Line1_prim.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Line1_prim.y2 = -(aR1*Line1_prim.x2)/bR1 - cR1/bR1 + Yo_R;

				Line2.x1 = matchedPoints.at(i).MatchLICFs_R.L_2.x_farthest + Xo_R;
				Line2.y1 = matchedPoints.at(i).MatchLICFs_R.L_2.y_farthest + Yo_R;
				Line2.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Line2.y2 = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;

				Line2_prim.x1 = matchedPoints.at(i).MatchLICFs_R.L_2.x_farthest + Xo_R;
				Line2_prim.y1 = -(aR2*Line2_prim.x1)/bR2 - cR2/bR2 + Yo_R;
				Line2_prim.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Line2_prim.y2 = -(aR2*Line2_prim.x2)/bR2 - cR2/bR2 + Yo_R;

				lineCrossPointLine1 = GetLineIntersection(Line1,Line1_prim);
				lineCrossPointLine2 = GetLineIntersection(Line2,Line2_prim);

				// calculus using distance between l' and ex2' instead of intersections
				// for ex'1
				dL1 = abs(aR1*Line1.x1 + bR1*Line1.y1 + cR1)/sqrt(pow(aR1,2)+ pow(bR1,2));
				dL3 = abs(aR1*lineCrossPointLine1.xi + bR1*lineCrossPointLine1.yi + cR1)/sqrt(pow(aR1,2)+ pow(bR1,2));
				// for ex'2
				dL2 = abs(aR2*Line2.x1 + bR2*Line2.y1 + cR2)/sqrt(pow(aR2,2)+ pow(bR2,2));
				dL4 = abs(aR1*lineCrossPointLine2.xi + bR1*lineCrossPointLine2.yi + cR1)/sqrt(pow(aR1,2)+ pow(bR1,2)); 


				// Corresponding points on the other image

				Value.val[0] = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Value.val[1] = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;
				cvSet2D(LICFs_pointsR,0,0,Value); 

				Value.val[0] = lineCrossPointLine1.xi;//matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest + Xo_R;
				Value.val[1] = lineCrossPointLine1.yi;//matchedPoints.at(i).MatchLICFs_R.L_1.y_farthest + Yo_R;
				cvSet2D(LICFs_pointsR,0,1,Value);

				Value.val[0] = lineCrossPointLine2.xi;//matchedPoints.at(i).MatchLICFs_R.L_2.x_farthest + Xo_R;
				Value.val[1] = lineCrossPointLine2.yi;//matchedPoints.at(i).MatchLICFs_R.L_2.y_farthest + Yo_R;
				cvSet2D(LICFs_pointsR,0,2,Value);
				// epipole_prim value: e'
				Value.val[2] = cvmGet(epipole_prim,2,0);
				Value.val[0] = cvmGet(epipole_prim,0,0)/Value.val[2];
				Value.val[1] = cvmGet(epipole_prim,1,0)/Value.val[2];
				Value.val[2] = 0.0f;
				cvSet2D(LICFs_pointsR,0,3,Value);
				//cvmGet(epipole_prim,0,2));
			}
			// find the homography
			double maxPixelErr = 3;
			cvFindHomography(LICFs_pointsL,LICFs_pointsR,H_matrix,CV_RANSAC,maxPixelErr);

		}
		return H_matrix;
	}
	catch(...){
	}
}

// Get the homography using the method proposed in the chapter 13 (algorithm 13.1) of the book
// multiple view geometry by Hartley and Zisserman
CvMat* LICFs::FindLICF_BasedHomographyZissermman(std::vector<Matching_LICFs> matchedPoints, CvMat *F_matrix, CvMat *epipole, CvMat *epipole_prim, SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR){
	try{
		// Matrices
		CvMat *H = cvCreateMat(3,3,CV_32FC1);
		CvMat *A = cvCreateMat(3,3,CV_32FC1);
		CvMat *M = cvCreateMat(3,3,CV_32FC1);
		CvMat *e_prim_skew_symetric = cvCreateMat(3,3,CV_32FC1);
		CvMat *v = cvCreateMat(3,1,CV_32FC1);
		CvMat *vT = cvCreateMat(1,3,CV_32FC1);
		CvMat *e_prim_vT = cvCreateMat(3,3,CV_32FC1);
		CvMat *b = cvCreateMat(3,1,CV_32FC1);
		CvMat *xi = cvCreateMat(3,3,CV_32FC1);
		CvMat *xi_prim =cvCreateMat(3,3,CV_32FC1);

		int matchSize = matchedPoints.size();
		double Xo_L,Yo_L,Xo_R,Yo_R;
		CvPoint upperLeft_L,upperLeft_R;
		// origin points of the subimages respect to the original images		
		upperLeft_L = cvPoint(SubAreaImageL.x_AreaCenter - abs(0.5*SubAreaImageL.width),
			SubAreaImageL.y_AreaCenter - abs(0.5*SubAreaImageL.heigh));
		upperLeft_R = cvPoint(SubAreaImageR.x_AreaCenter - abs(0.5*SubAreaImageR.width),
			SubAreaImageR.y_AreaCenter - abs(0.5*SubAreaImageR.heigh));
		Xo_L = upperLeft_L.x;
		Yo_L = upperLeft_L.y;
		Xo_R = upperLeft_R.x;
		Yo_R = upperLeft_R.y;

		cvSetIdentity(H);cvSetZero(v);cvSetZero(xi);
		cvSetZero(A);cvSetZero(b);cvSetZero(xi_prim);
		cvSetZero(M);cvSetZero(e_prim_skew_symetric);
		cvSetZero(vT);
		// set [e']x = cross product matrix representation
		double e1,e2,e3;
		e1 = cvmGet(epipole_prim,0,0);
		e2 = cvmGet(epipole_prim,1,0);
		e3 = cvmGet(epipole_prim,2,0);
		cvmSet(e_prim_skew_symetric,0,1,-e3);cvmSet(e_prim_skew_symetric,0,2, e2);		
		cvmSet(e_prim_skew_symetric,1,0, e3);cvmSet(e_prim_skew_symetric,1,2,-e1);		
		cvmSet(e_prim_skew_symetric,2,0,-e2);cvmSet(e_prim_skew_symetric,2,1, e1);
		// set A = [e']xF
		cvMatMul(e_prim_skew_symetric,F_matrix,A);
		// Ask for matching LICFs
		if(matchSize >= 1){ // we need at least 1 LICF feature = 3 matched points xk,L1 and L2 ends

			// set M = xi transpose, which correspond to left image points
			cvmSet(M,0,0,matchedPoints.at(0).MatchLICFs_L.x_xK + Xo_L);
			cvmSet(M,1,0,matchedPoints.at(0).MatchLICFs_L.L_1.x_farthest + Xo_L);
			cvmSet(M,2,0,matchedPoints.at(0).MatchLICFs_L.L_2.x_farthest + Xo_L);

			cvmSet(M,0,1,matchedPoints.at(0).MatchLICFs_L.y_xK + Yo_L);
			cvmSet(M,1,1,matchedPoints.at(0).MatchLICFs_L.L_1.y_farthest + Yo_L);
			cvmSet(M,2,1,matchedPoints.at(0).MatchLICFs_L.L_2.y_farthest + Yo_L);
			// set 3rd value to 1 as homogeneous coordinates, isn't it?
			cvmSet(M,0,2,1);
			cvmSet(M,1,2,1);
			cvmSet(M,2,2,1);

			// set xi' right image matching points vector matrix
			cvmSet(xi_prim,0,0,matchedPoints.at(0).MatchLICFs_R.x_xK + Xo_R);
			cvmSet(xi_prim,1,0,matchedPoints.at(0).MatchLICFs_R.y_xK + Yo_R);
			cvmSet(xi_prim,2,0,1);

			cvmSet(xi_prim,0,1,matchedPoints.at(0).MatchLICFs_R.L_1.x_farthest + Xo_R);
			cvmSet(xi_prim,1,1,matchedPoints.at(0).MatchLICFs_R.L_1.y_farthest + Yo_R);
			cvmSet(xi_prim,2,1,1);

			cvmSet(xi_prim,0,2,matchedPoints.at(0).MatchLICFs_R.L_2.x_farthest + Xo_R);
			cvmSet(xi_prim,1,2,matchedPoints.at(0).MatchLICFs_R.L_2.y_farthest + Yo_R);
			cvmSet(xi_prim,2,2,1);

			// Now set b vector for complete equation knowledgement Mv = b
			CvMat *Axi = cvCreateMat(3,3,CV_32FC1);
			 
			CvMat *vectorProduct1 = cvCreateMat(3,1,CV_32FC1);
			CvMat *vectorProduct2 = cvCreateMat(3,1,CV_32FC1);
			CvMat *transpose_vectorProduct1 = cvCreateMat(1,3,CV_32FC1);
			
			CvMat *vector_xi = cvCreateMat(3,1,CV_32FC1);
			CvMat *vector_Axi = cvCreateMat(3,1,CV_32FC1);
			CvMat *vector_xi_prim = cvCreateMat(3,1,CV_32FC1);
			CvMat *vector_e_prim = cvCreateMat(3,1,CV_32FC1);
			

			double Magnitude_vectorProduct2 = 1;
			double bi = 1;

			cvSetZero(vector_xi);cvSetZero(vector_Axi);
			cvSetZero(vector_xi_prim);cvSetZero(e_prim_vT);
			// e'
			cvmSet(vector_e_prim,0,0,e1);
			cvmSet(vector_e_prim,1,0,e2);
			cvmSet(vector_e_prim,2,0,e3);

			cvTranspose(M,xi);
			cvMatMul(A,xi,Axi);
 
			// get b finally
			for (int i = 0;i<3;i++){
				// x'i
				cvmSet(vector_xi_prim,0,0,cvmGet(xi_prim,0,i));
				cvmSet(vector_xi_prim,1,0,cvmGet(xi_prim,1,i));
				cvmSet(vector_xi_prim,2,0,cvmGet(xi_prim,2,i));
				// Axi
				cvmSet(vector_Axi,0,0,cvmGet(Axi,0,i));
				cvmSet(vector_Axi,1,0,cvmGet(Axi,1,i));
				cvmSet(vector_Axi,2,0,cvmGet(Axi,2,i));
				// find cross product  (x'i x Axi) and its transposed vector
				cvCrossProduct(vector_xi_prim,vector_Axi,vectorProduct1);
				cvTranspose(vectorProduct1,transpose_vectorProduct1);
				// find second cross product (x'i x e')
				cvCrossProduct(vector_xi_prim,vector_e_prim,vectorProduct2);				
				// get the magnitude of cross product (x'i x e')
				Magnitude_vectorProduct2 = cvDotProduct(vectorProduct2,vectorProduct2);
				// get the value for bi
				bi = cvDotProduct(vectorProduct1,vectorProduct2);
				bi = bi/Magnitude_vectorProduct2;
				cvmSet(b,i,0,bi);
				// set Zero values to matrices
				cvSetZero(vector_xi_prim);cvSetZero(vector_Axi);
				cvSetZero(vectorProduct1);cvSetZero(vectorProduct2);
			}
			// Solve Mv = b
			int solving_vFlag = 0;
			solving_vFlag = cvSolve(M,b,v,CV_LU);
			// Finally solve for Homography matrix H = A - e'vT
			if (solving_vFlag == 1){// valid solution
				cvTranspose(v,vT);
				cvMatMul(vector_e_prim,vT,e_prim_vT);
				cvSub(A,e_prim_vT,H);
			}


		}
		// return homography found for this points
		return H;
	}
	catch(...){
	}
}
//#include "LICFs.moc"

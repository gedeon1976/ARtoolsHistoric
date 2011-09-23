#include "LICFs.h"

LICFs::LICFs(IplImage *image)
{
	// Do a copy of the image to be analyzed
	lines = 0;
	HoughStorage = cvCreateMemStorage(0);
	imageOriginal = cvCloneImage(image);
	imgType = LEFT;
	LICFs_matchCounter = 0;
	 // matching
	minVal = 0;
	maxVal = 0;
	I_height = this->SubImageSize.height;
	I_width = this->SubImageSize.width;
	pt = cvPoint( I_height/2, I_width/2 );
	minLoc = &pt;
	maxLoc = &pt;
	

}

LICFs::~LICFs(void)
{
	// release memory
	
	cvReleaseImage(&imageOriginal);
	cvReleaseImage(&SubImage);		
	cvReleaseImage(&SubImageGray);	
	cvReleaseImage(&HoughSubImage);	
	cvReleaseImage(&EdgeSubImage);
	//cvReleaseMat(&LICF_feature);
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

		for( int i = 0; i < imageHoughLines->total; i++ )
        {
            CvPoint* line = (CvPoint*)cvGetSeqElem(imageHoughLines,i);
            //cvLine( HoughSubImage, line[0], line[1], CV_RGB(255,0,0), 1, 8 );

			// find line equations
			//if ((line[0].x - line[1].x)== 0){
			//	// this case correspond to a vertical line with undefined slope
			//	// and the line equation is reduced to x = a
			//	currentLine.m = 1000;
			//	currentLine.a = line[1].x;
			//	currentLine.b = 1000;
			//	currentLine.c = 1000;		
			//}
			//else{
			//	currentLine.m = (line[0].y - line[1].y)/(line[0].x - line[1].x);
			//	currentLine.a = currentLine.m*(-line[1].x);
			//	currentLine.b = -1;
			//	currentLine.c = line[1].y;				

			//	}

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
					d_Min1 = min(d_betweenLines1, d_betweenLines1);
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
// Get the current matching points for this image
vector<Matching_LICFs> LICFs::ApplyMatchingLICFs(IplImage *SubImageToMatch,float threshold, int Windowsize){
	try{
		// for all the LICFS found
		LICF_feature = cvCreateMat(Windowsize,Windowsize,CV_8UC1);
		// Create a matrix for the correspond match results
		int widthLICF = (SubImageGray->width - LICF_feature->width + 1);
		int heighLICF =  (SubImageGray->height - LICF_feature->height +1);
		matchOnImageResults = cvCreateMat(heighLICF,widthLICF,CV_32FC1);
		Matching_LICFs currentMatched_LICFs;
		LICFs_matchCounter = 0;

		for (int i=0;i < Actual_LICFs.size();i++){
			// get LICF position
			LICF_FeatureCenter.x = Actual_LICFs.at(i).x_xK;
			LICF_FeatureCenter.y = Actual_LICFs.at(i).y_xK;
			// crop the LICF as an subimage to look for a match in the other image
			cvGetRectSubPix(SubImageGray,LICF_feature,LICF_FeatureCenter);	
			cvMatchTemplate(SubImageToMatch,LICF_feature,matchOnImageResults,CV_TM_CCORR_NORMED);
			// find place of matching in the other image
			cvMinMaxLoc(matchOnImageResults,&minVal,&maxVal,minLoc,maxLoc);
			// save matching points
			if (maxVal > threshold){
				LICFs_matchCounter = LICFs_matchCounter + 1;
				// right LICF
				currentMatched_LICFs.MatchLICFs_R.x_xK = maxLoc->x;
				currentMatched_LICFs.MatchLICFs_R.y_xK = maxLoc->y;
				// left LICF
				currentMatched_LICFs.MatchLICFs_L.x_xK = Actual_LICFs.at(i).x_xK;
				currentMatched_LICFs.MatchLICFs_L.y_xK = Actual_LICFs.at(i).y_xK;
				// save on vector
				Actual_Matched_LICFs.push_back(currentMatched_LICFs);
			
			}
		}
		return Actual_Matched_LICFs;
	}
	catch(...){
	}
}

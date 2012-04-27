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
		// OpenCV mapping new_y = (h-actualImages_Points.yiL)
		// because OpenGL has the origin at the top left of the image
		// and OpenGL has the origin at the Bottom left of the image
		int new_yiL = imageOriginal->height - actualImages_Points.yiL;
		int new_yiR = imageOriginal->height - actualImages_Points.yiR;
		if (imgType == LEFT){
			UpperLeft = cvPoint((actualImages_Points.xiL - abs((percentage*imageOriginal->width/2))),new_yiL - abs((percentage*imageOriginal->height/2)));
			LowerRight = cvPoint((actualImages_Points.xiL + abs((percentage*imageOriginal->width/2))),new_yiL + abs((percentage*imageOriginal->height/2)));

			// set center of the area to analyze
			SubImageCenter.x = actualImages_Points.xiL;
			SubImageCenter.y = new_yiL;
		}
		else{
			UpperLeft = cvPoint((actualImages_Points.xiR - abs((percentage*imageOriginal->width/2))),new_yiR - abs((percentage*imageOriginal->height/2)));
			LowerRight = cvPoint((actualImages_Points.xiR + abs((percentage*imageOriginal->width/2))),new_yiR + abs((percentage*imageOriginal->height/2)));
			// set center of the area to analyze
			SubImageCenter.x = actualImages_Points.xiR;
			SubImageCenter.y = new_yiR;
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
// Get the Edges from the image using the Edge Drawing method
vector<lineParameters> LICFs::ApplyEdgeDrawingEdgeDetector(int MinLineLength){
	try{
		cv::Mat EdgeTmp;
		std::vector<lineParameters> LineSegments;
		int WindowSize = 7;//5
		float gradient_Threshold = 5.22;
		EDlines Edges(SubImageGray);
		EdgeTmp = Edges.EdgeDrawing(SubImageGray,WindowSize,gradient_Threshold);
		//EdgeSubImage = Edge
		LineSegments = Edges.EdgeFindLines(MinLineLength);
		// Show image
		if (imgType == LEFT){
			cv::imshow("Edge subImage L",EdgeTmp);
		}else{
			cv::imshow("Edge subImage R",EdgeTmp);
		}
		return LineSegments;
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


// Get the LICFs of the image using lines from the EDlines method
vector<LICFs_Structure> LICFs::ApplyLICF_Detection(std::vector<lineParameters> Actual_Lines, int LICF_MaxDistanceBetweenLines){
	try{
		// Detect LICFs features		
		// minimum intersection distance in pixels
		// Do a symetric matrix to examine the combinational set of lines
		int LineSize = Actual_Lines.size();
		float isaNewCombination = 0;
		CvMat *LinesMatrix;
		
		float dth_threshold = LICF_MaxDistanceBetweenLines;	 
		lineParameters currentLine;
		LICFs_Structure currentLICF,tmp_currentLICF;

		float d_betweenLines1 = 10;
		float d_betweenLines2 = 10;
		float d_betweenLines3 = 10;
		float d_betweenLines4 = 10;
		float d_Min_betweenLines,d_Min1,d_Min2 = 10;
		lineParameters currentLine1;
		lineParameters currentLine2;
		float thetaAngle_L1 = 0, thetaAngle_L2 = 0;
		float length_L1 = 1, length_L2 = 1;
		
		// check number of lines, to avoid access errors and get at least 1 LICF
		if (LineSize > 1){			// we need at least two lines to form a LICF feature
			LinesMatrix = cvCreateMat(LineSize,LineSize,CV_32FC1);
			cvSetIdentity(LinesMatrix); // we put 1 to the combination already used or not possible
									// as when i = j or (i,j) = (i,j)Transposed at analyzing the for loops
									// 
		}else{
			return Actual_LICFs;
		}

		// Find LICFs intersections		
		// using line intersection method by Paul Borke's web page
		float U_a,U_b = 1;		
		float numerator_U_a = 1, numerator_U_b = 1;
		float denominator_U_a = 1, denominator_U_b = 1;
		float xi,yi;
		float d1_L1,d2_L1,d1_L2,d2_L2;

		for(int i=0; i < LineSize; i++){
			for(int j=0; j < LineSize; j++){
				// check that the checking lines are not the same line
				// at the vector structure, use a symetric matrix to no
				// get repeated values
				isaNewCombination = cvmGet(LinesMatrix,i,j);
				if(isaNewCombination != 1.0f){
					currentLine1 = Actual_Lines.at(i);
					currentLine2 = Actual_Lines.at(j);
					// mark already used combinations
					cvmSet(LinesMatrix,i,j,1.0);
					cvmSet(LinesMatrix,j,i,1.0);
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
						 //if (((U_a >= 0.0f) & (U_a <= 1.0f) & (U_b >= 0.0f) & (U_b <= 1.0f))){
							
							yi = (currentLine1.y1 + U_a*(currentLine1.y2 - currentLine1.y1));
							xi = (currentLine1.x1 + U_a*(currentLine1.x2 - currentLine1.x1));
							// check for values inside the rectangle that defines the area of influence
							// of the 3D pointer
							if ((xi>0)&(xi<SubImageSize.width)&(yi>0)&(yi<SubImageSize.height)){

								// calculate the inclination of the lines
								thetaAngle_L1 = atan((currentLine1.y2 - currentLine1.y1)/(currentLine1.x2 - currentLine1.x1))*180/CV_PI;
								thetaAngle_L2 = atan((currentLine2.y2 - currentLine2.y1)/(currentLine2.x2 - currentLine2.x1))*180/CV_PI;
								currentLine1.thetaAngle = thetaAngle_L1;
								currentLine2.thetaAngle = thetaAngle_L2;
								// calculate the farthest end line points for each LICF line to the intersection point
								// for L1
								d1_L1 = sqrt(pow((xi-currentLine1.x1),2) + pow((yi-currentLine1.y1),2));
								d2_L1 = sqrt(pow((xi-currentLine1.x2),2) + pow((yi-currentLine1.y2),2));
								if (d1_L1 >= d2_L1){
									currentLine1.x_farthest = currentLine1.x1;
									currentLine1.y_farthest = currentLine1.y1;
								}
								if (d2_L1 > d1_L1){
									currentLine1.x_farthest = currentLine1.x2;
									currentLine1.y_farthest = currentLine1.y2;
								}
								// get the L1 line length
								length_L1 = sqrt(pow((xi - currentLine1.x_farthest),2)+ pow((yi - currentLine1.y_farthest),2));
								currentLine1.length = length_L1;

								// for L2
								d1_L2 = sqrt(pow((xi-currentLine2.x1),2) + pow((yi-currentLine2.y1),2));
								d2_L2 = sqrt(pow((xi-currentLine2.x2),2) + pow((yi-currentLine2.y2),2));
								if (d1_L2 >= d2_L2){
									currentLine2.x_farthest = currentLine2.x1;
									currentLine2.y_farthest = currentLine2.y1;
								}
								if (d2_L2 > d1_L2){
									currentLine2.x_farthest = currentLine2.x2;
									currentLine2.y_farthest = currentLine2.y2;
								}
								// get the L2 line length
								length_L2 = sqrt(pow((xi - currentLine2.x_farthest),2)+ pow((yi - currentLine2.y_farthest),2));
								currentLine2.length = length_L2;
								// save LICF
								currentLICF.x_xK = xi;
								currentLICF.y_xK = yi;
								currentLICF.L_1 = currentLine1;
								currentLICF.L_2 = currentLine2;
								// save found LICF
								if ((d1_L1>0)&(d2_L1>0)&(d1_L2>0)&(d2_L2>0)){
									// limit angles between lines min = 10, max 170
									// to avoid tight lines that also should not
									// contain planes
									float angleLimits = abs(currentLine1.thetaAngle - currentLine2.thetaAngle); 
									if ((angleLimits > 10)&(angleLimits < 170)){
										Actual_LICFs.push_back(currentLICF);
									}
								}
							}
						
						// }
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

				
		for(int i=0;i<Actual_Lines.size();i++){
			cvLine(HoughSubImage,cvPoint(Actual_Lines.at(i).x1,Actual_Lines.at(i).y1),cvPoint(Actual_Lines.at(i).x2,Actual_Lines.at(i).y2),CV_RGB(255,0,0));
		}
		// Show image
		if (imgType == LEFT){
			cvNamedWindow("EDlines L");
			cvShowImage("EDlines L",HoughSubImage);
		}else{
			cvNamedWindow("EDlines R");
			cvShowImage("EDlines R",HoughSubImage);
		}
 
		return Actual_LICFs;

	}
	catch(...){
	}
}
// get the LICFs of the image using hough lines
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
							if (d1_L1 >= d2_L1){
								currentLine1.x_farthest = currentLine1.x1;
								currentLine1.y_farthest = currentLine1.y1;
							}
							if (d2_L1 > d1_L1){
								currentLine1.x_farthest = currentLine1.x2;
								currentLine1.y_farthest = currentLine1.y2;
							}
							// for L2
							d1_L2 = sqrt(pow((xi-currentLine2.x1),2) + pow((yi-currentLine2.y1),2));
							d2_L2 = sqrt(pow((xi-currentLine2.x2),2) + pow((yi-currentLine2.y2),2));
							if (d1_L2 >= d2_L2){
								currentLine2.x_farthest = currentLine2.x1;
								currentLine2.y_farthest = currentLine2.y1;
							}
							if (d2_L2 > d1_L2){
								currentLine2.x_farthest = currentLine2.x2;
								currentLine2.y_farthest = currentLine2.y2;
							}
							// save LICF
							currentLICF.x_xK = xi;
							currentLICF.y_xK = yi;
							currentLICF.L_1 = currentLine1;
							currentLICF.L_2 = currentLine2;
							// save found LICF
							if ((d1_L1>0)&(d2_L1>0)&(d1_L2>0)&(d2_L2>0)){
								Actual_LICFs.push_back(currentLICF);
							}
						
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

// refine Matching LICFS using epipolar constraint relationship

// remember that the Homography and the Fundamental matrix must agree
// with the relation HtransposeF + FtransposeH = 0 according to eq 13.3
// from Multiple View Geometry,Zisserman Book
vector<Matching_LICFs> LICFs::RefineMatchingLICFs(cv::Mat F_matrix, vector<Matching_LICFs> actualMatchingLICFs,
												  SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR, float maxError){
try{
		// variables
		vector<Matching_LICFs> refinedMatches;
		vector<Matching_LICFs> refinedMatchesSorted;
		Matching_LICFs swap1,swap2;
		float ConstraintError = 0;
		float tmpConstraintError = 0;
		int Ntrans = actualMatchingLICFs.size();
		if (actualMatchingLICFs.empty()){
			//refinedMatches = vector<Matching_LICFs>();
			return refinedMatches;
		}
		int refinedNtrans = 0 ;
		double minThetaAngle = 10;
		double currentAngleDifference_L1 = 15;
		double currentAngleDifference_L2 = 15;

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
		vector<cv::Point3f> EpilinesL;
		vector<cv::Point3f> EpilinesR;
		vector<cv::Point2f> LICFs_pointsL;
		vector<cv::Point2f> LICFs_pointsR;
		cv::Point2f pointL,pointR;
		// line equations vectors x = xo + vt 
		float xL1,yL1,xL2,yL2;
		float xR1,yR1,xR2,yR2;
		float xK_L = 0,yK_L = 0,xK_R = 0,yK_R = 0;
		float v_L1_L_Mag = 1,v_L2_L_Mag = 1;
		float v_L1_R_Mag = 1,v_L2_R_Mag = 1;
		cv::Vec2f vLine1_L,vLine2_L,vLine1_R,vLine2_R;
		cv::Vec2f v_L1_L,v_L2_L,v_L1_R,v_L2_R;
		cv::Vec2f v_xoL,v_xoR;
		float length_L_L1 = 0,length_L_L2 = 0, finalLength = 0;
		float length_R_L1 = 0,length_R_L2 = 0;
		float minLength = 10;

		if (Ntrans >= 3){

			// fill the matching points
			CvScalar Value;
			Value.val[2] = 0;
			Value.val[3] = 0;
			for (int i =0;i < Ntrans;i++){

				pointL.x = actualMatchingLICFs.at(i).MatchLICFs_L.x_xK + Xo_L;
				pointL.y = actualMatchingLICFs.at(i).MatchLICFs_L.y_xK + Yo_L;
				LICFs_pointsL.push_back(pointL);

				pointR.x = actualMatchingLICFs.at(i).MatchLICFs_R.x_xK + Xo_R;
				pointR.y = actualMatchingLICFs.at(i).MatchLICFs_R.y_xK + Yo_R;
				LICFs_pointsR.push_back(pointR);

			}

			// Get epipolar lines for the right image
			cv::computeCorrespondEpilines(LICFs_pointsL,1,F_matrix,EpilinesR);
			// Get epipolar lines for the left image
			cv::computeCorrespondEpilines(LICFs_pointsR,2,F_matrix,EpilinesL);
			
			// APPLY THE EPIPOLAR CONSTRAINT TO ALL MATCHED POINTS

			// lines parameters ax + by + c =0
			double aL,bL,cL,aR,bR,cR;
			double xoL,yoL,xoR,yoR;
			for (int i=0; i < Ntrans; i++){

				aL = EpilinesL.at(i).x;	aR = EpilinesR.at(i).x;
				bL = EpilinesL.at(i).y;	bR = EpilinesR.at(i).y;
				cL = EpilinesL.at(i).z;	cR = EpilinesR.at(i).z;

				xoL = actualMatchingLICFs.at(i).MatchLICFs_L.x_xK + Xo_L;
				xoR = actualMatchingLICFs.at(i).MatchLICFs_R.x_xK + Xo_R;
				yoL = actualMatchingLICFs.at(i).MatchLICFs_L.y_xK + Yo_L;
				yoR = actualMatchingLICFs.at(i).MatchLICFs_R.y_xK + Yo_R;
				// find the error according to the equation:

				tmpConstraintError = pow((abs(aL*xoL + bL*yoL + cL)/sqrt(pow(aL,2) + pow(bL,2))),2) 
					+ pow((abs(aR*xoR + bR*yoR + cR)/sqrt(pow(aR,2)+ pow(bR,2))),2);
				
				// check epipolar error
				if (tmpConstraintError < maxError){
					// save matching epipolar error
					actualMatchingLICFs.at(i).epipolar_error = tmpConstraintError;
					// check line matching
					currentAngleDifference_L1 = actualMatchingLICFs.at(i).MatchLICFs_L.L_1.thetaAngle - actualMatchingLICFs.at(i).MatchLICFs_R.L_1.thetaAngle;
					currentAngleDifference_L2 = actualMatchingLICFs.at(i).MatchLICFs_L.L_2.thetaAngle - actualMatchingLICFs.at(i).MatchLICFs_R.L_2.thetaAngle;
					actualMatchingLICFs.at(i).line_matching_error = abs(currentAngleDifference_L1) +  abs(currentAngleDifference_L2);

					if ((abs(currentAngleDifference_L1) < minThetaAngle)&(abs(currentAngleDifference_L2) < minThetaAngle)){
						// change line ends = normalize line ends to a minimum distance from the
						// line intersection point
						length_L_L1 = actualMatchingLICFs.at(i).MatchLICFs_L.L_1.length; 
						length_L_L2 = actualMatchingLICFs.at(i).MatchLICFs_L.L_2.length; 
						finalLength = min(abs(length_L_L1),abs(length_L_L2));
						if (finalLength < minLength ){
							finalLength = minLength;
						}
						//line vector parameters
					
						// Left Match
						xK_L = actualMatchingLICFs.at(i).MatchLICFs_L.x_xK;
						yK_L = actualMatchingLICFs.at(i).MatchLICFs_L.y_xK;
						xL1 = actualMatchingLICFs.at(i).MatchLICFs_L.L_1.x_farthest - xK_L;
						yL1 = actualMatchingLICFs.at(i).MatchLICFs_L.L_1.y_farthest - yK_L;
						xL2 = actualMatchingLICFs.at(i).MatchLICFs_L.L_2.x_farthest - xK_L;
						yL2 = actualMatchingLICFs.at(i).MatchLICFs_L.L_2.y_farthest - yK_L;

						// Rigth Match
						xK_R = actualMatchingLICFs.at(i).MatchLICFs_R.x_xK;
						yK_R = actualMatchingLICFs.at(i).MatchLICFs_R.y_xK;
						xR1 = actualMatchingLICFs.at(i).MatchLICFs_R.L_1.x_farthest - xK_R;
						yR1 = actualMatchingLICFs.at(i).MatchLICFs_R.L_1.y_farthest - yK_R;
						xR2 = actualMatchingLICFs.at(i).MatchLICFs_R.L_2.x_farthest - xK_R;
						yR2 = actualMatchingLICFs.at(i).MatchLICFs_R.L_2.y_farthest - yK_R;
						
						length_R_L1 = actualMatchingLICFs.at(i).MatchLICFs_R.L_1.length; 
						length_R_L2 = actualMatchingLICFs.at(i).MatchLICFs_R.L_2.length;

						// set new end line points using vectors v = xo + vt for Left Match
						v_xoL = cv::Vec2f((float)xK_L,(float)yK_L);
						vLine1_L = cv::Vec2f((float)xL1/length_L_L1,(float)yL1/length_L_L1);
						vLine2_L = cv::Vec2f((float)xL2/length_L_L2,(float)yL2/length_L_L2);
						//	L1
						v_L1_L = v_xoL + finalLength*vLine1_L;
						//	L2
						v_L2_L = v_xoL + finalLength*vLine2_L;
						// new lengths
						v_L1_L_Mag = sqrt(pow(v_L1_L[0] - xK_L,2) + pow(v_L1_L[1] - yK_L,2));
						v_L2_L_Mag = sqrt(pow(v_L2_L[0] - xK_L,2) + pow(v_L2_L[1] - yK_L,2));

						// set new end line points using vectors v = xo + vt for Right Match
						v_xoR = cv::Vec2f((float)xK_R,(float)yK_R);
						vLine1_R = cv::Vec2f((float)xR1/length_R_L1,(float)yR1/length_R_L1);
						vLine2_R = cv::Vec2f((float)xR2/length_R_L2,(float)yR2/length_R_L2);
						//	L1
						v_L1_R = v_xoR + finalLength*vLine1_R;
						//	L2
						v_L2_R = v_xoR + finalLength*vLine2_R;
						// new lengths
						v_L1_R_Mag = sqrt(pow(v_L1_R[0] - xK_R,2) + pow(v_L1_R[1] - yK_R,2));
						v_L2_R_Mag = sqrt(pow(v_L2_R[0] - xK_R,2) + pow(v_L2_R[1] - yK_R,2));

						
						// save new end line points
						actualMatchingLICFs.at(i).MatchLICFs_L.L_1.x_farthest = v_L1_L[0];
						actualMatchingLICFs.at(i).MatchLICFs_L.L_1.y_farthest = v_L1_L[1];
						actualMatchingLICFs.at(i).MatchLICFs_L.L_2.x_farthest = v_L2_L[0];
						actualMatchingLICFs.at(i).MatchLICFs_L.L_2.y_farthest = v_L2_L[1];
						actualMatchingLICFs.at(i).MatchLICFs_L.L_1.length = v_L1_L_Mag;
						actualMatchingLICFs.at(i).MatchLICFs_L.L_2.length = v_L2_L_Mag;

						actualMatchingLICFs.at(i).MatchLICFs_R.L_1.x_farthest = v_L1_R[0];
						actualMatchingLICFs.at(i).MatchLICFs_R.L_1.y_farthest = v_L1_R[1];
						actualMatchingLICFs.at(i).MatchLICFs_R.L_2.x_farthest = v_L2_R[0];
						actualMatchingLICFs.at(i).MatchLICFs_R.L_2.y_farthest = v_L2_R[1];
						actualMatchingLICFs.at(i).MatchLICFs_R.L_1.length = v_L1_R_Mag;
						actualMatchingLICFs.at(i).MatchLICFs_R.L_2.length = v_L2_R_Mag;


						// save refined matches
						refinedMatches.push_back(actualMatchingLICFs.at(i));
						ConstraintError = ConstraintError + tmpConstraintError;
						refinedNtrans = refinedNtrans + 1;
					}
				}
			}
			
			ConstraintError = ConstraintError/refinedNtrans;
		
		}
		else{// Ntrans <3
				ConstraintError = -1; // not enough matching found
		}

		// emit the corresponding signal
		//emit SendEpipolarErrorConstraint(ConstraintError);
		
		// return results
		// Get the best matching point using the insertion sorting algorithm
		// so the best match is the first value and so on
		int k,j = 0;
		float error1,error2;
		for (int i = 1;i <refinedMatches.size();i++){
			k = i;
 			j = k-1;			
			for(k;k>0;k--){
				error1 = refinedMatches.at(k).epipolar_error;
				error2 = refinedMatches.at(j).epipolar_error;
				if (error1 < error2){
					swap1 = refinedMatches.at(k);
					swap2 = refinedMatches.at(j);
					swap<Matching_LICFs>(refinedMatches.at(k),refinedMatches.at(j));
				
				}				
			}
		}
		return refinedMatches;


}
catch(...){
}

}

// Draw LICFs Matches
void LICFs::DrawLICF_Matches(cv::Mat leftImage, cv::Mat rightImage, std::vector<Matching_LICFs> matchedPoints){
	try{
		// points and matches
		cv::Mat LICFsMatches;
		vector<cv::KeyPoint> KeyPointsL;
		vector<cv::KeyPoint> KeyPointsR;
		vector<cv::DMatch> matches;
		cv::KeyPoint pointL,pointR;
		cv::DMatch currentMatch;

		int size = matchedPoints.size();
		int counter = 0;
		for (int i=0;i < size;i++){
				
				//	left xk point
				pointL.pt.x = matchedPoints.at(i).MatchLICFs_L.x_xK;
				pointL.pt.y = matchedPoints.at(i).MatchLICFs_L.y_xK;
				KeyPointsL.push_back(pointL);
				//	right xk 
				pointR.pt.x = matchedPoints.at(i).MatchLICFs_R.x_xK;
				pointR.pt.y = matchedPoints.at(i).MatchLICFs_R.y_xK;
				KeyPointsR.push_back(pointR);
				// match xk
				currentMatch.imgIdx = counter;
				currentMatch.distance = matchedPoints.at(i).epipolar_error;
				currentMatch.trainIdx = counter;
				currentMatch.queryIdx = counter;
				counter = counter + 1;
				// matches 
				matches.push_back(currentMatch);
				//	left ex1 points
				pointL.pt.x = matchedPoints.at(i).MatchLICFs_L.L_1.x_farthest;
				pointL.pt.y = matchedPoints.at(i).MatchLICFs_L.L_1.y_farthest;
				KeyPointsL.push_back(pointL);
				//	right ex1 
				pointR.pt.x = matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest;
				pointR.pt.y = matchedPoints.at(i).MatchLICFs_R.L_1.y_farthest;
				KeyPointsR.push_back(pointR);
				// match ex1
				currentMatch.imgIdx = counter;
				currentMatch.distance = matchedPoints.at(i).epipolar_error;
				currentMatch.trainIdx = counter;
				currentMatch.queryIdx = counter;
				counter = counter + 1;
				// matches
				matches.push_back(currentMatch);
				//	left ex2 points
				pointL.pt.x = matchedPoints.at(i).MatchLICFs_L.L_2.x_farthest;
				pointL.pt.y = matchedPoints.at(i).MatchLICFs_L.L_2.y_farthest;
				KeyPointsL.push_back(pointL);
				//	right ex2
				pointR.pt.x = matchedPoints.at(i).MatchLICFs_R.L_2.x_farthest;
				pointR.pt.y = matchedPoints.at(i).MatchLICFs_R.L_2.y_farthest;
				KeyPointsR.push_back(pointR);
				// match ex2
				currentMatch.imgIdx = counter;
				currentMatch.distance = matchedPoints.at(i).epipolar_error;
				currentMatch.trainIdx = counter;
				currentMatch.queryIdx = counter;
				counter = counter + 1;
				// matches
				matches.push_back(currentMatch);
		
		}
		// draw LICFS matches
		cv::drawMatches(leftImage,KeyPointsL,rightImage,KeyPointsR,matches,LICFsMatches,cv::Scalar::all(-1),
			cv::Scalar::all(-1),vector<char>(),cv::DrawMatchesFlags::DEFAULT);
		cv::imshow("LICF Matches",LICFsMatches);

	}
	catch(...){
	}
}



// Draw detected planes
// using x' - Hx <= epsilon as valid points
void LICFs::DrawLICF_detectedPlane(cv::Mat x_prim,cv::Mat Hx, cv::Mat H, double epsilon){
	try{
		double Xdiff = 0,Ydiff = 0, Scale = 1;
		double distance_toHx = 0;
		double delta_x = 0,delta_y = 0;
		cv::Mat detectedPlane = cv::Mat(x_prim.size(),CV_32FC1,cv::Scalar::all(0));
		cv::Mat X_PRIM = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));
		cv::Mat X = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));
		cv::Mat H_X = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));

		// draw planes using B G R channels
		cv::Mat ImageComparison;
		cv::Mat detectedPlaneOn_RGBChannels;
		vector<cv::Mat> channelImage1,channelImage2;
		cv::Mat ImageR = cv::Mat(x_prim.size(),CV_8UC3,cv::Scalar::all(0));
		cv::Mat ImageG = cv::Mat(x_prim.size(),CV_8UC3,cv::Scalar::all(0));
		cv::Mat fillChannel = cv::Mat(x_prim.size(),CV_8UC1,cv::Scalar::all(0));

		// built the G channel image

		channelImage1.push_back(fillChannel);	// B
		channelImage1.push_back(Hx);			// G
		channelImage1.push_back(fillChannel);	// R

		// built the R channel image

		channelImage2.push_back(fillChannel);	// B
		channelImage2.push_back(fillChannel);	// G
		channelImage2.push_back(x_prim);		// R

		cv::merge(channelImage1,ImageR);
		cv::merge(channelImage2,ImageG);

		detectedPlaneOn_RGBChannels = ImageR + ImageG;
		cv::compare(x_prim,Hx,ImageComparison,cv::CMP_EQ);

		// Find plane using H
/*		float pixelValue = 0;
		for (int x=0; x < x_prim.cols;x++){
			for(int y=0;y < x_prim.rows;y++){
				X_PRIM.ptr<double>(0)[0] = x;
				X_PRIM.ptr<double>(1)[0] = y;
				X_PRIM.ptr<double>(2)[0] = 1;

				X.ptr<double>(0)[0] = x;
				X.ptr<double>(1)[0] = y;
				X.ptr<double>(2)[0] = 1;

				H_X = H*X;
				Scale = H_X.ptr<double>(2)[0];
				Xdiff = abs(x - (H_X.ptr<double>(0)[0]/Scale)); 
				Ydiff = abs(y - (H_X.ptr<double>(1)[0]/Scale));

				// check distance between points
				delta_x = Xdiff;
				delta_y = Ydiff;
				distance_toHx = sqrt(pow(delta_x,2)+ pow(delta_y,2));

				if (distance_toHx < epsilon){
						pixelValue = 1;
						detectedPlane.ptr<float>(y)[x] = pixelValue;
				}else{
						pixelValue = 0;
						detectedPlane.ptr<float>(y)[x] = pixelValue;
				}
			}
		}*/
		// draw Plane detected
		cv::imshow("Detected Planes RGB Channels",detectedPlaneOn_RGBChannels);
		cv::imshow("x' vs Hx",ImageComparison);
		//cv::imshow("Detected Plane",detectedPlane);
	}
	catch(...){
	}
}
// Get the epipolar constraint value
// this is an indicator of the level of coplanarity of the 
// intersection LICFs found
LICFs_EpipolarConstraintResult LICFs::GetEpipolarConstraintError(vector<Matching_LICFs> matchedPoints,CvMat* F_matrix,SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR){
	try{
		// variables
		LICFs_EpipolarConstraintResult finalResults;
		float ConstraintError = 0;
		float tmpConstraintError = 0;
		int Ntrans = matchedPoints.size();
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

				Value.val[0] = matchedPoints.at(i).MatchLICFs_L.x_xK + Xo_L;
				Value.val[1] = matchedPoints.at(i).MatchLICFs_L.y_xK + Yo_L;
				cvSet2D(LICFs_pointsL,0,i,Value);
				Value.val[0] = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Value.val[1] = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;
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
				xoL = matchedPoints.at(i).MatchLICFs_L.x_xK + Xo_L;
				xoR = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				yoL = matchedPoints.at(i).MatchLICFs_L.y_xK + Yo_L;
				yoR = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;
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
// check Homography Matrix Error
// x' = Hx, by that we check e = distance(x',Hx)/n
double LICFs::CheckHomographyMatrix(vector<cv::Point2f> x, vector<cv::Point2f> x_prim, cv::Mat H_matrix){
	try{
		// convert points to Homogeneus
		double Error = 0;
		double tmpError = 0;
		double Epsilon = 0.01;
		double distance_toHx = 0;
		double distanceAccum = 0;
		double delta_x = 0,delta_y = 0;
		double a,b,c;
		int correctCount = 0;
		cv::Mat Hx = cv::Mat(3,1,H_matrix.type(),cv::Scalar::all(0));
		cv::Mat tmpMat = cv::Mat(3,1,H_matrix.type(),cv::Scalar::all(0));
		cv::Mat tmpMat2 = cv::Mat(3,3,CV_32FC1,cv::Scalar::all(0));

		int size = x.size();
		cv::Point2f leftPoint,rightPoint;
		cv::Point3f Hx_Point;
		/*vector<cv::Point3f> HomogeneousLeftPoints;
		vector<cv::Point3f> HomogeneousRightPoints;
		cv::convertPointsToHomogeneous(LeftPoints,HomogeneousLeftPoints);
		cv::convertPointsToHomogeneous(RightPoints,HomogeneousRightPoints);*/
		
		for (int i=0; i <size; i++){

			rightPoint = x_prim.at(i);
			leftPoint = x.at(i);
			tmpMat.ptr<double>(0)[0] = leftPoint.x;
			tmpMat.ptr<double>(1)[0] = leftPoint.y;
			tmpMat.ptr<double>(2)[0] = 1;
			
			Hx = H_matrix*tmpMat;
			Hx_Point.z = Hx.ptr<double>(2)[0];
			Hx_Point.x = Hx.ptr<double>(0)[0]/Hx_Point.z;
			Hx_Point.y = Hx.ptr<double>(1)[0]/Hx_Point.z;
			
			// check distance between points
			delta_x = rightPoint.x - Hx_Point.x;
			delta_y = rightPoint.y - Hx_Point.y;
			distance_toHx = sqrt(pow(delta_x,2)+ pow(delta_y,2));
			distanceAccum = distance_toHx + distanceAccum;

			printf("match %i distance: %E\n",i,distance_toHx);
		
		}
		// printf error
		Error = distanceAccum/size; // Ideal error = 0 max = # of matches = 1 pixel per match		
		std::cout<<"d(x'- Hx) pixel average per match:"<<endl<<Error<<endl;
		return Error;

	}
	catch(...){
	}
}
// Check Homography conformity
// Htrans*F + Ftrans*H = 0  according to Zisserman Chapter 13.3
double LICFs::CheckHomographyConformity(cv::Mat H_matrix, cv::Mat F_matrix){
	try{
		cv::Mat Comformity = cv::Mat(3,3,cv::DataType<double>::type,cv::Scalar::all(0));
		cv::Scalar Suma;
		double value = 1;
		Comformity = H_matrix.t()*F_matrix + F_matrix.t()*H_matrix;
		Suma = cv::sum(Comformity);
		std::cout<<"Comformity value:"<< Suma.val[0]<<std::endl;
		std::cout<<"Htrans*F + Ftrans*H matrix"<<endl<<Comformity<<endl<<endl;
		value = Suma.val[0];
		return value;
	}
	catch(...){
	}
}

// Get the homography between the left and right images
cv::Mat LICFs::FindLICF_BasedHomography(vector<Matching_LICFs> matchedPoints,cv::Mat F_matrix, cv::Mat epipole, cv::Mat epipole_prim,
								  SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR){
	try{
		// Homography matrix
		cv::Mat H_matrix = cv::Mat(3,3,CV_32FC1,cv::Scalar::all(1));
		// matrices for saving epilolar lines corresponding to points ex1 and ex2
		// L1' = Fex1 ; L2' = Fex2
		cv::Mat x = cv::Mat::zeros(3,1,CV_32FC1);
		cv::Mat x_prim = cv::Mat::zeros(3,1,CV_32FC1);
		cv::Mat EpilinesL = cv::Mat(4,3,CV_32FC1);
		cv::Mat EpilinesR = cv::Mat(4,3,CV_32FC1);
		lineParameters Line1,Line1_prim;
		lineParameters Line2,Line2_prim;
		lineIntersection lineCrossPointLine1,lineCrossPointLine2;
		float aR1,bR1,cR1,aR2,bR2,cR2;
		CvPoint ex1_prim, ex2_prim;
		float dL1,dL2,dL3,dL4;
		// check number of matches
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
		vector<cv::Point2f> LICFs_pointsL;
		vector<cv::Point2f> LICFs_pointsR;
		CvScalar Value;
		cv::Point2f   pointL,pointR;

		if (matchSize >=1){					
		
			Value.val[2] = 0;
			Value.val[3] = 0;
			for (int i =0;i < 1;i++){

				//Value.val[0] = matchedPoints.at(i).MatchLICFs_L.x_xK + Xo_L;
				//Value.val[1] = matchedPoints.at(i).MatchLICFs_L.y_xK + Yo_L;
				//cvSet2D(LICFs_pointsL,0,0,Value);
				pointL.x = matchedPoints.at(i).MatchLICFs_L.x_xK + Xo_L;
				pointL.y = matchedPoints.at(i).MatchLICFs_L.y_xK + Yo_L;
				LICFs_pointsL.push_back(pointL);
				
				//cvmSet(ex1,0,0,Value.val[0]);
				//cvmSet(ex1,1,0,Value.val[1]);
								
				//Value.val[0] = matchedPoints.at(i).MatchLICFs_L.L_1.x_farthest + Xo_L;
				//Value.val[1] = matchedPoints.at(i).MatchLICFs_L.L_1.y_farthest + Yo_L;
				//cvSet2D(LICFs_pointsL,0,1,Value);
				pointL.x = matchedPoints.at(i).MatchLICFs_L.L_1.x_farthest + Xo_L;
				pointL.y = matchedPoints.at(i).MatchLICFs_L.L_1.y_farthest + Yo_L;
				LICFs_pointsL.push_back(pointL);

				//cvmSet(ex1,0,0,Value.val[0]);
				//cvmSet(ex1,1,0,Value.val[1]);
				//cvmSet(ex1,2,0,1.0f);
				
				//Value.val[0] = matchedPoints.at(i).MatchLICFs_L.L_2.x_farthest + Xo_L;
				//Value.val[1] = matchedPoints.at(i).MatchLICFs_L.L_2.y_farthest + Yo_L;
				//cvSet2D(LICFs_pointsL,0,2,Value);
				//cvmSet(ex2,0,0,Value.val[0]);
				//cvmSet(ex2,1,0,Value.val[1]);
				//cvmSet(ex2,2,0,1.0f);
				pointL.x = matchedPoints.at(i).MatchLICFs_L.L_2.x_farthest + Xo_L;
				pointL.y = matchedPoints.at(i).MatchLICFs_L.L_2.y_farthest + Yo_L;
				LICFs_pointsL.push_back(pointL);
			
				// epipole value: e // normalize this values
				//float norm = epipole.ptr<double>(0)[2];
				Value.val[0] = epipole.ptr<double>(0)[0];
				Value.val[1] = epipole.ptr<double>(0)[1];
				pointL.x = Value.val[0];
				pointL.y = Value.val[1];
				LICFs_pointsL.push_back(pointL);
				//cvSet2D(LICFs_pointsL,0,3,Value);

				/*Value.val[2] = cvmGet(&epipole,0,2);
				Value.val[0] = cvmGet(&epipole,0,0)/Value.val[2];
				Value.val[1] = cvmGet(&epipole,0,1)/Value.val[2];
				Value.val[2] = 0.0f;*/				

				//cvmGet(epipole,0,2));			

				// find the corresponding lines on the right image
				cv::computeCorrespondEpilines(LICFs_pointsL,1,F_matrix,EpilinesR);
				std::cout<<"Epilines R"<<endl<<EpilinesR<<endl<<endl;
				//L1_prim = F_matrix.mul(ex1);
				//L2_prim = F_matrix.mul(ex2);

				//cvComputeCorrespondEpilines(LICFs_pointsL,1,F_matrix,Epilines);
				//cvMatMul(&F_matrix,ex1,L1_prim);
				//cvMatMul(&F_matrix,ex2,L2_prim);

				// find line intersections that correspond to ex1' and ex2'
				
				/*aR1 = cvmGet(Epilines,0,1);aR2 = cvmGet(Epilines,0,2);
				bR1 = cvmGet(Epilines,1,1);bR2 = cvmGet(Epilines,1,2);
				cR1 = cvmGet(Epilines,2,1);cR2 = cvmGet(Epilines,2,2);*/

				// get the coefficients for each line ax + by + c = 0
				// remember l'i = Fxi, only for ex1 and ex2
				aR1 = EpilinesR.ptr<float>(1)[0];aR2 = EpilinesR.ptr<float>(2)[0];
				bR1 = EpilinesR.ptr<float>(1)[1];bR2 = EpilinesR.ptr<float>(2)[1];
				cR1 = EpilinesR.ptr<float>(1)[2];cR2 = EpilinesR.ptr<float>(2)[2];

				Line1.x1 = matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest + Xo_R;
				Line1.y1 = matchedPoints.at(i).MatchLICFs_R.L_1.y_farthest + Yo_R;
				Line1.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Line1.y2 = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;

				Line1_prim.x1 = matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest;// + Xo_R;
				Line1_prim.y1 = -(aR1*Line1_prim.x1)/bR1 - cR1/bR1;// + Yo_R;
				Line1_prim.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK;// + Xo_R;
				Line1_prim.y2 = -(aR1*Line1_prim.x2)/bR1 - cR1/bR1 ;//+ Yo_R;

				Line2.x1 = matchedPoints.at(i).MatchLICFs_R.L_2.x_farthest + Xo_R;
				Line2.y1 = matchedPoints.at(i).MatchLICFs_R.L_2.y_farthest + Yo_R;
				Line2.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Line2.y2 = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;

				Line2_prim.x1 = matchedPoints.at(i).MatchLICFs_R.L_2.x_farthest;// + Xo_R;
				Line2_prim.y1 = -(aR2*Line2_prim.x1)/bR2 - cR2/bR2;// + Yo_R;
				Line2_prim.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK;// + Xo_R;
				Line2_prim.y2 = -(aR2*Line2_prim.x2)/bR2 - cR2/bR2;// + Yo_R;

				lineCrossPointLine1 = GetLineIntersection(Line1,Line1_prim);
				lineCrossPointLine2 = GetLineIntersection(Line2,Line2_prim);

				// calculus using distance between l' and ex2' instead of intersections
				// for ex'1
				dL1 = abs(aR1*Line1.x1 + bR1*Line1.y1 + cR1)/sqrt(pow(aR1,2)+ pow(bR1,2));
				dL3 = abs(aR1*lineCrossPointLine1.xi + bR1*lineCrossPointLine1.yi + cR1)/sqrt(pow(aR1,2)+ pow(bR1,2));
				// for ex'2
				dL2 = abs(aR2*Line2.x1 + bR2*Line2.y1 + cR2)/sqrt(pow(aR2,2)+ pow(bR2,2));
				dL4 = abs(aR2*lineCrossPointLine2.xi + bR2*lineCrossPointLine2.yi + cR2)/sqrt(pow(aR2,2)+ pow(bR2,2)); 


				// Corresponding points on the other image

				/*Value.val[0] = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Value.val[1] = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;
				cvSet2D(LICFs_pointsR,0,0,Value); */
				pointR.x = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				pointR.y = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;
				LICFs_pointsR.push_back(pointR);

				/*Value.val[0] = lineCrossPointLine1.xi;//matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest + Xo_R;
				Value.val[1] = lineCrossPointLine1.yi;//matchedPoints.at(i).MatchLICFs_R.L_1.y_farthest + Yo_R;
				cvSet2D(LICFs_pointsR,0,1,Value);*/
				
				pointR.x = lineCrossPointLine1.xi;
				pointR.y = lineCrossPointLine1.yi;
				LICFs_pointsR.push_back(pointR);

				/*Value.val[0] = lineCrossPointLine2.xi;//matchedPoints.at(i).MatchLICFs_R.L_2.x_farthest + Xo_R;
				Value.val[1] = lineCrossPointLine2.yi;//matchedPoints.at(i).MatchLICFs_R.L_2.y_farthest + Yo_R;
				cvSet2D(LICFs_pointsR,0,2,Value);*/
				
				pointR.x = lineCrossPointLine2.xi;
				pointR.y = lineCrossPointLine2.yi;
				LICFs_pointsR.push_back(pointR);

				// epipole_prim value: e'

				pointR.x = epipole_prim.ptr<double>(0)[0];
				pointR.y = epipole_prim.ptr<double>(1)[0];
				LICFs_pointsR.push_back(pointR);

				/*Value.val[2] = cvmGet(epipole_prim,2,0);  
				Value.val[0] = cvmGet(epipole_prim,0,0)/Value.val[2];
				Value.val[1] = cvmGet(epipole_prim,1,0)/Value.val[2];
				Value.val[2] = 0.0f;*/
				
				//cvmGet(epipole_prim,0,2));
			}
			// find the homography
			double maxPixelErr = 3;
			H_matrix = cv::findHomography(LICFs_pointsL,LICFs_pointsR,cv::RANSAC,maxPixelErr);
			// check H matrix error
			double H_error = CheckHomographyMatrix(LICFs_pointsL,LICFs_pointsR,H_matrix);
			std::cout<<"H matrix"<<endl<<H_matrix<<endl<<endl;
			//cvFindHomography(LICFs_pointsL,LICFs_pointsR,H_matrix,CV_RANSAC,maxPixelErr);

		}
		return H_matrix;
	}
	catch(...){
	}
}

// Get the homography using the method proposed in the chapter 13 (algorithm 13.1) of the book
// multiple view geometry by Hartley and Zisserman
cv::Mat LICFs::FindLICF_BasedHomographyZissermman(vector<Matching_LICFs> matchedPoints, cv::Mat F_matrix, cv::Mat epipole, cv::Mat epipole_prim, 
												  SubArea_Structure SubAreaImageL, SubArea_Structure SubAreaImageR){
	try{
		// Matrices
		cv::Mat H = cv::Mat(3,3,cv::DataType<double>::type,cv::Scalar::all(1));
		cv::Mat A = cv::Mat(3,3,cv::DataType<double>::type,cv::Scalar::all(0));
		cv::Mat M = cv::Mat(3,3,cv::DataType<double>::type,cv::Scalar::all(0));		
		cv::Mat v = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));
		cv::Mat vT = cv::Mat(1,3,cv::DataType<double>::type,cv::Scalar::all(0));//(1,3)
		cv::Mat e_prim_vT = cv::Mat(3,3,cv::DataType<double>::type,cv::Scalar::all(0));
		cv::Mat b = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));
		cv::Mat xi_prim = cv::Mat(3,3,cv::DataType<double>::type,cv::Scalar::all(0));
		
		// find matching points using intersections between Lines of LICFS and epipolar lines
		// matrices for saving epilolar lines corresponding to points ex1 and ex2
		// L1' = Fex1 ; L2' = Fex2
		vector<cv::Point3f> Epilines; 
		lineParameters Line_Xk,Line_Xk_prim;
		lineParameters Line1,Line1_prim;
		lineParameters Line2,Line2_prim;
		lineIntersection lineCrossPointLine0,lineCrossPointLine1,lineCrossPointLine2;
		float aR0,bR0,cR0,aR1,bR1,cR1,aR2,bR2,cR2;
		float do_L1,do_L2,dL1,dL2,dL3,dL4;		

		// Homography Calculus
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

		// set [e']x = cross product matrix representation
		double e1,e2,e3;
		e1 = epipole_prim.ptr<double>(0)[0];
		e2 = epipole_prim.ptr<double>(1)[0];
		e3 = epipole_prim.ptr<double>(2)[0];

		double skew_symetric_data[3][3] =  {{  0,-e3, e2},
											{ e3,  0,-e1},
											{-e2, e1,  0}};
		cv::Mat e_prim_skew_symetric = cv::Mat(3,3,F_matrix.type(),skew_symetric_data);
		
		// set A = [e']xF
		A = e_prim_skew_symetric*F_matrix;
		//// Get the best matching point using the insertion sorting algorithm
		//// the best is the first value
		//Matching_LICFs swap1,swap2;
		//for (int i = 1;i <matchSize;i++){
		//	for(int k = i;(k>0)&(matchedPoints.at(k).epipolar_error < matchedPoints.at(k-1).epipolar_error);k--){
		//		
		//		swap<Matching_LICFs>(matchedPoints.at(k),matchedPoints.at(k-1));
		//	}
		//}
		vector<cv::Point2f> LICFs_pointsL;
		vector<cv::Point2f> LICFs_pointsR;
		// Ask for matching LICFs
		if(matchSize >= 1){ // we need at least 1 LICF feature = 3 matched points xk,L1 and L2 ends
			// First at all, Get intersection points
			
			cv::Point2f PointL,PointR;

			CvScalar Value;
			Value.val[2] = 0;
			Value.val[3] = 0;
			for (int i =0;i < 1;i++){

				PointL.x = matchedPoints.at(i).MatchLICFs_L.x_xK + Xo_L;
				PointL.y = matchedPoints.at(i).MatchLICFs_L.y_xK + Yo_L;
				LICFs_pointsL.push_back(PointL);
												
				PointL.x = matchedPoints.at(i).MatchLICFs_L.L_1.x_farthest + Xo_L;
				PointL.y = matchedPoints.at(i).MatchLICFs_L.L_1.y_farthest + Yo_L;
				LICFs_pointsL.push_back(PointL);
				
				PointL.x = matchedPoints.at(i).MatchLICFs_L.L_2.x_farthest + Xo_L;
				PointL.y = matchedPoints.at(i).MatchLICFs_L.L_2.y_farthest + Yo_L;
				LICFs_pointsL.push_back(PointL);
				
				// epipole value: e // normalize this values
				PointL.x = epipole.ptr<float>(0)[0];
				PointL.y = epipole.ptr<float>(0)[1];
				LICFs_pointsL.push_back(PointL);

				// find the corresponding lines on the right image
				cv::computeCorrespondEpilines(LICFs_pointsL,1,F_matrix,Epilines);
				
				// find line intersections that correspond to xk' , ex1' and ex2'
				aR0 = Epilines.at(0).x;aR1 = Epilines.at(1).x;aR2 = Epilines.at(2).x;
				bR0 = Epilines.at(0).y;bR1 = Epilines.at(1).y;bR2 = Epilines.at(2).y;
				cR0 = Epilines.at(0).z;cR1 = Epilines.at(1).z;cR2 = Epilines.at(2).z;

				Line_Xk.x1 = matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest + Xo_R;
				Line_Xk.y1 = matchedPoints.at(i).MatchLICFs_R.L_1.y_farthest + Yo_R;
				Line_Xk.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Line_Xk.y2 = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;

				Line_Xk_prim.x1 = matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest;// + Xo_R;
				Line_Xk_prim.y1 = -(aR0*Line_Xk_prim.x1)/bR0 - cR0/bR0;// + Yo_R;
				Line_Xk_prim.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK;// + Xo_R;
				// check if the x value is the same, avoid this situation
				if(Line_Xk_prim.x1 == Line_Xk_prim.x2){
					Line_Xk_prim.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + 1;
				}
				Line_Xk_prim.y2 = -(aR0*Line_Xk_prim.x2)/bR0 - cR0/bR0 ;//+ Yo_R;

				Line1.x1 = matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest + Xo_R;
				Line1.y1 = matchedPoints.at(i).MatchLICFs_R.L_1.y_farthest + Yo_R;
				Line1.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Line1.y2 = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;

				Line1_prim.x1 = matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest;// + Xo_R;
				Line1_prim.y1 = -(aR1*Line1_prim.x1)/bR1 - cR1/bR1;// + Yo_R;
				Line1_prim.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK;// + Xo_R;
				// check if the x value is the same, avoid this situation
				if(Line1_prim.x1 == Line1_prim.x2){
					Line1_prim.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + 1;
				}
				Line1_prim.y2 = -(aR1*Line1_prim.x2)/bR1 - cR1/bR1 ;//+ Yo_R;

				Line2.x1 = matchedPoints.at(i).MatchLICFs_R.L_2.x_farthest + Xo_R;
				Line2.y1 = matchedPoints.at(i).MatchLICFs_R.L_2.y_farthest + Yo_R;
				Line2.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Line2.y2 = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;

				Line2_prim.x1 = matchedPoints.at(i).MatchLICFs_R.L_2.x_farthest;// + Xo_R;
				Line2_prim.y1 = -(aR2*Line2_prim.x1)/bR2 - cR2/bR2;// + Yo_R;
				Line2_prim.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK;// + Xo_R;
					// check if the x value is the same, avoid this situation
				if(Line2_prim.x1 == Line2_prim.x2){
					Line2_prim.x2 = matchedPoints.at(i).MatchLICFs_R.x_xK + 1;
				}
				Line2_prim.y2 = -(aR2*Line2_prim.x2)/bR2 - cR2/bR2;// + Yo_R;

				lineCrossPointLine0 = GetLineIntersection(Line_Xk,Line_Xk_prim);
				lineCrossPointLine1 = GetLineIntersection(Line1,Line1_prim);
				lineCrossPointLine2 = GetLineIntersection(Line2,Line2_prim);

				// calculus using distance between l' and ex2' instead of intersections
				// for xk'
				Value.val[0] = matchedPoints.at(i).MatchLICFs_R.x_xK + Xo_R;
				Value.val[1] = matchedPoints.at(i).MatchLICFs_R.y_xK + Yo_R;

				do_L1 = abs(aR0*Value.val[0] + bR0*Value.val[1] + cR0)/sqrt(pow(aR0,2)+ pow(bR0,2));
				do_L2 = abs(aR0*lineCrossPointLine0.xi + bR0*lineCrossPointLine0.yi + cR0)/sqrt(pow(aR0,2)+ pow(bR0,2));
				// for ex'1
				dL1 = abs(aR1*Line1.x1 + bR1*Line1.y1 + cR1)/sqrt(pow(aR1,2)+ pow(bR1,2));
				dL3 = abs(aR1*lineCrossPointLine1.xi + bR1*lineCrossPointLine1.yi + cR1)/sqrt(pow(aR1,2)+ pow(bR1,2));
				// for ex'2
				dL2 = abs(aR2*Line2.x1 + bR2*Line2.y1 + cR2)/sqrt(pow(aR2,2)+ pow(bR2,2));
				dL4 = abs(aR2*lineCrossPointLine2.xi + bR2*lineCrossPointLine2.yi + cR2)/sqrt(pow(aR2,2)+ pow(bR2,2)); 


				// Corresponding points on the other image

				PointR.x = lineCrossPointLine0.xi;//matchedPoints.at(i).MatchLICFs_R.L_1.x_k + Xo_R;
				PointR.y = lineCrossPointLine0.yi;//matchedPoints.at(i).MatchLICFs_R.L_1.y_k + Yo_R;
				LICFs_pointsR.push_back(PointR); 

				PointR.x = lineCrossPointLine1.xi;//matchedPoints.at(i).MatchLICFs_R.L_1.x_farthest + Xo_R;
				PointR.y = lineCrossPointLine1.yi;//matchedPoints.at(i).MatchLICFs_R.L_1.y_farthest + Yo_R;
				LICFs_pointsR.push_back(PointR);

				PointR.x = lineCrossPointLine2.xi;//matchedPoints.at(i).MatchLICFs_R.L_2.x_farthest + Xo_R;
				PointR.y = lineCrossPointLine2.yi;//matchedPoints.at(i).MatchLICFs_R.L_2.y_farthest + Yo_R;
				LICFs_pointsR.push_back(PointR);

				// epipole_prim value: e'				  
				PointR.x = epipole_prim.ptr<double>(0)[0];
				PointR.y = epipole_prim.ptr<double>(1)[0];
				LICFs_pointsR.push_back(PointR);				

				// set M = xi transpose, which correspond to left image points
				
				M.ptr<double>(0)[0] = matchedPoints.at(0).MatchLICFs_L.x_xK + Xo_L;
				M.ptr<double>(1)[0] = matchedPoints.at(0).MatchLICFs_L.L_1.x_farthest + Xo_L;
				M.ptr<double>(2)[0] = matchedPoints.at(0).MatchLICFs_L.L_2.x_farthest + Xo_L;
				
				M.ptr<double>(0)[1] = matchedPoints.at(0).MatchLICFs_L.y_xK + Yo_L;
				M.ptr<double>(1)[1] = matchedPoints.at(0).MatchLICFs_L.L_1.y_farthest + Yo_L;
				M.ptr<double>(2)[1] = matchedPoints.at(0).MatchLICFs_L.L_2.y_farthest + Yo_L;
				// set 3rd value to 1 as homogeneous coordinates, isn't it?
				M.ptr<double>(0)[2] = 1;
				M.ptr<double>(1)[2] = 1;
				M.ptr<double>(2)[2] = 1;

				// set xi' right image matching points vector matrix
				
				xi_prim.ptr<double>(0)[0] = lineCrossPointLine0.xi;
				xi_prim.ptr<double>(1)[0] = lineCrossPointLine0.yi;
				xi_prim.ptr<double>(2)[0] = 1;
				
				xi_prim.ptr<double>(0)[1] = lineCrossPointLine1.xi;
				xi_prim.ptr<double>(1)[1] = lineCrossPointLine1.yi;
				xi_prim.ptr<double>(2)[1] = 1;
	
				xi_prim.ptr<double>(0)[2] = lineCrossPointLine2.xi;
				xi_prim.ptr<double>(1)[2] = lineCrossPointLine2.yi;
				xi_prim.ptr<double>(2)[2] = 1;				

			}
			// Now set b vector for complete equation knowledgement Mv = b

			cv::Mat xi = cv::Mat(3,3,cv::DataType<double>::type,cv::Scalar::all(0));
			cv::Mat Axi = cv::Mat(3,3,cv::DataType<double>::type,cv::Scalar::all(0));
			
			cv::Mat vectorProduct1 = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));
			cv::Mat vectorProduct2 = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));
			cv::Mat vectorProduct2copy = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));

			cv::Mat transpose_vectorProduct1 = cv::Mat(1,3,cv::DataType<double>::type,cv::Scalar::all(0));
			
			cv::Mat vector_xi = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));
			cv::Mat vector_Axi = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));
			cv::Mat vector_xi_prim = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));
			cv::Mat vector_e_prim = cv::Mat(3,1,cv::DataType<double>::type,cv::Scalar::all(0));//(3,1)
			
		
			double Magnitude_vectorProduct2 = 1;
			double bi = 1;
	
			// e'
			vector_e_prim.ptr<double>(0)[0] = e1;
			vector_e_prim.ptr<double>(1)[0] = e2;
			vector_e_prim.ptr<double>(2)[0] = e3;

			xi = M.t();
			Axi = A*xi;
				
			// get b finally
			for (int i = 0;i<3;i++){
				// x'i
				vector_xi_prim.ptr<double>(0)[0] = xi_prim.ptr<double>(0)[i];
				vector_xi_prim.ptr<double>(1)[0] = xi_prim.ptr<double>(1)[i];
				vector_xi_prim.ptr<double>(2)[0] = xi_prim.ptr<double>(2)[i];
				// Axi
				vector_Axi.ptr<double>(0)[0] = Axi.ptr<double>(0)[i];
				vector_Axi.ptr<double>(1)[0] = Axi.ptr<double>(1)[i];
				vector_Axi.ptr<double>(2)[0] = Axi.ptr<double>(2)[i];
				// find cross product  (x'i x Axi) and its transposed vector
				vectorProduct1 = vector_xi_prim.cross(vector_Axi);
				transpose_vectorProduct1 = vectorProduct1.t();
				// find second cross product (x'i x e')
				vectorProduct2 = vector_xi_prim.cross(vector_e_prim);
				vectorProduct2copy = vector_xi_prim.cross(vector_e_prim);
				// get the magnitude of cross product (x'i x e')
				Magnitude_vectorProduct2 = vectorProduct2copy.dot(vectorProduct2);
				// get the value for bi
				bi = vectorProduct1.dot(vectorProduct2);
				bi = bi/Magnitude_vectorProduct2;
				b.ptr<double>(i)[0] = bi;
				// set Zero values to matrices
				double factor = 0;
				vector_xi_prim = vector_xi_prim*factor;
				vector_Axi = vector_Axi*factor;
				vectorProduct1 = vectorProduct1*factor;
				vectorProduct1 = vectorProduct1*factor;

			}
			// Solve Mv = b
			int solving_vFlag = 0;
			solving_vFlag = cv::solve(M,b,v,cv::DECOMP_LU);
			// Finally solve for Homography matrix H = A - e'vT
			if (solving_vFlag == 1){// valid solution
				vT = v.t();
				e_prim_vT = vector_e_prim*vT;
				H = A - e_prim_vT;		
			}
		}
		// return homography found for this points
		// check H matrix error
		double H_error = CheckHomographyMatrix(LICFs_pointsL,LICFs_pointsR,H);
		std::cout<<"H matrix"<<endl<<H<<endl<<endl;
		return H;
	}
	catch(...){
	}
}

// Occlusion methods analysis
// Duchesne and Herve'05
// visibility test
Visibility_Status LICFs::visibility(IplImage *leftGrayImage,imagePoints &actualImages_Points,IplImage *OcclusionSubImage){
	try{
		int WindowsSizeW = 15;//9;
		int WindowsSizeH = 15;//7;
		double minVal = 0;
		double maxVal = 0;
		CvPoint* minLoc = &pt;
		CvPoint* maxLoc = &pt;
		float threshold = 0.95;// max difference in pixels
		float disparity_Shifment = actualImages_Points.disparityShifment;
		Visibility_Status vertexStatus = UNKNOWN;
		CvPoint2D32f currentVertexPosition,ImageToMatchCenter,CurrentLeftPoint;
		// Get the current points coordinates
		CurrentLeftPoint.x = actualImages_Points.xiL;
		CurrentLeftPoint.y = actualImages_Points.yiL; 
		

		CvMat *Pl = cvCreateMat(WindowsSizeH,WindowsSizeW,CV_32FC1);
		CvMat *ImageToMatch = cvCreateMat(0.1*OcclusionSubImage->height,0.2*OcclusionSubImage->width,CV_32FC1);
		CvMat *MatchResults = cvCreateMat(ImageToMatch->height - Pl->height + 1,
			ImageToMatch->width - Pl->width + 1,CV_32FC1);

		// OpenCV mapping new_y = (h-actualImages_Points.yiL)
		// because OpenGL has the origin at the top left of the image
		// and OpenGL has the origin at the Bottom left of the image
		int new_yiL = imageOriginal->height - actualImages_Points.yiL;
		int new_yiR = imageOriginal->height - actualImages_Points.yiR;

		// check if point are inside the image frontiers
		if ((actualImages_Points.xiR > imageOriginal->width)||(actualImages_Points.xiR < 0)){
			// vertex is outside
			vertexStatus = UNKNOWN;
			return vertexStatus;
		}else{
			// find match using template match and NCC within a 15x15 window
			// left point is the control point
			currentVertexPosition.x = actualImages_Points.xiL;
			currentVertexPosition.y = new_yiL;
			ImageToMatchCenter.x = actualImages_Points.xiR;
		    ImageToMatchCenter.y = new_yiR;

			cvGetRectSubPix(leftGrayImage,Pl,currentVertexPosition);
			cvGetRectSubPix(OcclusionSubImage,ImageToMatch,ImageToMatchCenter);
			cvMatchTemplate(ImageToMatch,Pl,MatchResults,CV_TM_CCORR_NORMED);
			//MatchCensusTemplate(ImageToMatch,Pl,MatchResults);
			//cvNormalize(MatchResults,MatchResults,1);
			cvMinMaxLoc(MatchResults,&minVal,&maxVal,minLoc,maxLoc);

			if (maxVal > threshold){
				// match is found, find out if this point is visible or invisible
				float VisibilityLimit = (actualImages_Points.xiR - 0.5*MatchResults->width) + maxLoc->x;
 				if ( VisibilityLimit > actualImages_Points.xiR){	// VISIBLE, outward window
					vertexStatus = VISIBLE;		// YELLOW
					return vertexStatus;	
				}else if (VisibilityLimit < actualImages_Points.xiR){	// INVISIBLE, inward window
					vertexStatus = INVISIBLE;	// ORANGE
					return vertexStatus;
				}

			}else{
				// match not found the set to unknown
				vertexStatus = UNKNOWN;			// GREEN
				return vertexStatus;
			}
		}
	
	}
	catch(...){
	}
}
// visibility match using a 9x7 Census Transform
void LICFs::MatchCensusTemplate(CvMat *ImageToSearch,CvMat *Feature,CvMat *Results){
	try{
		CvPoint2D32f currentCenter;
		int hammingDistance = 0;
		int sizeString = Feature->width*Feature->height - 1;
		CvMat *bitStringFeature =  cvCreateMat(1,sizeString,CV_32FC1);
		CvMat *bitStringTemplate =  cvCreateMat(1,sizeString,CV_32FC1);
		CvMat *bitStringCurrentHamming =  cvCreateMat(1,sizeString,CV_32FC1);
		CvMat *tmpResult = cvCreateMat(Results->height, Results->width,CV_32FC1);
		CvMat *Template = cvCreateMat(Feature->height,Feature->width,CV_32FC1);
		// find census transform
		int shiftX = 5;//(int)floor(Feature->width/2)) + 1;
		int shiftY = 4;//(int)floor(Feature->heigh/2)) + 1;
		cvSetZero(Results);
		bitStringFeature = CensusTransform(Feature);
		for(int i=0;i<Results->width;i++){
			for(int j=0;j<Results->height;j++){
				// get current analysis area
				currentCenter.x = i + shiftX;
				currentCenter.y = j + shiftY;
				cvGetRectSubPix(ImageToSearch,Template,currentCenter);
				bitStringTemplate = CensusTransform(Template);
				// compare with feature and save the hamming distance
				cvXor(bitStringFeature,bitStringTemplate,bitStringCurrentHamming);
				hammingDistance = cvCountNonZero(bitStringCurrentHamming);
				cvmSet(Results,j,i,hammingDistance);
			}
		}
	}
	catch(...){
	
	}
}
// this method calculates the census transform for one image
// according to Humenberger paper 
// "A census-based stereo vision algorithm using modified 
// semi-global matching and plane-fitting to improve matching quality"
// E(p1,p2) = 0 if p1 <= p2
//			= 1 if p1 > p2
// where p1 = current pixel
//		 p2 = central pixel
CvMat* LICFs::CensusTransform(CvMat *Image){
	try{
		int sizeString =Image->width*Image->height - 1; 
		float indexValues_9x7[] = {0,0,1,0,2,0,3,0,4,0,5,0,6,0,7,0,8,0,
							8,1,8,2,8,3,8,4,8,5,8,6,
							7,6,6,6,5,6,4,6,3,6,2,6,1,6,0,6,
							0,5,0,4,0,3,0,2,0,1,
							1,1,2,1,3,1,4,1,5,1,6,1,7,1,
							7,2,7,3,7,4,7,5,
							6,5,5,5,5,4,5,3,5,2,5,1,
							1,4,1,3,1,2,
							2,2,2,3,2,4,2,5,2,6,
							6,3,6,4,
							5,4,4,4,4,3,4,2,
							2,3,3,3,3,5};

		//CvMat *CensusIndex;
		CvMat *StringBits = cvCreateMat(1,sizeString,CV_32FC1);
		CvMat *CensusImage = cvCloneMat(Image);
		cvSetZero(StringBits);
		cvSetZero(CensusImage);
		CvMat CensusIndex;
		cvInitMatHeader(&CensusIndex,1,sizeString,CV_32FC2,indexValues_9x7);
		int center_x,center_y;
		//int n1=0,n2=0,m1=0,m2=0;
		int x=0,y=0;
		//int bitPosition = 0;
		//int levelClockWise = 0;
		double valueString = 0;
		double p1=0,p2=0;
		//float divFactor=1;		
		center_x = 5;//int(floor(Image->width/2)) + 1;
		center_y = 4;//int(floor(Image->height/2)) + 1;
		//int levelClockWiseLimit = center_y - 1;
		// Examine values
		p2 = cvmGet(Image,center_y,center_x);
		CvScalar indexValue;
		for (int i = 0;i < sizeString;i++){
			indexValue = cvGet2D(&CensusIndex,0,i);
			x = indexValue.val[0];
			y = indexValue.val[1];
			p1 = cvmGet(Image,y,x);
			if (p1 <= p2){
					// image position already is zero
					valueString = 0;
			}else if(p1 > p2){
					cvmSet(CensusImage,y,x,1.0);
					valueString = 1;
			}
			// set current string value
			cvmSet(StringBits,0,i,valueString);			
		}
		//// clockwise reading 
		//int angleStep = 9;
		//for (int angle = 0; angle < 360;angleStep){
		//	switch(angle){
		//		case(0 <= angle < 90):
		//			angleStep = Image->width;
		//			divFactor= (90/Image->width);
		//			x =  (int)floor(sin(angle)*divFactor);
		//			y = levelClockWise;
		//			p1 = cvmGet(Image,x,y);					
		//			break;
		//		case(90 < angle < 180):
		//			break;
		//		case(180 < angle < 270):
		//			break;
		//		case(270 < angle < 360):
		//			break;
		//	}
			
		//}
		return StringBits;
	}
	catch(...){
	}
}

//#include "LICFs.moc"

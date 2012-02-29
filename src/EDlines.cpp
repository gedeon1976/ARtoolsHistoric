#include "EDlines.h"

EDlines::EDlines(void){

}

EDlines::EDlines(IplImage *GrayImage)
{
	// Initialize variables
	// get image to process
	InputImage = cvCloneImage(GrayImage);
	imgSize.width = InputImage->width;
	imgSize.height = InputImage->height;

	//imageToProcess = cvCreateMat(imgSize.height,imgSize.width,CV_32FC1);
	//SmoothedImage = cvCreateMat(imgSize.height,imgSize.width,CV_32FC1);
	GradientImage = cvCreateMat(imgSize.height,imgSize.width,CV_32FC1);
	AnchorsImage = cvCreateMat(imgSize.height,imgSize.width,CV_32FC1);
	EdgeImage = cvCreateMat(imgSize.height,imgSize.width,CV_32FC1);;
	GradientMap = cvCreateMat(imgSize.height,imgSize.width,CV_32FC3);
	cvSetZero(EdgeImage);cvSetZero(AnchorsImage);
	cvSetZero(GradientMap);
}

EDlines::~EDlines(void)
{
	cvReleaseImage(&InputImage);
	cvReleaseImage(&imageToProcess);
	cvReleaseMat(&SmoothedImage);
	cvReleaseMat(&GradientImage);
	cvReleaseMat(&AnchorsImage);
	cvReleaseMat(&EdgeImage);
	cvReleaseMat(&GradientMap);
	

}
// this method smooth the input image using
// a gaussian kernel filter with sigma = 1
void EDlines::SmoothImage(int WindowsSize, float sigma){
	try{
		CvMat imgHeader,*dstMat,*SmoothedImageTmp;
		int coi = 0;		
		imageToProcess = cvCloneImage(InputImage);
		cvSmooth(InputImage,imageToProcess,CV_GAUSSIAN,WindowsSize,0,sigma);
		dstMat = cvGetMat(imageToProcess,&imgHeader,&coi,0);
		SmoothedImage = cvCloneMat(&imgHeader);	
		
		}
	catch(...){
	}
}
// get the Smoothed Image
CvMat* EDlines::getSmoothImage(void){
	try{
		CvMat* img = cvCloneMat(SmoothedImage);
		return img;
	}
	catch(...){
	}
}
// Find the gradient for each point of the image 
// and refine after checking values bigger that gradient_Threshold
void EDlines::ApplyImageGradient(int WindowsSize,double gradient_Threshold){
	try{
		// find image gradient
		CvScalar values,FilteredValues;
		float pixelValue,G,Gx,Gy,direction=0;
		Gradient_Properties currentEdgePoint;
		CvMat *GradientImageTmp = cvCreateMat(imgSize.height,imgSize.width,CV_32FC1);
		CvMat *GxImageTmp = cvCreateMat(imgSize.height,imgSize.width,CV_32FC1);
		CvMat *GyImageTmp = cvCreateMat(imgSize.height,imgSize.width,CV_32FC1);
		CvMat *edgelImageTmp = cvCreateMat(imgSize.height,imgSize.width,CV_32FC1);
		cvSetZero(GradientImageTmp);
		// find original gradient
		//cvSobel(SmoothedImage,GradientImageTmp,1,1,WindowsSize);

		// Find the gradient map using a mask of 2x2
		int limit_i = SmoothedImage->height - 2;
		int limit_j = SmoothedImage->width - 2;
		for (int i=0;i<limit_i;i++){
			for(int j=0;j<limit_j;j++){
					// read the pixels values
					currentEdgePoint = GetImagePointGradient(SmoothedImage,i,j);
					// gradient on X
					Gx = currentEdgePoint.gradientMagnitudeX;
					// gradient on Y
					Gy = currentEdgePoint.gradientMagnitudeY;
					// current pixel gradient
					G = currentEdgePoint.gradientMagnitude;
					
					// filter Gradient > gradientThreshold
					if (G > gradient_Threshold ){
						// Direction of pixel
						direction = currentEdgePoint.direction;
						// Set point on the Gradient image
						cvmSet(GradientImage,i,j,G);
						//pixelValue = cvmGet(GradientImageTmp,i,j);
						// Save the data on the  Gradient Map, Direction Map and Edge Map					
						values.val[0] = G;
						values.val[1] = direction;
						values.val[2] = 0;			// false edge pixel
						cvSet2D(GradientMap,i,j,values);
					}
					
				}
		}
		// apply gradient threshold to eliminate weaker pixels and non-edge pixels
		//cvSplit(GradientMap,GradientImageTmp,GxImageTmp,GyImageTmp,edgelImageTmp);
		//cvThreshold(GradientImageTmp,GradientImage,gradient_Threshold,255,CV_THRESH_TOZERO);
		//cvNamedWindow("ED gradientTmp");
		//cvShowImage("ED gradient",GradientImageTmp);
		cvNamedWindow("ED gradient");
		cvShowImage("ED gradient",GradientImage);
		cvNamedWindow("ED gradientMap");
		cvShowImage("ED gradientMap",GradientMap);
				
	}
	catch(...)
	{
	}
}
// get the Gradient Image
CvMat* EDlines::getGradientImage(void){
	try{
		CvMat* img = cvCloneMat(GradientImage);
		return img;
	}
	catch(...){
	}
}
// get the gradient for a specific position on the image
Gradient_Properties EDlines::GetImagePointGradient(CvMat *image, int x, int y){
	try{
		Gradient_Properties pixelProperties;
		double G,direction = 0;
		double Gx,Gy = 0;
		CvScalar values;
		double Ixy,Ix1y,Ixy1,Ix1y1=0;
		int y1,x1 = 0;
		// default values
		pixelProperties.x = x;
		pixelProperties.y = y;
		pixelProperties.gradientMagnitude = 0;
		pixelProperties.gradientMagnitudeX = 0;
		pixelProperties.gradientMagnitudeY = 0;
		pixelProperties.direction = 45;

		if (((x>=0)&(x<image->height-2))&((y>=0)&(y<image->width-2))){
			// read the pixels values
			values = cvGet2D(image,x,y);Ixy = values.val[0];
			x1 = x + 1;
			values = cvGet2D(image,x1,y);Ix1y = values.val[0];
			y1 = y + 1;
			values = cvGet2D(image,x,y1);Ixy1  = values.val[0];
			values = cvGet2D(image,x1,y1);Ix1y1 = values.val[0];
			// gradient on X
			Gx = 0.5*((Ix1y - Ixy)+(Ix1y1- Ixy1));
			// gradient on Y
			Gy = 0.5*((Ixy1 - Ixy)+(Ix1y1 - Ix1y));
			// Direction of pixel
			if(abs(Gx)>=abs(Gy)){
				direction = 0;	// belong to a horizontal edge
			}else{
				direction = 90; // belong to a vertical edge
			}
			// get gradient magnitude
			G = sqrt(pow(Gx,2) + pow(Gy,2));
			// return value
			pixelProperties.x = x;
			pixelProperties.y = y;
			pixelProperties.gradientMagnitude = G;
			pixelProperties.gradientMagnitudeX = Gx;
			pixelProperties.gradientMagnitudeY = Gy;
			pixelProperties.direction = direction;
		}
	// default value = 0
	return pixelProperties;
	}
	catch(...){
	}
}
// Smart routing methods
void EDlines::GoTroughRoute(Route path,Orientation EdgeDirection,int x,int y,double PixelGradient,double PixelDirection,double PixelEdgel){
	try{
		// explore left side
		EdgePoint currentEdgel;
		//EdgeChain currentEdgeSegment;
		PixelChain actualEdgeSegment;
		// gradient of neigbors pixels
		float G_left_Up,G_left_Middle,G_left_Down;
		float G_right_Up,G_right_Middle,G_right_Down;
		float G_up_Left,G_up_Middle,G_up_Right;
		float G_down_Left,G_down_Middle,G_down_Right;
		float currentPixelGradient,currentPixelEdgel;
		int currentPixelDirection;
		CvScalar values,valuesEdge;
		valuesEdge.val[0] = 255;
		valuesEdge.val[1] = 0;
		valuesEdge.val[2] = 0;
		valuesEdge.val[3] = 0;
		Orientation EdgeOrientation;
		// check edge direction horizontal or vertical
		if (EdgeDirection == Orientation::HORIZONTAL){
			EdgeOrientation = Orientation::HORIZONTAL;
		}else{		
			EdgeOrientation = Orientation::VERTICAL;
		}
		currentPixelDirection = (int)PixelDirection;
		currentPixelGradient = PixelGradient;
		currentPixelEdgel = PixelEdgel;

		while((currentPixelGradient > 0)&
			  (currentPixelDirection == EdgeOrientation)&
			  (currentPixelEdgel != 1)){
			 
				 
			  if (((x-1)>=0)&((y-1)>=0)&(y<(GradientImage->height-2))&(x<(GradientImage->width-2))){
				   // mark pixel as visited and as an edge pixel
				  values.val[2] = 1;
				  cvSet2D(GradientMap,y,x,values);
				  // Set point on the edge image
				  valuesEdge.val[0] = 255;
				  cvSet2D(EdgeImage,y,x,valuesEdge);
				  // save point to current line segment
				  currentEdgel.x = x;
				  currentEdgel.y = y;
				  actualEdgeSegment.push_back(currentEdgel);
				  // look for route direction
				  switch(EdgeOrientation){
					  case Orientation::HORIZONTAL:
						  switch(path){
							  // LEFT SIDE
							  case Route::LEFT_ROUTE:
								  G_left_Up = cvmGet(GradientImage,y-1,x-1);
								  G_left_Middle = cvmGet(GradientImage,y,x-1);
								  G_left_Down = cvmGet(GradientImage,y+1,x-1);
								  // check bigger
								  if ((G_left_Up > G_left_Middle)&(G_left_Up > G_left_Down)){// up is the bigger
									x = x-1;
									y = y-1;
								  }else if((G_left_Down > G_left_Up)&(G_left_Down >= G_left_Middle)){// down is the bigger
									x = x-1;
									y = y+1;
								  }else{// middle in other cases the bigger
									x = x-1;
									y = y;
								  }
							  break;
							  // RIGHT SIDE
							  case Route::RIGHT_ROUTE:
								  G_right_Up = cvmGet(GradientImage,y-1,x+1);
								  G_right_Middle = cvmGet(GradientImage,y,x+1);
								  G_right_Down = cvmGet(GradientImage,y+1,x+1);
								  // check bigger
								  if ((G_right_Up > G_right_Middle)&(G_right_Up > G_right_Down)){// up is the bigger
									x = x+1;
									y = y-1;
								  }else if((G_right_Down > G_right_Up)&(G_right_Down >= G_right_Middle)){// down is the bigger
									x = x+1;
									y = y+1;
								  }else{// middle in other cases the bigger
									x = x+1;
									y = y;
								  }
							  break;
						  }// end internal switch 
						  break;
					  case Orientation::VERTICAL:
						  switch(path){
							  // UP SIDE
								case Route::UP_ROUTE:
								  G_up_Left = cvmGet(GradientImage,y-1,x-1);
								  G_up_Middle = cvmGet(GradientImage,y-1,x);
								  G_up_Right = cvmGet(GradientImage,y-1,x+1);
								  // check bigger
								  if ((G_up_Left > G_up_Middle)&(G_up_Left > G_up_Right)){// left is the bigger
									x = x-1;
									y = y-1;
								  }else if((G_up_Right > G_up_Left)&(G_up_Right >= G_up_Middle)){// right is the bigger
									x = x+1;
									y = y-1;
								  }else{// middle up in other cases is the bigger
									x = x;
									y = y-1;
								  }
							  break;
							  // DOWN SIDE
								case Route::DOWN_ROUTE:
								  G_down_Left = cvmGet(GradientImage,y+1,x-1);
								  G_down_Middle = cvmGet(GradientImage,y+1,x);
								  G_down_Right = cvmGet(GradientImage,y+1,x+1);
								  // check bigger
								  if ((G_down_Left > G_down_Middle)&(G_down_Left > G_down_Right)){// left is the bigger
									x = x-1;
									y = y+1;
								  }else if((G_down_Right > G_down_Left)&(G_down_Right >= G_down_Middle)){// right is the bigger
									x = x+1;
									y = y+1;
								  }else{// middle down in other cases is the bigger
									x = x;
									y = y+1;
								  }
							  break;
						  }// end internal switch 
						  break;
				  }// end external switch

			  }
			// get properties of next pixel
			if (((x-1)>=0)&((y-1)>=0)&(y<(GradientImage->height-2))&(x<(GradientImage->width-2))){

				values = cvGet2D(GradientMap,y,x);			
				currentPixelGradient = values.val[0];
				currentPixelDirection = values.val[1];
				currentPixelEdgel = values.val[2];
				//out_ofBounds = false;
			}
			// CHECK limits
			if((x<=0)|(y<=0)|(x>=(GradientImage->width-2))|(y>=(GradientImage->height-2))){
				currentPixelGradient = -1;
			}				  

		}// end while
		// SAVE CHAINS OF PIXELS bigger that 4 pixels
		if (actualEdgeSegment.size()>=5){
			FoundLineSegments.push_back(actualEdgeSegment);
		}

	}
	catch(...){
	}

}
// Find the Anchors of the edges of the image
std::vector<Anchors_ED> EDlines::FindEdgeAnchors(int ScanInterval, float AnchorThreshold){
	try{
		Anchors_ED currentEdgePoint;
		std::vector<Anchors_ED> AnchorsList;
		bool isAnchor = false;
		float pixelValue,direction=0;
		float G,Gup,Gdown,Gleft,Gright = 0;
		CvScalar values;
		
		// Find the gradient map using a mask of 2x2
		for (int i=1;i<GradientImage->height - 1;i+=ScanInterval){
			for(int j=1;j<GradientImage->width - 1;j+=ScanInterval){
				//  check if the pixel has a value > 0
				pixelValue = cvmGet(GradientImage,i,j);
				if (pixelValue!=0){
					// current pixel gradient
					G = cvmGet(GradientImage,i,j);
					// Direction of pixel
					values = cvGet2D(GradientMap,i,j);
					direction = values.val[1];
					// check for anchors, actual details = 1 pixel
					Gup = cvmGet(GradientImage,i,j-1);
					Gdown = cvmGet(GradientImage,i,j+1);
					Gleft = cvmGet(GradientImage,i-1,j);
					Gright = cvmGet(GradientImage,i+1,j);

					if (direction == 0){// horizontal, compare with up and down pixels						
						if (((G - Gdown)>= AnchorThreshold)&((G - Gup)>= AnchorThreshold)){
							isAnchor = true;
						}
					}else if (direction == 90){	// vertical, compare with left and right pixels
						if(((G - Gleft)>= AnchorThreshold)&((G - Gright)>= AnchorThreshold)){
							isAnchor = true;
						}
					}
					// save Anchor data
					if (isAnchor == true){
						currentEdgePoint.x = j;
						currentEdgePoint.y = i;
						currentEdgePoint.is_inEdgel_Map = false;
						currentEdgePoint.is_Anchor = true;
						AnchorsList.push_back(currentEdgePoint);
						isAnchor = false;
					}
					
				}

			}
		}

	// return Anchors found	
	for (int i=0;i<AnchorsList.size();i++){
		cvCircle(AnchorsImage,cvPoint(AnchorsList.at(i).x,AnchorsList.at(i).y),2,CV_RGB(255,128,128));
	}
	cvNamedWindow("ED Anchors");
	cvShowImage("ED Anchors",AnchorsImage);
	return AnchorsList;
	}
	catch(...)
	{
	}
}
// Find the corresponding edges from the Anchor lists
void EDlines::EdgeLinking(std::vector<Anchors_ED> Anchors){
	try{
		// get the size of pixels to look	
		Route path;
		Orientation EdgeOrientation;
		float currentPixelDirection,currentPixelGradient,currentPixelEdgel = 0;
		bool out_ofBounds = false;
		int x,y;
		CvScalar values,valuesEdge;
		valuesEdge.val[0] = 255;
		valuesEdge.val[1] = 0;
		valuesEdge.val[2] = 0;
		valuesEdge.val[3] = 0;
		float N_Anchors = Anchors.size();
		for (int i=0;i < N_Anchors;i++){
			// check walk conditions
			//	1. pixels gradient > 0
			//  2. direction doesn't change
			//  3. pixel has not been detected before
			x = Anchors.at(i).x;
			y = Anchors.at(i).y;
			// check limits for x and y
			if ((x<0)&(y<0)&(y>(GradientImage->height-2))&(x>(GradientImage->width-2))){
				out_ofBounds = true;
			}else{
				values = cvGet2D(GradientMap,y,x);			
				currentPixelGradient = values.val[0];
				currentPixelDirection = values.val[1];
				currentPixelEdgel = values.val[2];
				/// SEARCH FOR HORIZONTAL EDGES
				// explore left side
				EdgeOrientation = Orientation::HORIZONTAL;
				path = Route::LEFT_ROUTE;
				GoTroughRoute(path,EdgeOrientation,x,y,currentPixelGradient,currentPixelDirection,currentPixelEdgel);	
				// explore right side			
				path = Route::RIGHT_ROUTE;			
				GoTroughRoute(path,EdgeOrientation,x,y,currentPixelGradient,currentPixelDirection,currentPixelEdgel);
				// SEARCH FOR VERTICAL EDGES
				EdgeOrientation = Orientation::VERTICAL;
				// explore the up side
				path = Route::UP_ROUTE;			
				GoTroughRoute(path,EdgeOrientation,x,y,currentPixelGradient,currentPixelDirection,currentPixelEdgel);
				// explore the down side
				path = Route::DOWN_ROUTE;			
				GoTroughRoute(path,EdgeOrientation,x,y,currentPixelGradient,currentPixelDirection,currentPixelEdgel);
			}		
		}//end for Anchors
		//PixelChain currentPixelChain;
		//EdgePoint currentPixelPoint;
		//// DRAW FILTERED EDGE IMAGE WHERE LINE SEGMENTS >= 5 pixels
		//for (int i= 0; i < FoundLineSegments.size();i++){
		//	currentPixelChain =FoundLineSegments.at(i); 
		//	for (int j = 0; j <currentPixelChain.size();j++){
		//		currentPixelPoint = currentPixelChain.at(j);
		//		x = currentPixelPoint.x;
		//		y = currentPixelPoint.y;
		//		// Set point on the edge image
		//		valuesEdge.val[0] = 255;
		//		cvSet2D(EdgeImage,y,x,valuesEdge);
		//	}
		//}

	// draw results
	//cvNamedWindow("ED edge");
	//cvShowImage("ED edge",EdgeImage);
	}
	catch(...){
		
	}
}
// find Edges from an gray image
IplImage* EDlines::EdgeDrawing(IplImage *edgeDst,int WindowSize,double gradient_Threshold){
	try{
		std::vector<Anchors_ED> anchorsList;
		int scanInterval = 1; 
		float AnchorThreshold = 0.5, sigma = 1;
		SmoothImage(WindowSize,sigma);
		ApplyImageGradient(WindowSize,gradient_Threshold);
		anchorsList = FindEdgeAnchors(scanInterval,AnchorThreshold);
		EdgeLinking(anchorsList);
		// return internal calculed edge image
		IplImage header,*dstImg;
		IplImage *dstImg2  = cvCreateImage(imgSize,IPL_DEPTH_8U,1);
		dstImg = cvGetImage(EdgeImage,&header);
		cvConvertScaleAbs(dstImg,dstImg2);
		return dstImg2;
	}
	catch(...){
	}
}
// Compute distance between a line and a point
double EDlines::ComputePointDistance2Line(lineParameters lineEquation, EdgePoint point){
	try{
		double distance;
		float a = lineEquation.a;
		float b = lineEquation.b;
		float c = lineEquation.c;

		distance = abs(a*point.x + b*point.y + c)/sqrt(pow(a,2)+pow(b,2));
		return distance;
	}
	catch(...){
	}
}
// Find the line Equation that best fits to the pixel chain of points
// the code is based on the book Matrix Analysis by Carl D. Meyer section 4.6
void EDlines::LeastSquareLineFit(PixelChain pixels,int MinLineLength,lineParameters &lineEquation,
								 double &lineFitErrorValue,Orientation lineType, int startPixel){
	try{
		// it finds an equation in the slope-intercept form using least square fitting method
		// y = alpha + beta*x from the input pair points
		// where alpha = intercept
		//       beta = slope
		// see http://www.purplemath.com/modules/strtlneq.htm for details
		int m,n,rankA,limit;
		float alpha,beta;
		//double Error;
		CvMat *A,*A_Transposed,*AtA,*AtA_Inv,*Atb,*b,*x;
		CvMat  *Ax,*Ax_b,*Ax_bt,*lineFitError;
		m = MinLineLength;
		n = 2;	// only 2 columns
		// matrices creation
		A = cvCreateMat(m,n,CV_32FC1);
		A_Transposed = cvCreateMat(n,m,CV_32FC1);
		AtA = cvCreateMat(2,2,CV_32FC1);
		AtA_Inv = cvCreateMat(2,2,CV_32FC1);
		Atb = cvCreateMat(2,1,CV_32FC1);
		Ax = cvCreateMat(m,1,CV_32FC1);
		Ax_b = cvCreateMat(m,1,CV_32FC1);
		Ax_bt = cvCreateMat(1,m,CV_32FC1);
		b = cvCreateMat(m,1,CV_32FC1);
		x = cvCreateMat(2,1,CV_32FC1); 
		lineFitError = cvCreateMat(1,1,CV_32FC1);
		// build Matrices
		limit = m + startPixel;
		if (limit > pixels.size()){
			limit = pixels.size(); 
		}
		for (int i = 0;i < m; i++){
			cvmSet(A,i,0,1.0);
			if (lineType == Orientation::HORIZONTAL){	// Do a least squares taking pairs as (x,y)
				if ((i + startPixel)< pixels.size()){
					cvmSet(A,i,1,pixels.at(i + startPixel).x);	// x values
					cvmSet(b,i,0,pixels.at(i + startPixel).y);	// y values
				}
			}
			if (lineType == Orientation::VERTICAL){
				if ((i + startPixel)< pixels.size()){// Do a least squares taking pairs as (y,x)
					cvmSet(A,i,1,pixels.at(i + startPixel).y);	// x values
					cvmSet(b,i,0,pixels.at(i + startPixel).x);	// y values
				}
			}
		}		
		cvTranspose(A,A_Transposed);
		cvMatMul(A_Transposed,A,AtA);
		cvInvert(AtA,AtA_Inv,CV_LU);
		cvMatMul(A_Transposed,b,Atb);
		// solution for x
		cvMatMul(AtA_Inv,Atb,x);
		// calculate the least square error	
		cvMatMul(A,x,Ax);
		cvSub(Ax,b,Ax_b);
		cvTranspose(Ax_b,Ax_bt);
		cvMatMul(Ax_bt,Ax_b,lineFitError);
		// set values for lineEquation
		alpha = cvmGet(x,0,0);
		beta = cvmGet(x,1,0);
		// set the adequate values on the line equation
		// according to line orientation
		if (lineType == Orientation::HORIZONTAL){	// Do a least squares taking pairs as (x,y)
				lineEquation.a = -beta;
				lineEquation.b = 1.0;
				lineEquation.c = -alpha; 
		}else{									// Do a least squares taking pairs as (y,x)
				lineEquation.a = -1;
				lineEquation.b = beta;
				lineEquation.c = alpha; 
		}
		// set other values of line Equation
		lineEquation.x1 = pixels.at(startPixel).x;
		lineEquation.y1 = pixels.at(startPixel).y;
		lineEquation.x2 = pixels.at(limit-1).x;
		lineEquation.y2 = pixels.at(limit-1).y;
		// return error
		lineFitErrorValue = cvmGet(lineFitError,0,0);
 			
	}
	catch(...){
	}
}
// Algorithm to extract line segments from a pixel chain
std::vector<lineParameters> EDlines::LineFit(PixelChain pixels,int MinLineLength, double &leastSquareError, Orientation lineType){
	try{
		std::vector<lineParameters> lineSegments;
		double lineFitError = 1000;		// current line fit error
		lineParameters lineEquation;	// y = ax + b or x = ay + b
		int noPixels = pixels.size();
		int lineLen = MinLineLength; 
		int StartPixel = 0;				// start pixel  position, used to explore all
										// the chain pixels for several segments
		EdgePoint currentPoint;
		double distance;
		int ErrorLimitCounter = 0;
		
		while(lineLen < noPixels){

			LeastSquareLineFit(pixels,MinLineLength,lineEquation,lineFitError,lineType,StartPixel);
								
			if(lineFitError > 1.0){			// no initial line segment, returns
				//leastSquareError = 1000;
				//ErrorLimitCounter = ErrorLimitCounter + 1;
				StartPixel = MinLineLength + StartPixel;
				lineLen = StartPixel;
			}else{	
				// O.k An initial line segment detected
				// try to extend the line segment detected
				while(lineLen < noPixels){
					currentPoint = pixels.at(lineLen);
					distance = ComputePointDistance2Line(lineEquation,currentPoint);
					if (distance > 1.0){		// max take 1 pixel from the line
						break;
					}
					lineLen++;
				}// end while

				// end of the current line segment, Computes the final line equation and
				// returns it
				LeastSquareLineFit(pixels,lineLen,lineEquation,leastSquareError,lineType,StartPixel);
				lineSegments.push_back(lineEquation);
				StartPixel = lineLen;
			}

		}
		return lineSegments;

	}
	catch(...){
	}

}
// Find the lines on the current image
std::vector<lineParameters> EDlines::EdgeFindLines(int MinimalLength){
	try{		
		
		double LineFitError,Error;
		lineParameters lineEquation;
		PixelChain currentPixelChain;
		std::vector<lineParameters> LineSegmentsHor,LineSegmentsVer,LineSegments;
		std::vector<lineParameters>::iterator it;
		Orientation lineType;

		int MinLineLength = MinimalLength;
		int Nlines = FoundLineSegments.size();
		for (int i=0;i < Nlines;i++){
			currentPixelChain = FoundLineSegments.at(i);
			// detect horizontal lines
			lineType = Orientation::HORIZONTAL;
			LineSegmentsHor = LineFit(currentPixelChain,MinLineLength,Error,lineType);
			if (Error != 1000){
				it = LineSegments.begin();
				LineSegments.insert(it,LineSegmentsHor.begin(),LineSegmentsHor.end());
			}
			// detect vertical lines
			lineType = Orientation::VERTICAL;
			LineSegmentsVer = LineFit(currentPixelChain,MinLineLength,Error,lineType);
			if (Error != 1000){
				it = LineSegments.begin();
				LineSegments.insert(it,LineSegmentsVer.begin(),LineSegmentsVer.end());
			}
		}	
    // draw lines
   /* IplImage *Lines;
	Lines = cvCreateImage(imgSize,IPL_DEPTH_8U,1);
	for(int i=0;i<LineSegments.size();i++){
		cvLine(Lines,cvPoint(LineSegments.at(i).x1,LineSegments.at(i).y1),cvPoint(LineSegments.at(i).x2,LineSegments.at(i).y2),CV_RGB(255,0,0));
	}
	cvNamedWindow("ED lines");
	cvShowImage("ED lines",Lines);*/
	return LineSegments;
	}
	catch(...){
	} 
}
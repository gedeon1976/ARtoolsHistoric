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
	GradientImage = cv::Mat(imgSize.height,imgSize.width,CV_8U,cv::Scalar::all(0));
	GradientImageOriginal = cv::Mat(imgSize.height,imgSize.width,CV_32FC1,cv::Scalar::all(0));
	AnchorsImage = cv::Mat(imgSize.height,imgSize.width,CV_32FC1,cv::Scalar::all(0));
	EdgeImage = cv::Mat(imgSize.height,imgSize.width,CV_32FC1,cv::Scalar::all(0));
	GradientMap = cv::Mat(imgSize.height,imgSize.width,CV_8UC3,cv::Scalar::all(0));
	
}

EDlines::~EDlines(void)
{
	cvReleaseImage(&InputImage);
	cvReleaseImage(&imageToProcess);

}
// this method smooth the input image using
// a gaussian kernel filter with sigma = 1
void EDlines::SmoothImage(int WindowsSize, float sigma){
	try{
		//CvMat imgHeader,*dstMat,*SmoothedImageTmp;
		//int coi = 0;	
		cv::Mat blurredImage,SmoothedSharpen,FilteredSmooth;
		imageToProcess = cvCloneImage(InputImage);
		cvSmooth(InputImage,imageToProcess,CV_GAUSSIAN,WindowsSize,0,sigma);
		//dstMat = cvGetMat(imageToProcess,&imgHeader,&coi,0);  // C API
		//SmoothedImage = cvCloneMat(&imgHeader);
		SmoothedImage = cv::cvarrToMat(imageToProcess,true);
		cv::blur(SmoothedImage,blurredImage,cv::Size(3,3));
		// unsharp filter to improve edges
		SmoothedSharpen = SmoothedImage - blurredImage;
		cv::addWeighted(SmoothedImage,1,SmoothedSharpen,2,0,FilteredSmooth);
		cv::bilateralFilter(FilteredSmooth,SmoothedImage,3,1000,1000);
		cv::imshow("Smoothed Image",SmoothedImage);	
		cv::imshow("Sharpen Image",SmoothedSharpen);	
		}
	catch(...){
	}
}
// get the Smoothed Image
cv::Mat EDlines::getSmoothImage(void){
	try{
		cv::Mat img = SmoothedImage.clone();
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
		cv::Vec3b values;
		float pixelValue,G,Gx,Gy;
		double minVal, maxVal;
		int direction = 0;
		Gradient_Properties currentEdgePoint;
		cv::Mat GradientDirectionHor = cv::Mat(imgSize.height,imgSize.width,CV_32FC1,cv::Scalar::all(0));
		cv::Mat GradientDirectionVert = cv::Mat(imgSize.height,imgSize.width,CV_32FC1,cv::Scalar::all(0));
		cv::Mat GradientEdgels = cv::Mat(imgSize.height,imgSize.width,CV_8U,cv::Scalar::all(0));
		cv::Mat FilteredGradientA,FilteredGradientB,FilteredGradientC,FilterTmp,GradientMapDirection;
		cv::Mat GradientMapHor,GradientMapVert,GradientMapTmp,FilterTmp2, BlurHor,BlurVert;
		cv::Mat FilterDirectionA,FilterDirectionB,FilterErode,FilterErodeB,FilterErodeC,FilterDilate,BlurImage,BlurImageB;
		cv::Mat FilterA;
		std::vector<cv::Mat> GradientMapChannels;
		
		// Find the gradient map using a mask of 2x2
		int limit_i = imgSize.height - 2;
		int limit_j = imgSize.width - 2;
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
						direction = (int)currentEdgePoint.direction;
						switch(direction){
							case 0:
								GradientDirectionHor.ptr<float>(i)[j] = 255;
								GradientDirectionVert.ptr<float>(i)[j] = 0;
								break;
							case 90:
								GradientDirectionHor.ptr<float>(i)[j] = 0;
								GradientDirectionVert.ptr<float>(i)[j] = 255;
								break;
							}
						// Set point on the Gradient image
						GradientImageOriginal.ptr<float>(i)[j] = G;	
						
					}					
				}
		}
		// apply gradient threshold to eliminate weaker pixels and non-edge pixels

		// convert to 8U to show gradient results and apply filters
		// to improve results	
		FilteredGradientA = GradientImageOriginal.clone();
		cv::minMaxLoc(GradientImageOriginal,&minVal, &maxVal);
		GradientImageOriginal.convertTo(FilteredGradientA,CV_8U,255.0/(maxVal - minVal), -minVal);
		//cv::convertScaleAbs(GradientImageOriginal,FilteredGradientA);
		cv::bilateralFilter(FilteredGradientA,FilteredGradientB,3,200,200);
		FilteredGradientC  = 2*FilteredGradientB;

		cv::Mat Kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
		cv::erode(FilteredGradientC,FilterErode,Kernel,cv::Point(-1,-1),1);

		cv::blur(FilterErode,BlurImage,cv::Size(3,3));
	    GradientImage = 3*(FilterErode - BlurImage);		

		// Built Gradient Map 
		// Save the data on the  Gradient Map, Direction Map and Edge Map
		cv::convertScaleAbs(GradientDirectionHor,GradientMapHor);
		cv::convertScaleAbs(GradientDirectionVert,GradientMapVert);

		// filter direction map
		for (int i=0;i<limit_i;i++){
			for(int j=0;j<limit_j;j++){
			// read the pixels values
			G = GradientImage.ptr<uchar>(i)[j];
			if (G == 0){
				GradientMapHor.ptr<uchar>(i)[j] = 0;
				GradientMapVert.ptr<uchar>(i)[j] = 0;
				}					
			}
		}		
		// Horizontal direction
		cv::bilateralFilter(GradientMapHor,FilterDirectionA,3,200,200);
		FilterTmp = 3*FilterDirectionA;
		cv::morphologyEx(FilterTmp,GradientMapHor,cv::MORPH_CLOSE,Kernel,cv::Point(-1,-1),1);
		// Vertical direction
		cv::bilateralFilter(GradientMapVert,FilterDirectionB,3,200,200);
		FilterTmp2 = 3*FilterDirectionB;
		cv::morphologyEx(FilterTmp2,GradientMapVert,cv::MORPH_CLOSE,Kernel,cv::Point(-1,-1),1);
	
	
		GradientMapChannels.push_back(GradientMapHor);
		GradientMapChannels.push_back(GradientMapVert);
		GradientMapChannels.push_back(GradientEdgels);
		//GradientMapChannels.push_back(GradientImage);
		cv::merge(GradientMapChannels,GradientMapTmp);
		GradientMap = GradientMapTmp;
		
		cv::imshow("Gradient",FilteredGradientB);		
		cv::imshow("Gradient Filtered",FilteredGradientC);
		cv::imshow("ED gradient",GradientImage);
		//cv::imshow("Morphological Filter",FilterTmp);
		cv::imshow("ED gradient original",GradientImageOriginal);
		cv::imshow("ED gradientMap",GradientMap);
		cv::imshow("ED gradientMap Hor Direction",GradientMapHor);
		cv::imshow("ED gradientMap Vert Direction",GradientMapVert);		
				
	}
	catch(...)
	{
	}
}
// get the Gradient Image
cv::Mat EDlines::getGradientImage(void){
	try{
		cv::Mat img = GradientImage.clone();
		return img;
	}
	catch(...){
	}
}
// get the gradient for a specific position on the image
Gradient_Properties EDlines::GetImagePointGradient(cv::Mat image, int x, int y){
	try{
		Gradient_Properties pixelProperties;
		double G,direction = 0;
		double Gx,Gy = 0;
		cv::Scalar values;
		double Ixy,Ix1y,Ixy1,Ix1y1=0;
		int y1,x1 = 0;
		int limitX = image.rows - 2;
		int limitY = image.cols - 2;
		// default values
		pixelProperties.x = x;
		pixelProperties.y = y;
		pixelProperties.gradientMagnitude = 0;
		pixelProperties.gradientMagnitudeX = 0;
		pixelProperties.gradientMagnitudeY = 0;
		pixelProperties.direction = 45;

		if (((x>=0)&(x<limitX))&((y>=0)&(y<limitY))){
			// read the pixels values
			Ixy = image.at<uchar>(x,y);
			Ix1y = image.at<uchar>(x+1,y);
			Ixy1 = image.at<uchar>(x,y+1);
			Ix1y1 = image.at<uchar>(x+1,y+1);
			
			/*values = cvGet2D(image,x,y);Ixy = values.val[0];
			x1 = x + 1;
			values = cvGet2D(image,x1,y);Ix1y = values.val[0];
			y1 = y + 1;
			values = cvGet2D(image,x,y1);Ixy1  = values.val[0];
			values = cvGet2D(image,x1,y1);Ix1y1 = values.val[0];*/

			// gradient on X
			Gx = 0.5*((Ix1y - Ixy)+(Ix1y1- Ixy1));
			// gradient on Y
			Gy = 0.5*((Ixy1 - Ixy)+(Ix1y1 - Ix1y));
			// Direction of pixel
			if(abs(Gx)>=abs(Gy)){
				direction = 0; // belong to a horizontal edge
			}else{
				direction = 90;  // belong to a vertical edge
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
		float currentPixelDirectionHor,currentPixelDirectionVert=0;
		int currentPixelDirection = 0;
		cv::Vec3b values,valuesEdge;
		valuesEdge.val[0] = 255;
		valuesEdge.val[1] = 0;
		valuesEdge.val[2] = 0;
		//valuesEdge.val[3] = 0;
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
			 
				 
			  if (((x-1)>=0)&((y-1)>=0)&(y<(imgSize.height-2))&(x<(imgSize.width-2))){
				   // mark pixel as visited and as an edge pixel
				  values.val[2] = 1;
				  //cvSet2D(GradientMap,y,x,values);
				  GradientMap.at<cv::Vec3b>(y,x) = values;
				  // Set point on the edge image
				  valuesEdge.val[0] = 255;
				  //cvSet2D(EdgeImage,y,x,valuesEdge);
				  EdgeImage.at<float>(y,x) = valuesEdge.val[0];
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
								  G_left_Up = GradientImage.at<uchar>(y-1,x-1);
								  G_left_Middle = GradientImage.at<uchar>(y,x-1);
								  G_left_Down = GradientImage.at<uchar>(y+1,x-1);
								  // check bigger
								  if ((G_left_Up > G_left_Middle)&(G_left_Up > G_left_Down)){// up is the bigger
									x = x-1;
									y = y-1;
								  }else if((G_left_Down > G_left_Up)&(G_left_Down >= G_left_Middle)){// down is the bigger
									x = x-1;
									y = y+1;
								  }else{// middle in another case is the bigger
									x = x-1;
									y = y;
								  }
							  break;
							  // RIGHT SIDE
							  case Route::RIGHT_ROUTE:
								  G_right_Up = GradientImage.at<uchar>(y-1,x+1);
								  G_right_Middle = GradientImage.at<uchar>(y,x+1);
								  G_right_Down = GradientImage.at<uchar>(y+1,x+1);
								  // check bigger
								  if ((G_right_Up > G_right_Middle)&(G_right_Up > G_right_Down)){// up is the bigger
									x = x+1;
									y = y-1;
								  }else if((G_right_Down > G_right_Up)&(G_right_Down >= G_right_Middle)){// down is the bigger
									x = x+1;
									y = y+1;
								  }else{// middle in another case is the bigger
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
								  G_up_Left = GradientImage.at<uchar>(y-1,x-1);
								  G_up_Middle = GradientImage.at<uchar>(y-1,x);
								  G_up_Right = GradientImage.at<uchar>(y-1,x+1);
								  // check bigger
								  if ((G_up_Left > G_up_Middle)&(G_up_Left > G_up_Right)){// left is the bigger
									x = x-1;
									y = y-1;
								  }else if((G_up_Right > G_up_Left)&(G_up_Right >= G_up_Middle)){// right is the bigger
									x = x+1;
									y = y-1;
								  }else{// middle up in another case is the bigger
									x = x;
									y = y-1;
								  }
							  break;
							  // DOWN SIDE
								case Route::DOWN_ROUTE:
								  G_down_Left = GradientImage.at<uchar>(y+1,x-1);
								  G_down_Middle = GradientImage.at<uchar>(y+1,x);
								  G_down_Right = GradientImage.at<uchar>(y+1,x+1);
								  // check bigger
								  if ((G_down_Left > G_down_Middle)&(G_down_Left > G_down_Right)){// left is the bigger
									x = x-1;
									y = y+1;
								  }else if((G_down_Right > G_down_Left)&(G_down_Right >= G_down_Middle)){// right is the bigger
									x = x+1;
									y = y+1;
								  }else{// middle down in another case is the bigger
									x = x;
									y = y+1;
								  }
							  break;
						  }// end internal switch 
						  break;
				  }// end external switch

			  }
			// get properties of next pixel
			if (((x-1)>=0)&((y-1)>=0)&(y<(imgSize.height-2))&(x<(imgSize.width-2))){

				//values = cvGet2D(GradientMap,y,x);			
				values = GradientMap.at<cv::Vec3b>(y,x);
				currentPixelGradient = GradientImage.at<uchar>(y,x);
				currentPixelDirectionHor = values.val[0];
				currentPixelDirectionVert = values.val[1];
				currentPixelEdgel = values.val[2];
				if (currentPixelDirectionVert == 255){
					currentPixelDirection = 255;
				}else{
					currentPixelDirection = 0;
				}
			
				//out_ofBounds = false;
			}
			// CHECK limits
			if((x<=0)|(y<=0)|(x>=(imgSize.width-2))|(y>=(imgSize.height-2))){
				currentPixelGradient = -1;
			}				  

		}// end while
		// SAVE CHAINS OF PIXELS bigger that 5 pixels
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
		float pixelValue,directionHor=0,directionVert=0;
		float G,Gup,Gdown,Gleft,Gright = 0;
		cv::Vec3b values;
		
		// Find the gradient map using a mask of 2x2
		for (int i=1;i<imgSize.height - 1;i+=ScanInterval){
			for(int j=1;j<imgSize.width - 1;j+=ScanInterval){
				//  check if the pixel has a value > 0
				pixelValue = GradientImage.ptr<uchar>(i)[j];
				if (pixelValue!=0){
					// current pixel gradient
					G = GradientImage.ptr<uchar>(i)[j];
					// Direction of pixel
					values = GradientMap.at<cv::Vec3b>(i,j);
					directionHor = values.val[0];
					directionVert = values.val[1];					
					// check for anchors, actual details = 1 pixel
					Gup = GradientImage.ptr<uchar>(i)[j-1];
					Gdown = GradientImage.ptr<uchar>(i)[j+1];
					Gleft = GradientImage.ptr<uchar>(i-1)[j];
					Gright = GradientImage.ptr<uchar>(i+1)[j];

					if (directionVert == 0){// horizontal, compare with up and down pixels						
						if (((G - Gdown)>= AnchorThreshold)&((G - Gup)>= AnchorThreshold)){
							isAnchor = true;
						}
					}else if (directionVert == 255){	// vertical, compare with left and right pixels
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
		cv::circle(AnchorsImage,cv::Point(AnchorsList.at(i).x,AnchorsList.at(i).y),1,CV_RGB(255,128,128));
	}
	// draw anchors found
	cv::imshow("ED Anchors",AnchorsImage);
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
		float currentPixelDirection = 0,currentPixelGradient,currentPixelEdgel = 0;
		float currentPixelDirectionHor,currentPixelDirectionVert=0;
		
		bool out_ofBounds = false;
		int x,y;
		cv::Scalar valuesEdge;
		cv::Vec3b values;
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
			if ((x<0)&(y<0)&(y>(imgSize.height-2))&(x>(imgSize.width-2))){
				out_ofBounds = true;
			}else{
				//values = cvGet2D(GradientMap,y,x);
				values = GradientMap.ptr<cv::Vec3b>(y)[x];
				currentPixelGradient = GradientImage.at<uchar>(y,x);
				currentPixelDirectionHor = values.val[0]; 
				currentPixelDirectionVert = values.val[1]; 
			
				if (currentPixelDirectionVert == 255){
					currentPixelDirection = 255;
				}else{
					currentPixelDirection = 0;
				}
				currentPixelEdgel = values.val[2];
				//currentPixelDirection = values.val[1];
				
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
cv::Mat EDlines::EdgeDrawing(cv::Mat edgeDst,int WindowSize,double gradient_Threshold){
	try{
		cv::Mat dstImg;
		std::vector<Anchors_ED> anchorsList;
		int scanInterval = 1; 
		float AnchorThreshold = 20, sigma = 1;//0.5
		SmoothImage(WindowSize,sigma);
		ApplyImageGradient(WindowSize,gradient_Threshold);// set tracking filter here
		anchorsList = FindEdgeAnchors(scanInterval,AnchorThreshold);
		EdgeLinking(anchorsList);
		// return internal calculed edge image

		/*IplImage header,*dstImg;
		IplImage *dstImg2  = cvCreateImage(imgSize,IPL_DEPTH_8U,1);
		dstImg = cvGetImage(EdgeImage,&header);
		cvConvertScaleAbs(dstImg,dstImg2);*/

		dstImg = EdgeImage.clone();
		return dstImg;
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
		cv::Mat A,A_Transposed,AtA,AtA_Inv,Atb,b,x;
		cv::Mat  Ax,Ax_b,Ax_bt,lineFitError;
		m = MinLineLength;
		n = 2;	// only 2 columns
		// matrices creation
		A = cv::Mat(m,n,CV_32FC1);
		A_Transposed = cv::Mat(n,m,CV_32FC1);
		AtA = cv::Mat(2,2,CV_32FC1);
		AtA_Inv = cv::Mat(2,2,CV_32FC1);
		Atb = cv::Mat(2,1,CV_32FC1);
		Ax = cv::Mat(m,1,CV_32FC1);
		Ax_b = cv::Mat(m,1,CV_32FC1);
		Ax_bt = cv::Mat(1,m,CV_32FC1);
		b = cv::Mat(m,1,CV_32FC1);
		x = cv::Mat(2,1,CV_32FC1); 
		lineFitError = cv::Mat(1,1,CV_32FC1);
		// build Matrices
		limit = m + startPixel;
		if (limit > pixels.size()){
			limit = pixels.size(); 
		}
		for (int i = 0;i < m; i++){
			//cvmSet(A,i,0,1.0);
			A.at<float>(i,0) = 1;
			if (lineType == Orientation::HORIZONTAL){	// Do a least squares taking pairs as (x,y)
				if ((i + startPixel)< pixels.size()){
					//cvmSet(A,i,1,pixels.at(i + startPixel).x);	
					//cvmSet(b,i,0,pixels.at(i + startPixel).y);	
					A.at<float>(i,1) = pixels.at(i + startPixel).x;	// x values
					b.at<float>(i,0) = pixels.at(i + startPixel).y;	// y values
				}
			}
			if (lineType == Orientation::VERTICAL){
				if ((i + startPixel)< pixels.size()){	// Do a least squares taking pairs as (y,x)
					//cvmSet(A,i,1,pixels.at(i + startPixel).y);	
					//cvmSet(b,i,0,pixels.at(i + startPixel).x);	
					A.at<float>(i,1) = pixels.at(i + startPixel).y;	// y values
					b.at<float>(i,0) = pixels.at(i + startPixel).x;	// x values

				}
			}
		}		
		/*cvTranspose(A,A_Transposed);
		cvMatMul(A_Transposed,A,AtA);
		cvInvert(AtA,AtA_Inv,CV_LU);
		cvMatMul(A_Transposed,b,Atb);*/
		
		A_Transposed = A.t();
		AtA = A_Transposed*A;
		AtA_Inv = AtA.inv(cv::DECOMP_LU);
		Atb = A_Transposed*b;

		// solution for x
		//cvMatMul(AtA_Inv,Atb,x);
		x = AtA_Inv*Atb;
		// calculate the least square error	
		
		/*cvMatMul(A,x,Ax);
		cvSub(Ax,b,Ax_b);
		cvTranspose(Ax_b,Ax_bt);
		cvMatMul(Ax_bt,Ax_b,lineFitError);*/

		Ax = A*x;
		Ax_b = Ax - b;
		Ax_bt = Ax_b.t();
		lineFitError = Ax_bt*Ax_b;

		// set values for lineEquation
		
		/*alpha = cvmGet(x,0,0);
		beta = cvmGet(x,1,0);*/
		alpha = x.at<float>(0,0);
		beta = x.at<float>(1,0);

		// set the adequate values on the line equation
		// according to line orientation
		if (lineType == Orientation::HORIZONTAL){	// Do a least squares taking pairs as (x,y)
				lineEquation.a = -beta;
				lineEquation.b = 1.0;
				lineEquation.c = -alpha; 
		}else{										// Do a least squares taking pairs as (y,x)
				lineEquation.a = 1;
				lineEquation.b = -beta;
				lineEquation.c = -alpha; 
		}
		// set other values of line Equation
		lineEquation.x1 = pixels.at(startPixel).x;
		lineEquation.y1 = pixels.at(startPixel).y;
		lineEquation.x2 = pixels.at(limit-1).x;
		lineEquation.y2 = pixels.at(limit-1).y;
		// return error
		//lineFitErrorValue = cvmGet(lineFitError,0,0);
		lineFitErrorValue = lineFitError.at<float>(0,0);
 			
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
								
			if(lineFitError > 3.0){	//1.0		// no initial line segment, returns
				//leastSquareError = 1000;
				//ErrorLimitCounter = ErrorLimitCounter + 1;
				StartPixel = MinLineLength + StartPixel;
				lineLen = StartPixel;
			}else if (lineFitError <= 3.0){	//1.0
				// O.k An initial line segment detected
				// try to extend the line segment detected
				while(lineLen < noPixels){
					currentPoint = pixels.at(lineLen);
					distance = ComputePointDistance2Line(lineEquation,currentPoint);
					if (distance > 3.0){//1.0		// max take 1 pixel from the line
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
/*==================================================================/
Application for rotating a cube using OpenCV library - header file 	/
Author: Bartosz Pacuła												/
January, 2019														/
===================================================================*/
// kostka.hpp
//

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/calib3d.hpp"

#ifndef PUGIXML_HEADER
	#include "pugixml.hpp" // header file for xml support
#endif

#ifndef IO_HEADERS
	#include "iostream"
	#include "fstream"  //for file output functionality
#endif

//default value for thresholding filter:
#define DEFAULT_THRESHOLD 128


namespace kostka {

	using namespace std;
	using namespace cv;


	class Face {
	public:
		
		Face();
		void set(Vec4i); //sets this face's vertex order
		Vec4i get() const; //gets this face's vertex order
		void setFlag(int); //sets a value of visibility flag 
		void set_color(Scalar); //sets face's colour
		Scalar get_color() const; //gets face's colour
		void setTexture(Mat); //sets face's texture
		Mat getTexture() const; //gets face's texture
		Mat drawTexture(Scalar); //draws a face of a given colour
		int getFlag() const; //gets visibility flag pointer
	private:
		int isVisible; //face visibility flag
		Vec4i vertexOrder; //vector of vertices order values
		Scalar color; //this face's colour
		Mat texture; //this face's texture
	};


	Face::Face(void){
		vertexOrder=Vec4i(0,1,2,3); //default vertices order
		isVisible=1; //this face is visible by default when it's created
		color=Scalar(255,255,255); //at its creation the default colour of the face is white 
		Mat im(2,2,CV_8UC3,Scalar(255,255,255)); //initial texture of the face (plain white) is being created (texture size is 2x2 pixels just for a sake of initialization)
		texture=im; //freshly created texture is being assigned to the face
	}


	void Face::set(Vec4i order){
		vertexOrder=order;
	}

	Vec4i Face::get() const {
		return vertexOrder;
	}

	void Face::setFlag(int flaga){
		isVisible=flaga;
	}

	int Face::getFlag() const {
		return isVisible;
	}

	void Face::set_color(Scalar k){
		color=k;
	}

	Scalar Face::get_color() const {
		return color;
	}

	void Face::setTexture(Mat im){
		texture=im.clone();
	}

	Mat Face::getTexture() const {
		return texture;
	}

	Mat Face::drawTexture(Scalar color){ //draws texture of a given colour
		int i,j;
		int edgeWidth=5;
		Point2f p1,p2; //points' coordinates for gray lines
		Scalar lightGray=Scalar(200,200,200); //for the sake of drawing gray lines
		Scalar white=Scalar(255,255,255); //BGR colour palette values for the colour white
		Mat im(300,300,CV_8UC3,white); //bitmap creation (number of rows first, then the number of columns, data type specifying the number of channels and last the colour of the bitmap)
		int width=(int)((im.cols-edgeWidth*4)/3.0); //width of a single rectangle
		int height=width; //height of a single rectangle, it's a square in fact
		Mat im_temp(width,height,CV_8UC3,color); //an image of a single square
		Rect rectangle; // ROI is being set (the region of interest)
		
		//drawing of squares--------------------------------	
		for(j=0;j<3;j++)
			for(i=0;i<3;i++)
			{
				rectangle=Rect(edgeWidth+i*(edgeWidth+width),edgeWidth+j*(edgeWidth+height),width,height);
				im_temp.copyTo(im(rectangle)); //an image of a square is being pasted into the face's bitmap
			}

		//drawing gray lines--------------------------------
		for(i=0;i<3;i++)
		{
			p1=Point2f(edgeWidth+width/(float)2.0+i*(edgeWidth+width),(float)edgeWidth+2 /* 2 - an adjustment for the line's thickness */); //for vertical lines 
			p2=Point2f(edgeWidth+width/(float)2.0+i*(edgeWidth+width),(float)im.rows-edgeWidth-2);

			line(im,p1,p2,lightGray,2 /*line's thickness*/,CV_AA /*type: with antialiasing*/,0 /*number of fraction bits within a pixel coordinate*/);

			p1=Point2f((float)edgeWidth+2,edgeWidth+width/(float)2.0+i*(edgeWidth+width)); //for horizontal lines
			p2=Point2f((float)im.cols-edgeWidth-2,edgeWidth+width/(float)2.0+i*(edgeWidth+width));
		
			line(im,p1,p2,lightGray,2 /*line's thickness*/,CV_AA /*type: with antialiasing*/,0 /*number of fraction bits within a pixel coordinate*/);
		}

		return im;
	}


	class Cube
	{
	public:
		Cube(); //defaut constructor
		Mat* getMatVrtPtr(); //gets a pointer to vertices' matrix
		Face* getFacePtr(int); //gets a pointer to a cube's face of a given number
	private:
		Mat m_vrt; //cube's vertices (8x3 matrix)
		Face m_faceObj[6]; //six faces of the cube
	};


	Cube::Cube(void){ //initialization
		int i;
		m_vrt=(Mat_<double>(8,3) << //a cube of size 2 and center in the coordinates system's origin
		-1.0,1.0,-1.0,			//coordinated of the cube's vertices
		-1.0,-1.0,-1.0,
		1.0,-1.0,-1.0,
		1.0,1.0,-1.0,
		-1.0,1.0,1.0,
		-1.0,-1.0,1.0,
		1.0,-1.0,1.0,
		1.0,1.0,1.0);
		m_faceObj[0].set(Vec4i(0,1,2,3)); //the order of vertices of a given face is being set
		m_faceObj[1].set(Vec4i(3,2,6,7));
		m_faceObj[2].set(Vec4i(7,6,5,4));
		m_faceObj[3].set(Vec4i(4,5,1,0));
		m_faceObj[4].set(Vec4i(4,0,3,7));
		m_faceObj[5].set(Vec4i(1,5,6,2));

		//initial faces' colour setup (channels' colour order is BGR)
		m_faceObj[0].set_color(Scalar(255,128,128));
		m_faceObj[1].set_color(Scalar(128,255,128));
		m_faceObj[2].set_color(Scalar(128,128,255));
		m_faceObj[3].set_color(Scalar(255,255,128));
		m_faceObj[4].set_color(Scalar(128,255,255));
		m_faceObj[5].set_color(Scalar(255,128,255));

		//initial texture bitmaps are being assigned to the faces
		for(i=0;i<6;i++)
			m_faceObj[i].setTexture(m_faceObj[i].drawTexture(m_faceObj[i].get_color())); //assigning textures to the cube's faces
	}

	Mat* Cube::getMatVrtPtr(){
		return &m_vrt;
	}

	Face* Cube::getFacePtr(int i) { //gets a pointer to a cube's face of a given number
		return (i>=0 && i<=5)?&m_faceObj[i]:&m_faceObj[0]; // additionaly handles out of range indexes
	}

	//--------------------------------------Forward function definitions-------------------------------------------------------
	
	void about(void); //shows some information about this application
	void calcNormVector(Cube&); //calculation of normal vectors of cube's faces and visibility flags' setup
	string charStr2str(char*&); // converts zero-terminated char string to string
	short _checkFileType(char*); //checks if the file name extension is .txt or .xml, if none of them returns 0, if .txt 1 and .xml 2
	void _convGauss(Mat&, const Mat&, const Mat&, const Mat&, const int); //calculating convolution with 1D kernel
	int countCheck(const int, char**); //checks input count for the main function
	bool __extCheck(const char*, const char*); //Checking if a .txt file
	char __flipChar(char); // fliping character function - if c is lowercase letter flip makes it uppercase and vice versa, if c is not a letter it is returned unchanged
	void gauss(Mat&); // applying a gaussian blur filter
	void help(); //displays usage information
	void im2bw(Mat&, int); // convertion from BGR image to black and white image with a given threshold
	void joinImages(const vector<Mat>&,Mat&,int); //joining of particular faces' images into a composition image
	int __length(const char*); //calculates a zero-terminated string's length
	bool __isValidFileName(const char*); //checks if the file name is one of the two supported file types
	bool __isValidFilter(const char*); //checks if the image filter name is one of the two supported filter kinds
	bool __isValidImgFileName(const char*); //checks if the file name is one of the two supported image file names
	void loadConfigTxt(const char*, Cube&); //loads display image and filter configuration from a text file
	void loadConfigXml(const char*, Cube&); //loads display image and filter configuration from a xml file
	Mat renderPerspecitve(Cube&,Cube&); //creation of the cube's image with perspective-transformed faces' bitmaps
	void rot90CCW(Mat&); //image rotation by 90 degrees counterclockwise
	void rot90CW(Mat&); //image rotation by 90 degrees clockwise
	Mat rotateObject(Mat&,const double,const double,const double); //rotation of an object in 3D space
	char* __str2charStr(const string&); //convertion of string to a zero-terminated C-style string
	Mat updateView(Cube&,double,double,double); //an update of the cube's view (rotation of a cube against particular axis and creation of the cube's image (angles values in degrees)

	//-------------------------------------------------------------------------------------------------------------------------

	Mat rotateObject(Mat& matObj, const double alfa, const double beta, const double gamma) //rotation of an object in 3D space
	{
		Mat Rx=Mat_<double>(3,3);
		Mat Ry=Mat_<double>(3,3);
		Mat Rz=Mat_<double>(3,3);

		Mat R=Mat_<double>(3,3); //composite rotation matrix
		
		//rotation against x-axis matrix setup
		Rx.at<double>(0,0)=1.0;
		Rx.at<double>(0,1)=0.0;
		Rx.at<double>(0,2)=0.0;

		Rx.at<double>(1,0)=0.0;
		Rx.at<double>(1,1)=cos(alfa);
		Rx.at<double>(1,2)=-sin(alfa);

		Rx.at<double>(2,0)=0.0;
		Rx.at<double>(2,1)=sin(alfa);
		Rx.at<double>(2,2)=cos(alfa);
		//------------------------------------------
		//rotation against y-axis matrix setup
		Ry.at<double>(0,0)=cos(beta);
		Ry.at<double>(0,1)=0.0;
		Ry.at<double>(0,2)=sin(beta);

		Ry.at<double>(1,0)=0.0;
		Ry.at<double>(1,1)=1.0;
		Ry.at<double>(1,2)=0.0;

		Ry.at<double>(2,0)=-sin(beta);
		Ry.at<double>(2,1)=0.0;
		Ry.at<double>(2,2)=cos(beta);
		//------------------------------------------
		//rotation against z-axis matrix setup
		Rz.at<double>(0,0)=cos(gamma);
		Rz.at<double>(0,1)=-sin(gamma);
		Rz.at<double>(0,2)=0.0;

		Rz.at<double>(1,0)=sin(gamma);
		Rz.at<double>(1,1)=cos(gamma);
		Rz.at<double>(1,2)=0.0;

		Rz.at<double>(2,0)=0.0;
		Rz.at<double>(2,1)=0.0;
		Rz.at<double>(2,2)=1.0;
		//------------------------------------------

		R=Rz*Ry*Rx; //composite rotation matrix calculation

		transpose(matObj,matObj); //transpose the object matrix before its rotation

		matObj=R*matObj; // rotate object's vertices against the origin of coordinates system

		transpose(matObj,matObj); //transpose the object matrix after the rotation

		return matObj;
	}

	void joinImages(const vector<Mat>& vecImages,Mat& imComposite,int nImages) //joining of particular faces' images into a composition image
	{
		int i,j,ch,z;
		int maxx,maxy; //image width and height (in pixels)
		Scalar px; //a given pixel's colour

		imComposite=vecImages[0].clone(); //initialization of the composition image with a copy of one of the face images
		maxx=imComposite.cols; //get image width
		maxy=imComposite.rows; //get image height

		//copying pixel intensity values of particular image into a composition image only if the pixel colour differs from black
		for(z=1;z<nImages;z++)
			for(j=0;j<maxx;j++)
				for(i=0;i<maxy;i++)
					for(ch=0;ch<3;ch++) //3-channel image
					{
						px=vecImages[z].at<Vec3b>(i,j); //get pixel intensity values vector
						if(px.val[ch] > 0) //if not black
							imComposite.at<Vec3b>(i,j).val[ch]=vecImages[z].at<Vec3b>(i,j).val[ch]; //copying pixel intensity value for a given channel
					}
		//----------------------------------------------------------------------------------------------------------------------
	}


	Mat renderPerspecitve(Cube& cubeObj,Cube& cubeObj_orig) //creation of the cube's image with perspective-transformed faces' bitmaps
	{
		int i;
		vector<Mat> vecImages; //a vector containing images of the cube's faces after the perspective transform
		Mat imComposite; //resulting composition image
		Mat trans; //transform matrix
		Mat temp; //temporary matrix containing an image of a given face after the perspective transform
		Mat m_vrt=*cubeObj.getMatVrtPtr(); //get pointer to matrix of cube vertices
		Mat m_vrt_orig=*cubeObj_orig.getMatVrtPtr(); //get pointer to matrix of original cube vertices

		Mat im[6]; //six bitmaps, each for every face of the cube
		for(i=0;i<6;i++)
			im[i]=cubeObj.getFacePtr(i)->getTexture(); //get and assign face's texture

		Point2f pIm[4]; //four points being a result of perspective projection
		Point2f pW[4]; //four coresponding points for the world coordinates

		int j,nImages=0;
		for(j=0;j<6;j++)
		{
			if(cubeObj.getFacePtr(j)->getFlag()) //visible faces are considered only
			{
				Vec4i kol=cubeObj.getFacePtr(j)->get();

				//projected cube face's vertices are being translated together so one of them is in the point of the origin and scaled afterwards according to the bitmap size
			    pIm[0]=Point2f((float)(m_vrt.at<double>(kol[0],0)+1)*im[0].cols/2,(float)(m_vrt.at<double>(kol[0],1)+1)*im[0].cols/2);
			    pIm[1]=Point2f((float)(m_vrt.at<double>(kol[1],0)+1)*im[0].cols/2,(float)(m_vrt.at<double>(kol[1],1)+1)*im[0].cols/2);
			    pIm[2]=Point2f((float)(m_vrt.at<double>(kol[2],0)+1)*im[0].cols/2,(float)(m_vrt.at<double>(kol[2],1)+1)*im[0].cols/2);
			    pIm[3]=Point2f((float)(m_vrt.at<double>(kol[3],0)+1)*im[0].cols/2,(float)(m_vrt.at<double>(kol[3],1)+1)*im[0].cols/2);  
			    //---------------------------------------------------------------------------------------------------------------
			    //in case four points appear in the same line a certain correction of the pixel values is provided
			    for(i=0;i<4;i++)
				    if(pIm[i].x==pIm[(i+1)%4].x || pIm[i].y==pIm[(i+1)%4].y) {
				    	pIm[(i+1)%4].x+=(float)0.1;
				    	pIm[(i+1)%4].y+=(float)0.1;
				    }
			    //------------------------------------------------------------------------------------------------

				//projected vertices of the original unrotated cube face are being translated and scaled either 
			    pW[0]=Point2f((float)(m_vrt_orig.at<double>(0,0)+1)*(im[0].cols)/2,(float)(m_vrt_orig.at<double>(0,1)+1)*(im[0].cols)/2);
			    pW[1]=Point2f((float)(m_vrt_orig.at<double>(1,0)+1)*(im[0].cols)/2,(float)(m_vrt_orig.at<double>(1,1)+1)*(im[0].cols)/2);
			    pW[2]=Point2f((float)(m_vrt_orig.at<double>(2,0)+1)*(im[0].cols)/2,(float)(m_vrt_orig.at<double>(2,1)+1)*(im[0].cols)/2);
			    pW[3]=Point2f((float)(m_vrt_orig.at<double>(3,0)+1)*(im[0].cols)/2,(float)(m_vrt_orig.at<double>(3,1)+1)*(im[0].cols)/2);
			    //---------------------------------------------------------------------------------------------------------------------------------------

			    Mat pImMat=(Mat_<float>(4,2) << //changing vectors into a matrix for use in findHomography() function
			    	pIm[0].x,pIm[0].y,
			    	pIm[1].x,pIm[1].y,
			    	pIm[2].x,pIm[2].y,
			    	pIm[3].x,pIm[3].y);
			    Mat pWMat=(Mat_<float>(4,2) << //changing vectors into a matrix for use in findHomography() function
			    	pW[0].x,pW[0].y,
			    	pW[1].x,pW[1].y,
			    	pW[2].x,pW[2].y,
			    	pW[3].x,pW[3].y);

			    trans=findHomography(pImMat,pWMat); //homography transform matrix is being calculated

			    Mat tmat=(Mat_<double>(3,3) << //translation matrix for homography transform matrix
			    	1,0,im[j].cols/2,
			    	0,1,im[j].cols/2,
			    	0,0,1);

			    trans=trans.inv(); // inverse of homography transform matrix is being calculated
			    trans=tmat*trans;  
			    // homography transform matrix is being translated down and to the right by a half of bitmap size accordingly

			    warpPerspective(im[j],temp,trans,Size(2*im[j].cols,2*im[j].rows)); // bitmap is remapped with perspective warp and the field of view is made wider

			    vecImages.push_back(temp.clone()); //resulted image is pushed into the container vector

				nImages++; //number of calculated images updated
			}
		}

		joinImages(vecImages,imComposite,nImages); //joining of particular faces' images into a composition image

		return imComposite;
	}

	void calcNormVector(Cube& cubeObj) //calculation of normal vectors of cube's faces and visibility flags' set
	{
		int i;
		Vec3f vec1,vec2;
		Vec4i vecFace;
		Point3f p,p1,p2,pCenter;
		Mat m_vrt=*cubeObj.getMatVrtPtr(); //get pointer to matrix of cube vertices

		for(i=0;i<6;i++)
		{
			vecFace=cubeObj.getFacePtr(i)->get(); //get face's vertices order

			//finding center point of the face
			p1=Point3f(m_vrt.row(vecFace[0])); 
			p2=Point3f(m_vrt.row(vecFace[2]));
			pCenter=Point3f((p1.x+p2.x)/(float)2.0,(p1.y+p2.y)/(float)2.0,(p1.z+p2.z)/(float)2.0);

			//finding two coplanar vectors
			p=pCenter;
			p1=Point3f(m_vrt.row(vecFace[1])); 
			p2=Point3f(m_vrt.row(vecFace[0]));

			vec1=Vec3f(p1.x-p.x,p1.y-p.y,p1.z-p.z);
			vec2=Vec3f(p2.x-p.x,p2.y-p.y,p2.z-p.z);
			//---------------------------------------

			//calculation of normal vector-----------
			Mat wektory=(Mat_<float>(2,3) <<
				vec1[0],vec1[1],vec1[2],
				vec2[0],vec2[1],vec2[2]);

			Vec3f normal=wektory.row(0).cross(wektory.row(1)); //normal vector is a cross-product of two coplanar vectors having common origin
			//---------------------------------------

			//setting visibility flag value for a given face
			cubeObj.getFacePtr(i)->setFlag((normal[2]>-0.4)?0:1); //the condition used has been found experimentally (1 - face is visible, 0 - face is invisible)
			//---------------------------------------
		}
	}

	Mat updateView(Cube& cubeObj,double alfa, double beta, double gamma) //an update of the cube's view (rotation of a cube against particular axis and creation of the cube's image; angles values in degrees)
	{
		Cube cubeObj_orig; //original version of the cube as a reference for the calculation (the one that's not being rotated)
		
		Mat m_vrt_orig=*cubeObj_orig.getMatVrtPtr(); //get matrix of original cube vertices
		Mat* p_m_vrt=cubeObj.getMatVrtPtr(); //get pointer to matrix of cube vertices
		
		Mat im; //result image


		alfa=alfa*CV_PI/180.0; //value changed into radians
		beta=beta*CV_PI/180.0; //value changed into radians
		gamma=gamma*CV_PI/180.0; //value changed into radians

		//rotation-----------------------------------------------------
		*p_m_vrt=rotateObject(m_vrt_orig,alfa,beta,gamma);
		//-------------------------------------------------------------

		calcNormVector(cubeObj); //find normal vectors of cube's faces and set visibility flags accordingly

		//projection-------------------------------------------------------------------------------------------
		double f=3; // camera focal point

		Mat camM=(Mat_<double>(3,3) << //camera matrix
			f,0.0,0,
			0.0,f,0,
			0.0,0.0,1.0);

		Vec3d rvec(0.0,0.0,0.0),tvec(0.0,0.0,5.0); 	//rotation vector (its values set to zero, just for use in projectPoints() function)
													// and translation vector to make all the z-axis values of the points be greater than zero
		double aspectRatio=0.0; //zero means a default aspect ratio

		projectPoints(*p_m_vrt,rvec,tvec,camM,noArray(),*p_m_vrt,noArray(),aspectRatio); //project cube's vertices onto 2D plane
		//-----------------------------------------------------------------------------------------------------

		LOG(_RETURN,"Mat im");
		return renderPerspecitve(cubeObj,cubeObj_orig); //creation of the cube's image with perspective-transformed faces' bitmaps
	}

	void about(void) //shows some information about this application
	{
		cout << "\n";
		cout << "/===================================================================/\n";
		cout << "Application for rotating a cube using OpenCV library\t\t    /\n";
		cout << "Author: Bartosz Pacula, version 1.1\t\t\t\t    /\n";
		cout << "January, 2019\t\t\t\t\t    OpenCV ver.3.2.0/\n";
		cout << "/===================================================================/\n";
		cout << "Control:\t\t\t\t\t\t\t    /\n";
		cout << "Press \"s\" or \"x\" to rotate against x axis\t\t\t    /\n";
		cout << "Press \"d\" or \"c\" to rotate against y axis\t\t\t    /\n";
		cout << "Press \"a\" or \"z\" to rotate against z axis\t\t\t    /\n";
		cout << "Press \"q\" to exit the program\t\t\t\t\t    /\n";
		cout << "(Rotation of the cube is possible only with the cube window active) /\n";
		cout << "/===================================================================/\n";
		cout << "\n";
	}

	void im2bw(Mat& im, int threshold) // convertion from BGR image to black and white image with a given threshold
	{
		int i,j;

		LOG("Converting the image to gray scale image");
		cvtColor(im,im,COLOR_BGR2GRAY); // convertion to gray scale image

		LOG(_FORLOOP);
		LOG("Binarizing in a loop");
		for(i=0;i<im.cols;i++)
			for(j=0;j<im.rows;j++)
				im.at<uchar>(i,j)=(im.at<uchar>(i,j)>=threshold)?255:0; //changing intensity values depending on the threshold, 255 for white and 0 for black
		LOG(_FORLOOPE);	
		LOG("Converting the image back to 3-channel image");
		cvtColor(im,im,COLOR_GRAY2BGR); //converting back to 3-channel image
		LOG("Leaving im2bw() function");
	}

	void rot90CW(Mat& im) //image rotation by 90 degrees clockwise
	{
		LOG("Transposition and flip");
		transpose(im,im);
		flip(im,im,1);
		LOG("Leaving rot90CW() function");
	}

	void rot90CCW(Mat& im) //image rotation by 90 degrees counterclockwise
	{
		LOG("Flip and transposition");
		flip(im,im,1);
		transpose(im,im);
		LOG("Leaving rot90CCW() function");
	}

	void _convGauss(Mat& dst,const Mat& temp, const Mat& im,const Mat& kernel,const int offset) //calculating convolution with 1D kernel 
																							//(im width and height must be smaller than those of temp)
																							//temp and dst must be 32bit color images
	{
		int i,j,ch;
		int l; //additional variable for looping the kernel
		double convSum; // sum of pixel values multiplied by kernel
		LOG(_FORLOOP);
		for(i=0;i<im.rows;i++)
			for(j=0;j<im.cols;j++)
				for(ch=0;ch<3;ch++)
				{
					convSum=0;
					for(l=-offset;l<=offset;l++)
						convSum+=temp.at<Vec3w>(i+offset,j+offset+l).val[ch]*kernel.at<double>(0,offset+l);

					dst.at<Vec3w>(i+offset,j+offset).val[ch]=(ushort)convSum;
				}
		LOG(_FORLOOPE);
		LOG("Leaving _convGauss() function");
	} 


	void gauss(Mat& im) // applying a gaussian blur filter
						// (if using integer kernels this filter applies only up to kernel size 5 due to 16 bits per pixel value limit)
	{
		int i,j,k;

		//default 1D kernel size=3
		Mat v1=(Mat_<double>(1,3) << 1,2,1); //default values for a gaussian 1D kernel of size 3
		Mat v2;
		double sum=0.0;
		transpose(v1,v2);
		Mat mask=v2*v1; //creating 2D kernel mask

		LOG(_FORLOOP);
		for(i=0;i<mask.cols;i++)
			for(j=0;j<mask.rows;j++)
				sum+=mask.at<double>(i,j); //adding all matrix elements up (this sum will be used for normalization later on)
		LOG(_FORLOOPE);
		LOG("sum = ",sum);

		//resizing the image to handle border pixels----------------------------
		LOG("Resizing the image to handle border pixels");
		Mat temp(im.rows+mask.rows-1,im.cols+mask.cols-1,CV_8UC3,Scalar(0,0,0)); //creating a temporary BGR image slightly bigger than the original one
		int offset=(mask.rows-1)/2; //offset value that equals to the border size
		LOG("offset = ",offset);

		LOG("Finding vertical borders");
		//finding vertical borders--------
		Mat upperBorder=im.row(0);
		Mat lowerBorder=im.row(im.rows-1);
		//--------------------------------

		LOG(_FORLOOP);
		for(i=0;i<offset;i++) //in case of 3x3 kernel this loop will execute only once
			for(j=0;j<im.cols;j++)	//for each pixel in a row	
				for(k=0;k<3;k++) //for each image channel
				{
					temp.at<Vec3b>(i,offset+j).val[k]=upperBorder.at<Vec3b>(0,j).val[k]; //pasting border pixels into temporary image
					temp.at<Vec3b>(temp.rows-1-i,offset+j).val[k]=lowerBorder.at<Vec3b>(0,j).val[k]; //pasting border pixels into temporary image
				}
		LOG(_FORLOOPE);
		im.copyTo(temp(Rect(offset,offset,im.cols,im.rows))); // pasting the original image into a center of the temporary one

		LOG("Finding horizontal borders");
		//finding horizontal borders-----------------
		Mat leftBorder=temp.col(offset);
		Mat rightBorder=temp.col(temp.cols-1-offset);
		//-------------------------------------------

		LOG(_FORLOOP);
		for(i=0;i<offset;i++) //in case of 3x3 kernel this loop will execute only once
			for(j=0;j<temp.rows;j++)	//for each pixel in a row	
				for(k=0;k<3;k++) //for each image channel
				{
					temp.at<Vec3b>(j,i).val[k]=leftBorder.at<Vec3b>(j,0).val[k]; //pasting border pixels into temporary image
					temp.at<Vec3b>(j,temp.cols-1-i).val[k]=rightBorder.at<Vec3b>(j,0).val[k]; //pasting border pixels into temporary image
				}
		//end of resizing the image---------------------------------------------
		LOG("End of resizing the image");
		
		LOG("Convolution - the first pass");		
		//convolution - the first pass------------------------------------------
		temp.convertTo(temp,CV_16U); //convertion into 16bit unsigned int image
		Mat tempPass1=temp.clone(); //output matrix for the first pass
		LOG("Call _convGauss(",_MAT(tempPass1),",",_MAT(temp),",",_MAT(im),",",_MAT(v1),",",offset,")");
		_convGauss(tempPass1,temp,im,v1,offset); //calculating first convolution with 1D kernel
		//----------------------------------------------------------------------

		LOG("Call rot90CW(",_MAT(tempPass1),")");
		rot90CW(tempPass1); //rotation of tempPass1 image by 90 degrees clockwise

		LOG("Convolution - the second pass");		
		//convolution - the second pass------------------------------------------
		Mat tempPass2=temp.clone(); //output matrix for the second pass
		LOG("Call _convGauss(",_MAT(tempPass2),",",_MAT(tempPass1),",",_MAT(im),",",_MAT(v1),",",offset,")");
		_convGauss(tempPass2,tempPass1,im,v1,offset); //calculating second convolution with 1D kernel
		//----------------------------------------------------------------------

		LOG("Call rot90CCW(",_MAT(tempPass2),")");
		rot90CCW(tempPass2); //rotation by 90 degrees back to the original orientation

		tempPass2=1/sum*tempPass2; //normalization of the pixel values

		tempPass2.convertTo(tempPass2,CV_8U); //convertion back to BGR image

		LOG("Cropping the transformed image to get rid of borders");
		im=tempPass2(Rect(offset,offset,im.cols,im.rows)); //cropping the transformed image to get rid of borders
		LOG("im = ",_MAT(im));
		LOG("Leaving gauss() function");
	}

	void help() //displays usage information
	{
		LOG("Displaying help message");
		cout << "\nZadanie_kostka by Bartosz Pacula  ver. 1.1\n";
		cout << "\nUsage: zadanie_kostka [config_file.txt|config_file.xml]\n";
	}

	string charStr2str(char*& cstr) // converts zero-terminated char string to string
	{
		string str="";

		LOG("Call __length(\"",cstr,"\")");
		for(int i=0;i<__length(cstr);i++)
			str+=cstr[i];

		LOG(_RET("charStr2str()"),str);
		return str;
	}

	char* __str2charStr(const string& str) //convertion of string to a zero-terminated C-style string
	{
		unsigned int i;
		char *cStr;
		cStr=(char*)malloc(sizeof(char)*(str.size()+1));

		if(str.size()!=0){
			for(i=0;i<str.size();i++)
				*(cStr+i)=str[i];
			cStr[i]='\0';
		}
		else *cStr='\0';

		return cStr;	
	}


	bool __isValidImgFileName(const char* filename) //checks if the file name is one of the two supported image file names
	{
		return (__extCheck(filename,"jpg") || __extCheck(filename,"png"))?1:0;
	}


	bool __isValidFilter(const char* filter) //checks if the image filter name is one of the two supported filter kinds
	{
		return (!strcmp(filter,"thresholding") || !strcmp(filter,"gauss") || !strcmp(filter,""))?1:0;
	}


	void loadConfigTxt(const char* filename, Cube& cubeObj) //loads display image and filter configuration from a text file
	{
		cv::Mat im;
		unsigned int i,j;
		vector<string> buffer;
		string temp;

		ifstream configFile(filename,ios::in);

		if(!configFile.is_open())
		{
			ELOG("Error reading ",filename," configuration file!");
			cout << "\nError reading " << filename << " configuration file!\n";
			LOG("Call exit(-1)");
			exit(-1);
		}
		else
		{	
			i=0;
			while(!configFile.eof())
			{
				//each line of the input file is being put into the buffer vector
				getline(configFile,temp);
				buffer.push_back(temp);
				//---------------------------------------------------------------
				i++;
			}
			configFile.close();
		}

		pair<string, string> *fileName_Filter=new pair<string, string>[buffer.size()];

		LOG(_FORLOOP);
		for(i=0;i<buffer.size();i++)
		{
			fileName_Filter[i].first="";
			fileName_Filter[i].second="";
			j=0;

			if(buffer[i].size()==0) goto end_of_this_loop;

			while(!(buffer[i][j]!=' ' && buffer[i][j]!='\t') && j<buffer[i].size()){ //skipping white spaces
				j++;
			}

			while((buffer[i][j]!=' ' && buffer[i][j]!='\t') && j<buffer[i].size())
			{
				fileName_Filter[i].first+=buffer[i][j];
				j++;
			}
			while(!(buffer[i][j]!=' ' && buffer[i][j]!='\t') && j<buffer[i].size()){ //skipping white spaces
				j++;
			}

			while((buffer[i][j]!=' ' && buffer[i][j]!='\t') && j<buffer[i].size()){

				fileName_Filter[i].second+=buffer[i][j];
				j++;
			}
		}
		end_of_this_loop:
		LOG(_FORLOOPE);

		if(buffer.size()>6)
			ELOG("Too many entries within the input ",filename," file");

		j=(buffer.size()<=6)?buffer.size():6; //loading more than 6 images is forbidden no matter how many entries are there within the input file
		
		char *tempPtr; //for memory leaks handling
		LOG(_FORLOOP);
		for(i=0;i<j;i++)
		{
			LOG("Call __isValidImgFileName(\"",fileName_Filter[i].first,"\")");
			if(__isValidImgFileName(tempPtr=__str2charStr(fileName_Filter[i].first)))
			{
				free(tempPtr);
				im=cv::imread(fileName_Filter[i].first,cv::IMREAD_COLOR); //reading image file
					//optional filtering--
					LOG("Call __isValidFilter(\"",fileName_Filter[i].second,"\")");

					if(__isValidFilter(tempPtr=__str2charStr(fileName_Filter[i].second)))
					{
						if(fileName_Filter[i].second=="thresholding"){
							LOG("Call im2bw(",_MAT(im),",",DEFAULT_THRESHOLD,")");
							kostka::im2bw(im,DEFAULT_THRESHOLD);
						}
						else if(fileName_Filter[i].second=="gauss"){
							LOG("Call gauss(",_MAT(im),")");
							kostka::gauss(im);
						}
						
					}
					else if(fileName_Filter[i].second=="");
						 else
						 	ELOG("Invalid image filter name!");

					//assigning image to a current face
					LOG("Assigning texture to a cube's face with setTexture(",_MAT(im),") method");
					cubeObj.getFacePtr(i)->setTexture(im);
			}
			else{
				ELOG("Invalid image file name in the configuration text file!");
				free(tempPtr);
			}
		}
		LOG(_FORLOOPE);
		delete [] fileName_Filter; //freeing the memory
		LOG("Leaving loadConfigTxt() function");
	}

	void loadConfigXml(const char* filename, Cube& cubeObj)
	{
		int i=0;
		Mat im;
		using namespace pugi;
		LOG("filename = \"",filename,"\"");

		xml_document doc; 
		xml_parse_result result=doc.load_file(filename); //loading xml config file using pugi library function
		if(!result)
		{
			ELOG("Error reading ",filename," configuration file! ",result.description(),"!");
			cout << "\nError reading " << filename << " configuration file! " << result.description() << "!\n";
			LOG("Call exit(-1)");
			exit(-1);
		}


		LOG("i = ",i);
		LOG(_FORLOOP);
		for (xml_node photo = doc.child("Configuration").child("Photo"); photo; photo = photo.next_sibling("Photo"))
		{
			if(strcmp(photo.child_value(),"")) //if value not empty
			{
				im=imread(photo.child_value(),IMREAD_COLOR);
				LOG("photo = \"",photo.child_value(),"\"");
				LOG("im = ",_MAT(im));

				if(photo.attribute("graphicFilter").value())
				{
					LOG("photo.attribute(\"graphicFilter\") = ",photo.attribute("graphicFilter").value());
					if(!strcmp(photo.attribute("graphicFilter").value(),"thresholding")) //strcmp() returns 0 when given strings are equal
						{
							LOG("Call im2b(",_MAT(im),DEFAULT_THRESHOLD,")");
							im2bw(im,DEFAULT_THRESHOLD);
						}
					else if(!strcmp(photo.attribute("graphicFilter").value(),"gauss"))
							{
								LOG("Call gauss(",_MAT(im),")");
								gauss(im);
							}
				}
				LOG("Call cubeObj.getFacePtr(",i,")->setTexture(",_MAT(im),") method");
				cubeObj.getFacePtr(i)->setTexture(im);
			}
			i++;
		}
		LOG(_FORLOOPE);
		LOG("i = ",i);
		LOG("Leaving loadConfigXml() function");
	}


	int __length(const char* str) //calculates a zero-terminated string's length
	{
		int i=0,len=0;
		LOG("i = ",i," len = ",len," str = \"",str,"\"");
		LOG(_WHILELOOP);
		
		while(str[i++]!='\0') 
			len++;

		LOG(_WHILELOOPE);
		LOG("i = ",i," len = ",len," str = \"",str,"\"");
		LOG(_RET("_length()"),len);
		return len;
	}

	bool __isValidFileName(const char* filename)
	{
		int i,len,flag=1;
		LOG("flag = ",flag);
		LOG("filename = ",filename);

		LOG("Call __length(\"",filename,"\")");
		len=__length(filename);
		LOG("len = ",len);
		if(!len)
		{
			flag=0;
			goto loop_end;
		}

		LOG(_FORLOOP);
		for(i=0;i<len;i++)
			if((filename[i]<'0' || filename[i]>'9') && (filename[i]<'a' || filename[i]>'z') && (filename[i]<'A' || filename[i]>'Z') && filename[i]!='_' && filename[i]!='.'){
				flag=0;
				goto loop_end;
			}
		loop_end:
		LOG(_FORLOOPE);
		LOG(_RET("__isValidFileName()"),flag);
		return flag;
	}

	char __flipChar(char c) // fliping character function - if c is lowercase letter flip makes it uppercase and vice versa, if c is not a letter it is returned unchanged
	{
		if((c<'a' || c>'z') && (c<'A' || c>'Z')){ //check if c is a valid letter character
			ELOG("Not a valid letter character found!");
			LOG(_RET("__flipChar()"),c);
			return c;
		} 
		else{
			LOG(_RET("__flipChar()"),(char)(islower(c)?toupper(c):tolower(c)));			
			return islower(c)?toupper(c):tolower(c);
		}
	}

	bool __extCheck(const char* filename, const char* ext) //Checking if a .txt file
	{
		int len,i=0,j=0,state=1,flag=0;
		LOG("i = ",i," j = ",j," state = ",state," flag = ",flag);
		char temp[5]; //temporary string that will contain the file's extension

		LOG("Call __length(\"",ext,"\")");
		if(__length(ext)!=3) {ELOG("Only 3-character-long extensions accepted!"); LOG(_RET("__extCheck()"),0); return 0;} //o nly 3-character-long extensions accepted
		LOG("Call __isValidFileName(\"",filename,"\")");
		if(!__isValidFileName(filename)) {ELOG("Unsupported file extension!"); LOG(_RET("__extCheck()"),0); return 0;} //in case of bad filename exit the function

		LOG("Call __length(\"",filename,"\")");
		len=__length(filename);
		LOG("len = ",len);
		if(len<5){ //file names together with an extension cannot be shorter than 5 characters
			LOG(_RET("__extCheck()"),"0");
			return 0;
		}
		i=len-5; //going back within the string by 5 characters
		LOG("i = ",i);
		LOG(_WHILELOOP);
		while(filename[i++]!='\0')
			temp[j++]=filename[i];
		LOG(_WHILELOOPE);

		LOG("State automata switch within a loop");
		LOG(_FORLOOP);
		for(;;) // state automata
		{
			LOG(_SWITCH);	
			switch(state)
			{
				case 1:
				if(temp[0]=='.') state=2;
				else if(temp[0]!='.') state=7; 
				break;

				case 2:
				if(temp[1]==ext[0]||temp[1]==__flipChar(ext[0])) state=3;
				else if(temp[1]!=ext[0]&&temp[1]!=__flipChar(ext[0])) state=7; 
				break;

				case 3:
				if(temp[2]==ext[1]||temp[2]==__flipChar(ext[1])) state=4;
				else if(temp[2]!=ext[1]&&temp[2]!=__flipChar(ext[1])) state=7; 
				break;

				case 4:
				if(temp[3]==ext[2]||temp[3]==__flipChar(ext[2])) state=5;
				else if(temp[3]!=ext[2]&&temp[3]!=__flipChar(ext[2])) state=7; 
				break;

				case 5:
				if(1) {LOG(_RET("__extCheck()"),"1"); return 1; state=6;} break; // specified file extension found

				case 6:
				break;

				case 7:
				if(1) {LOG(_RET("__extCheck()"),"0"); return 0; state=6;} break; // specified file extension not found
			}
			LOG(_SWITCHE);
		}
		LOG(_FORLOOPE);	
	}

	short _checkFileType(char* filename) //checks if the file name extension is .txt or .xml, if none of them returns 0, if .txt 1 and .xml 2
	{
		LOG("Call __extCheck(\"",filename,"\"","\"txt\")");		
		if(__extCheck(filename,"txt")) {LOG(_RET("_checkFileType()"),1); return 1;}
		else{
			LOG("Call __extCheck(\"",filename,"\"","\"xml\")");		

			if(__extCheck(filename,"xml")) {LOG(_RET("_checkFileType()"),2); return 2;}
			else {LOG(_RET("_checkFileType()"),0); return 0;}
		}
	}

	int countCheck(const int value, char** argv) //checks input count for the main function
	{
		short iFlaga;

		if(value>=3) {LOG(_RET("countCheck()"),0); return 0;}
		else
			if(value<3)
			{
				if(value==1) {LOG(_RET("countCheck()"),1); return 1;}
				else
					if(value==2){
						LOG("Call _checkFileType(\"",argv[1],"\")");
						iFlaga=_checkFileType(argv[1]);
						if(!iFlaga) {LOG(_RET("countCheck()"),2); return 2;}
						else{
							if(iFlaga==1) {LOG(_RET("countCheck()"),3); return 3;}
							else {
								if(iFlaga==2) {LOG(_RET("countCheck()"),4); return 4;}
							}
						}
					} 
			}
			else{
				ELOG("Function countCheck() unknown error");
				return(-1);
			}
		return 0;
	}

} //end of namespace kostka definition

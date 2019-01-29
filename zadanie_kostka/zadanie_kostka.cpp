/*==================================================================/
Application for rotating a cube using OpenCV library			 	/
Author: Bartosz Pacuła												/
Date: January, 2019													/
===================================================================*/
// zadanie_kostka.cpp
//

#include "pch.h"

#ifndef GCC_COMPILER
	// #define GCC_COMPILER
#endif

#define _MAT(X) "Mat",(X).size(),"x[",(X).channels(),"C]"
#define _RETURN "return value = "
#define _RET(XX) "Function ",(XX)," return value = "
#define _FORLOOP "Entering FOR loop"
#define _FORLOOPE "Just left FOR loop"
#define _WHILELOOP "Entering WHILE loop"
#define _WHILELOOPE "Just left WHILE loop"
#define _SWITCH "Entering SWITCH"
#define _SWITCHE "Just left SWITCH"

#ifndef IO_HEADERS
	#define IO_HEADERS
	#include "iostream"
	#include "fstream"  //for file input-output functionality
#endif

#ifndef MEM_CTIME_HEADERS
	#define MEM_CTIME_HEADERS
	#include "memory"   // unique_ptr handling
	#include "ctime"
#endif

#ifndef PUGIXML_HEADER
	#define PUGIXML_HEADER
	#include "pugixml.hpp" // header file for xml support
#endif

#include "logger.hpp"  // logging utility
#include "kostka.hpp"  // contains main classes and helper functions

using namespace std;
using namespace kostka;
// using namespace logging;


int main(int argc, char** argv)
{
	double alfa, beta, gamma; //angle values for axis x, y, z
	char key;
	Mat obraz; //result image of a cube

	for(int i=0;i<argc;i++){
		LOG("argv[",i,"] = \"",*(argv+i),"\"");
	}
	LOG("Call countCheck(",argc,",char** argv)");

	int action=countCheck(argc,argv); //checking the argument count
	LOG("action = ",action);

	Cube cubeObj; //creates a default cube


	//taking a decision knowing the input command----------------------------------------
	if(!action) {LOG("Call help()"); help(); LOG("Call exit(",-1,")"); ELOG("Error taking action, action = ",action); exit(-1);}
	else
		if(action==1) ; //default action with no configuration information
		else
			if(action==2) {cout << "\nBad input file format!\n"; LOG("Call help()"); help(); LOG("Call exit(",-1,")"); exit(-1);}
			else
				if(action==3) {LOG("Call loadConfigTxt(",argv[1],",Cube)"); loadConfigTxt(argv[1],cubeObj);}
				else
					if(action==4) {LOG("Call loadConfigXml(",argv[1],",Cube)"); loadConfigXml(argv[1],cubeObj);}
	//-----------------------------------------------------------------------------------

	//initial values of angles
	alfa = 30.0; //in degrees (rotation against x axis) 
	beta = 30.0; //in degrees (rotation against y axis)
	gamma = 0.0; //in degrees (rotation against z axis)
	LOG("alfa = ",alfa," beta = ",beta," gamma = ",gamma);

	LOG("Call about()");
	about(); //shows some information about this application

	LOG("Call updateView(Cube",",",alfa,",",beta,",",gamma,")");
	obraz = updateView(cubeObj, alfa, beta, gamma); //an update of the cube's view (rotation of a cube against particular axis and creation of the cube's image (angles values in degrees)
	LOG("obraz = ",_MAT(obraz));

	LOG("Call imshow(const char*,",_MAT(obraz),")");
	imshow("Kostka v.1.1 by Bartosz Pacula -rot. ctrl keys: a-z (x-axis), s-x (y axis), d-c (z axis), q: quit", obraz); //initial display of the cube's image

	LOG(_FORLOOP);
	for(;;)
	{
		key=waitKey(0); //gets control key

		switch(key)
		{
			//Upper case characters added in case someone pressed a CapsLock
			case 'q': 
			LOG("Main return value = ",0," -End of Log");
			return 0; 
			break; //exits the program
			
			case 'Q':
			LOG("Main return value = ",0," -End of Log");
			return 0; 
			break; //exits the program

			case 'a': 
			alfa+=5; break;
			
			case 'A':
			alfa+=5; break;

			case 'z': 
			alfa-=5; break;
			
			case 'Z':
			alfa-=5; break;

			case 's': 
			beta+=5; break;
			
			case 'S':
			beta+=5; break;

			case 'x': 
			beta-=5; break;
			
			case 'X':
			beta-=5; break;

			case 'd': 
			gamma+=5; break;
			
			case 'D':
			gamma+=5; break;

			case 'c': 
			gamma-=5; break;
			
			case 'C':
			gamma-=5; break;

			default: break;
		}
		
		obraz = updateView(cubeObj, alfa, beta, gamma); //an update of the cube's view (rotation of a cube against particular axis and creation of the cube's image (angles values in degrees)
		imshow("Kostka v.1.1 by Bartosz Pacula -rot. ctrl keys: a-z (x-axis), s-x (y axis), d-c (z axis), q: quit", obraz);
	}


	return 0;
}

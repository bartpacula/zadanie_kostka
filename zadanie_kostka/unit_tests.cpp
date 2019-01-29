#include "pch.h"

#define BOOST_TEST_MODULE kostka
#include "boost/test/unit_test.hpp"

// #define GCC_COMPILER

#define _MAT(X) "Mat",(X).size(),"x[",(X).channels(),"C]"
#define _RETURN "return value = "
#define _RET(XX) "Function ",(XX)," return value = "
#define _FORLOOP "Entering FOR loop"
#define _FORLOOPE "Just left FOR loop"
#define _WHILELOOP "Entering WHILE loop"
#define _WHILELOOPE "Just left WHILE loop"
#define _SWITCH "Entering SWITCH"
#define _SWITCHE "Just left SWITCH"

#include "logger.hpp"
#include "kostka.hpp"

using namespace std;

namespace unit_tests{



	int __length(const char* str) //calculates a zero-terminated string's length
	{
		int i=0,len=0;
		while(str[i++]!='\0')
			len++;
		return len;
	}

	bool __isValidFileName(const char* filename)
	{
		int i,len,flag=1;
		len=__length(filename);
		if(!len)
		{
			flag=0;
			goto loop_end;
		}
		for(i=0;i<len;i++)
			if((filename[i]<'0' || filename[i]>'9') && (filename[i]<'a' || filename[i]>'z') && (filename[i]<'A' || filename[i]>'Z') && filename[i]!='_' && filename[i]!='.'){
				flag=0;
				goto loop_end;
			}
		loop_end:
		return flag;
	}

	char __flipChar(char c) // fliping character function - if c is lowercase letter flip makes it uppercase and vice versa, if c is not a letter it is returned unchanged
	{
		if((c<'a' || c>'z') && (c<'A' || c>'Z')) //check if c is a valid letter character
			return c;
		else
			return islower(c)?toupper(c):tolower(c);
	}

	bool __extCheck(const char* filename, const char* ext) //Checking if a .txt file
	{
		int len,i=0,j=0,state=1;
		char temp[5]; //temporary string that will contain the file's extension

		if(__length(ext)!=3) return 0; //only 3-character-long extensions accepted
		if(!__isValidFileName(filename)) return 0; //in case of bad filename exit the function

		len=__length(filename);

		if(len<5){ //file names together with an extension cannot be shorter than 5 characters
			return 0;
		}

		i=len-5; //going back within the string by 5 characters
		while(filename[i++]!='\0')
			temp[j++]=filename[i];

		for(;;) // state automata
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
				if(1) {return 1; state=6;} break; // .txt extension found

				case 6:
				break;

				case 7:
				if(1) {return 0; state=6;} break; // .txt extension not found
			}
	}


	struct LCT_Data{ //helper structure for data gathering in __loadConfigTxt()

		cv::Mat im;
		int i,j;
		vector<string> buffer;
		string temp;
		pair<string,string> fileName_Filter[8];
		ifstream *configFile;
		bool im2bwFlag=0;
		bool gaussFlag=0;
	}data;


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

	void test_loadConfigTxt(const char* filename, kostka::Cube& cubeObj) //loads display image and filter configuration from a text file
	{
		cv::Mat im;
		uint i,j;
		vector<string> buffer;
		string temp;

		ifstream configFile(filename,ios::in);
		data.configFile=&configFile;

		if(!configFile.is_open())
		{
			cout << "\nError reading " << filename << " configuration file!\n";
		}
		else
		{	

			i=0;
			data.i=i;
			while(!configFile.eof())
			{
				//each line of the input file is being put into the buffer vector
				getline(configFile,temp);
				buffer.push_back(temp);
				data.buffer=buffer;
				//---------------------------------------------------------------
				i++;
			}
			configFile.close();
			data.configFile=&configFile;
		}

		pair<string,string> *fileName_Filter=new pair<string, string>[buffer.size()];

		for(i=0;i<buffer.size();i++)
		{

			fileName_Filter[i].first="";
			data.fileName_Filter[i].first=fileName_Filter[i].first;
			fileName_Filter[i].second="";
			data.fileName_Filter[i].second=fileName_Filter[i].second;
			j=0;
			data.j=j;

			if(buffer[i].size()==0) goto end_of_this_loop;

			while(!(buffer[i][j]!=' ' && buffer[i][j]!='\t') && j<buffer[i].size()){ //skipping white spaces
				j++;
			}

			while((buffer[i][j]!=' ' && buffer[i][j]!='\t') && j<buffer[i].size())
			{
				fileName_Filter[i].first+=buffer[i][j];
				data.fileName_Filter[i].first=fileName_Filter[i].first;
				j++;
			}
			while(!(buffer[i][j]!=' ' && buffer[i][j]!='\t') && j<buffer[i].size()){ //skipping white spaces
				j++;
			}

			while((buffer[i][j]!=' ' && buffer[i][j]!='\t') && j<buffer[i].size()){

				fileName_Filter[i].second+=buffer[i][j];
				data.fileName_Filter[i].second=fileName_Filter[i].second;
				j++;
			}
		}
		end_of_this_loop:

		j=(buffer.size()<=6)?buffer.size():6; //loading more than 6 images is forbidden no matter how many entries are there within the input file
		char *tempPtr; //for memory leaks handling
		for(i=0;i<j;i++)
		{
			if(__isValidImgFileName(tempPtr=__str2charStr(fileName_Filter[i].first)))
			{
				free(tempPtr);
				im=cv::imread(fileName_Filter[i].first,cv::IMREAD_COLOR); //reading image file
					data.im=im.clone();
					//optional filtering--
					if(__isValidFilter(tempPtr=__str2charStr(fileName_Filter[i].second)))
					{
						if(fileName_Filter[i].second=="thresholding"){
							data.im2bwFlag=1;
							kostka::im2bw(im,DEFAULT_THRESHOLD);
							data.im=im.clone();
						}
						else if(fileName_Filter[i].second=="gauss"){
							data.gaussFlag=1;
							kostka::gauss(im);
							data.im=im.clone();
						}
						
					}
					else if(fileName_Filter[i].second=="");
						 else
						 	// ELOG("Invalid image filter name!")
						 	;

					//assigning image to a current face
					cubeObj.getFacePtr(i)->setTexture(im);
			}
			else{
				// ELOG("Invalid image file name in the configuration text file!");
				free(tempPtr);
			}

		}
	delete [] fileName_Filter;
	}



	BOOST_AUTO_TEST_SUITE(Helper_Functions_Suite)

		BOOST_AUTO_TEST_CASE(__length_Function_Test)
		{
			BOOST_CHECK_EQUAL(__length("abc") , 3);
			BOOST_CHECK_EQUAL(__length("") , 0);
			BOOST_CHECK_EQUAL(__length(" d9#&SP SDF jsa") , 15);
			BOOST_CHECK_EQUAL(__length("?\\ \t \n \b\r ") , 10);
			BOOST_CHECK_EQUAL(__length("?\\ \t \0 \b\r ") , 5);
		}

		BOOST_AUTO_TEST_CASE(__extCheck_Function_Test)
		{
			BOOST_CHECK(__extCheck("abc" , "txt") == 0);
			BOOST_CHECK(__extCheck("" , "txt") == 0);
			BOOST_CHECK(__extCheck("abc." , "txt") == 0);
			BOOST_CHECK_MESSAGE(__extCheck("." , "txt") == 0, "File name cannot contain the extension or dot only!");
			BOOST_CHECK_MESSAGE(__extCheck(".txt." , "txt") == 0, "This program forbids file names with a dot at the begining!");
			BOOST_CHECK(__extCheck("abc.jpg" , "txt") == 0);
			BOOST_CHECK(__extCheck("abc. txt" , "txt") == 0);
			BOOST_CHECK(__extCheck("abc.Tx t" , "txt") == 0);
			BOOST_CHECK(__extCheck("abc.txt" , "txt") == 1);
			BOOST_CHECK(__extCheck("abc.TXT" , "txt") == 1);
			BOOST_CHECK(__extCheck("abc.tXt" , "txt") == 1);
			BOOST_CHECK(__extCheck("abc.TxT" , "txt") == 1);
			BOOST_CHECK_MESSAGE(__extCheck(".txt.txt" , "txt") == 1, "This program forbids file names with a dot at the begining!");
			BOOST_CHECK(__extCheck("abc.xml" , "txt") == 0);
			BOOST_CHECK_MESSAGE(__extCheck(".txt" , "txt") == 0, "File name cannot contain the extension or dot only!");
			BOOST_CHECK_MESSAGE(__extCheck(".TXT" , "txt") == 0, "File name cannot contain the extension or dot only!");
			BOOST_CHECK_MESSAGE(__extCheck(".t x t" , "txt") == 0, "File name cannot contain the extension or dot only!");
			BOOST_CHECK(__extCheck("\\\t.txt" , "txt") == 0);
			BOOST_CHECK(__extCheck("\\.txt" , "txt") == 0);
			BOOST_CHECK(__extCheck("\n\b\r\0\".txt" , "txt") == 0);
			BOOST_CHECK(__extCheck("\n\b\r\".txt" , "txt") == 0);
			BOOST_CHECK(__extCheck("\".txt" , "txt") == 0);
			BOOST_CHECK_MESSAGE(__extCheck("d d.txt" , "txt") == 0, "File name cannot contain spaces!");
			BOOST_CHECK_MESSAGE(__extCheck("d .txt" , "txt") == 0, "File name cannot contain spaces!");
			BOOST_CHECK_MESSAGE(__extCheck(" d .txt" , "txt") == 0, "File name cannot contain spaces!");
			BOOST_CHECK_MESSAGE(__extCheck(" .txt" , "txt") == 0, "File name cannot contain spaces!");
			BOOST_CHECK(__extCheck("`!@#$%&*()}|{[]'<>/~.txt" , "txt") == 0);
			BOOST_CHECK(__extCheck("abc" , "xml") == 0);
			BOOST_CHECK(__extCheck("" , "xml") == 0);
			BOOST_CHECK(__extCheck("abc." , "xml") == 0);
			BOOST_CHECK_MESSAGE(__extCheck("." , "xml") == 0, "File name cannot contain the extension or dot only!");
			BOOST_CHECK(__extCheck("`!@#$%&*()}|{[]'<>/~.txt" , "xml") == 0);
			BOOST_CHECK_MESSAGE(__extCheck(" .txt" , "xml") == 0, "File name cannot contain spaces!");
			BOOST_CHECK(__extCheck("abc.xml" , "xml") == 1);
			BOOST_CHECK(__extCheck("abc__abc.xml" , "xml") == 1);
			BOOST_CHECK(__extCheck("ABC__abc.XML" , "xml") == 1);
			BOOST_CHECK_MESSAGE(__extCheck(".xml" , "xml") == 0, "File name cannot contain the extension or dot only!");
			BOOST_CHECK_MESSAGE(__extCheck(".XML" , "xml") == 0, "File name cannot contain the extension or dot only!");
			BOOST_CHECK_MESSAGE(__extCheck(".x m l" , "xml") == 0, "File name cannot contain the extension or dot only!");
			BOOST_CHECK(__extCheck("\\\t.xml" , "xml") == 0);
			BOOST_CHECK(__extCheck("\\.xml" , "xml") == 0);
			BOOST_CHECK(__extCheck("\n\b\r\0\".xml" , "xml") == 0);
			BOOST_CHECK(__extCheck("\n\b\r\".xml" , "xml") == 0);
			BOOST_CHECK(__extCheck("\".xml" , "xml") == 0);
			BOOST_CHECK_MESSAGE(__extCheck("d d.xml" , "xml") == 0, "File name cannot contain spaces!");
			BOOST_CHECK_MESSAGE(__extCheck("d .xml" , "xml") == 0, "File name cannot contain spaces!");
			BOOST_CHECK_MESSAGE(__extCheck(" d .xml" , "xml") == 0, "File name cannot contain spaces!");
			BOOST_CHECK_MESSAGE(__extCheck(" .xml" , "xml") == 0, "File name cannot contain spaces!");

			BOOST_CHECK_MESSAGE(__extCheck("abc.conf" , "conf") == 0, "Only 3-character-long extensions accepted!");
			BOOST_CHECK_MESSAGE(__extCheck("abc.conf" , "CONF") == 0, "Only 3-character-long extensions accepted!");
			BOOST_CHECK_MESSAGE(__extCheck("abc.conf" , "a") == 0, "Only 3-character-long extensions accepted!");
			BOOST_CHECK_MESSAGE(__extCheck("abc.conf" , "MD") == 0, "Only 3-character-long extensions accepted!");
			BOOST_CHECK_MESSAGE(__extCheck("abc.MD" , "MD") == 0, "Only 3-character-long extensions accepted!");
			BOOST_CHECK_MESSAGE(__extCheck("abc.com" , "MD") == 0, "Only 3-character-long extensions accepted!");
			BOOST_CHECK_MESSAGE(__extCheck("abc.com" , "co") == 0, "Only 3-character-long extensions accepted!");
			BOOST_CHECK(__extCheck("abc.jp" , "jpg") == 0);

			BOOST_CHECK(__extCheck("abc.jpg" , "jpg") == 1);
			BOOST_CHECK(__extCheck("abc_.png" , "png") == 1);
			BOOST_CHECK(__extCheck("abc_cad.TIF" , "tif") == 1);
			BOOST_CHECK(__extCheck("abc.jpg" , "JPG") == 1);
			BOOST_CHECK(__extCheck("abc_.png" , "PNG") == 1);
			BOOST_CHECK(__extCheck("abc_cad.TIF" , "TIF") == 1);
			BOOST_CHECK(__extCheck("abc_cad.gif" , "gif") == 1);

		}

		BOOST_AUTO_TEST_CASE(strcmp_Function_Test)
		{
			string str1="-a89vasdf ajpar daf9";
			char* cstr1=__str2charStr(str1);
			BOOST_CHECK_EQUAL(strcmp(cstr1,"-a89vasdf ajpar daf9"),0);
			BOOST_CHECK_EQUAL(__length("-a89vasdf ajpar daf9"),20);
			BOOST_CHECK_EQUAL(str1.size(),(uint)20);
			
			string str2="@)#($*%^_#";
			char* cstr2=__str2charStr(str2);
			BOOST_CHECK(!strcmp(cstr2,"@)#($*%^_#"));
			BOOST_CHECK_EQUAL(__length("@)#($*%^_#"),10);
			BOOST_CHECK_EQUAL(str2.size(),(uint)10);
			
			string str3="()()(2324***af))";
			char* cstr3=__str2charStr(str3);
			BOOST_CHECK(!strcmp(cstr3,"()()(2324***af))"));
			BOOST_CHECK_EQUAL(__length("()()(2324***af))"),16);
			BOOST_CHECK_EQUAL(str3.size(),(uint)16);
			
			string str4="\n \t \\ ASDF";
			char* cstr4=__str2charStr(str4);
			BOOST_CHECK(!strcmp(cstr4,"\n \t \\ ASDF"));
			BOOST_CHECK_EQUAL(__length("\n \t \\ ASDF"),10);
			BOOST_CHECK_EQUAL(str4.size(),(uint)10);

			//memory freeing ---
			free(cstr1);
			free(cstr2);
			free(cstr3);
			free(cstr4);
		}


		BOOST_AUTO_TEST_CASE(__str2charStr_Function_Test)
		{
			char *pStr;

			string str="abc";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"abc"));
			free(pStr);

			str="-a89vasdf ajpar daf9";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"-a89vasdf ajpar daf9"));
			free(pStr);

			str="@)#($*%^_#";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"@)#($*%^_#"));
			free(pStr);

			str="-_-sdfaj asd aj2 je";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"-_-sdfaj asd aj2 je"));
			free(pStr);

			str="()()(2324***af))";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"()()(2324***af))"));
			free(pStr);

			str="%%%%";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"%%%%"));
			free(pStr);

			str="$$$ $#";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"$$$ $#"));
			free(pStr);

			str="@%  0x0";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"@%  0x0"));
			free(pStr);

			str="";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),""));
			free(pStr);

			str="// \\ ";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"// \\ "));
			free(pStr);

			str="|\t\3\2\r\b";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"|\t\3\2\r\b"));
			free(pStr);

			str="\n";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"\n"));
			free(pStr);

			str="\"    \0 sdf";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"\"    \0 sdf"));
			free(pStr);

			str="\n \t \\ ASDF";
			BOOST_CHECK(!strcmp(pStr=__str2charStr(str),"\n \t \\ ASDF"));
			free(pStr);

		}


		BOOST_AUTO_TEST_CASE(loadConfigTxt_Function_Test)
		{
			kostka::Cube cubeObj;

			const char* filename="config.txt"; //this test will succeed for the original content of config.txt only

			BOOST_CHECK(&cubeObj != 0);
			test_loadConfigTxt(filename,cubeObj);

			BOOST_CHECK(data.i==0);
			BOOST_CHECK(data.j==0);

			BOOST_WARN(data.buffer.size()==6); //actually it's a number of lines of the input file (basically there should be equal to 6)

			BOOST_CHECK(&(data.im)!=0);
			BOOST_CHECK(data.im.channels()==3);
			BOOST_CHECK(data.im.cols==300);

			for(int i=0;i<6;i++){
				BOOST_CHECK(cubeObj.getFacePtr(i)!=0); //check for valid pointers of the cube's faces
			}

			string col1[]={
				"Obraz1.png","gauss","Obraz2.png","","Obraz3.png","thresholding","Obraz4.png","","Obraz5.png","thresholding","Obraz6.png",""
			};
			string col2[]={
				data.fileName_Filter[0].first,
				data.fileName_Filter[0].second,
				data.fileName_Filter[1].first,
				data.fileName_Filter[1].second,
				data.fileName_Filter[2].first,
				data.fileName_Filter[2].second,
				data.fileName_Filter[3].first,
				data.fileName_Filter[3].second,
				data.fileName_Filter[4].first,
				data.fileName_Filter[4].second,
				data.fileName_Filter[5].first,
				data.fileName_Filter[5].second
			};

			BOOST_CHECK_EQUAL_COLLECTIONS(col1,col1+12,col2,col2+12);
			BOOST_CHECK(data.configFile!=0);
			
			//converting strings to cstrings for compatibility reasons------------------------
			char* charFName[6];
			char* tempFNamePtr[6];  //temporary pointers just to prevent memory leaks
			for(int i=0;i<6;i++){
				charFName[i]=(char*)malloc(sizeof(char)*(data.fileName_Filter[i].first.size()+1));
				tempFNamePtr[i]=charFName[i];
				charFName[i]=__str2charStr(data.fileName_Filter[i].first);
			}
			//--------------------------------------------------------------------------------
			for(int i=0;i<6;i++){
				BOOST_CHECK_MESSAGE(__isValidFileName(charFName[i])==1,"String \"" << charFName[i] << "\" is not a valid file name!");
				if(!__extCheck(charFName[i],"png"))
					BOOST_CHECK_MESSAGE(__extCheck(charFName[i],"jpg")==1,"File \"" << charFName[i] << "\" is not a .jpg file!");
				if(!__extCheck(charFName[i],"jpg"))
					BOOST_CHECK_MESSAGE(__extCheck(charFName[i],"png")==1,"File \"" << charFName[i] << "\" is not a .png file!");
			}
			//--------------------------------------------------------------------------------

			//converting strings to cstrings for compatibility reasons------------------------
			char* charFilterName[6];
			char* tempFilterNamePtr[6]; //temporary pointers just to prevent memory leaks
			for(int i=0;i<6;i++){
				charFilterName[i]=(char*)malloc(sizeof(char)*(data.fileName_Filter[i].second.size()+1));
				tempFilterNamePtr[i]=charFilterName[i];
				charFilterName[i]=__str2charStr(data.fileName_Filter[i].second);
			}
			//--------------------------------------------------------------------------------
			for(int i=0;i<6;i++){
				BOOST_CHECK_MESSAGE(__isValidFilter(charFilterName[i]),"String \"" << charFilterName[i] << "\" is not a valid filter name!");
			}
			//--------------------------------------------------------------------------------
			
			//memory freeing--------------
			for (int i = 0; i < 6; i++) {
				free(charFName[i]);
				free(charFilterName[i]);
				free(tempFNamePtr[i]);
				free(tempFilterNamePtr[i]);
			}
		}

	BOOST_AUTO_TEST_SUITE_END()


}//end of namespace unit_tests














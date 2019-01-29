### Zadanie - Kostka
## Author: Bartosz Pacuła 
* email: [bartpacula@yahoo.com]<mailto:bartpacula@yahoo.com>
* LinkedIn: <https://www.linkedin.com/in/bartoszpacula>
## About
* Date: January, 2019 
* Version: 1.1
* OpenCV library version: 3.2.0
* PugiXml library version: 1.9 
* Boost library version: 1.69.0
* IDE: Visual Studio: Microsoft Visual Studio Community 2017 v.15.9.4
* Aplikacja konsoli Windows (architektura: win32)

A cube of mapped bitmaps is being displayed using a perspective warp function of OpenCV library. The cube can be rotated in 3D using control keys.
If a configuration file (.txt or .xml) is provided the program starts with textures loaded based on information within the file. Additional image filtering is possible. Supported filter types are: thresholding and gauss blur filter (namely: `thresholding` and `gauss`).

The syntax of *config.txt* file is:
    
    image_file filter_name

The filter name is optional.

The syntax of config.xml file is:

    <Configuration>
     
		<Photo graphicFilter="filter_name">image_file</Photo>
		
	</Configuration>

The graphicFilter attribute is optional as well. Every `<Photo>` tag entry has to come within the `<Configuragion>` tag. There's only one `<Configuration>` tag allowed in the configuration file but many `<Photo>` tags are allowed. If the number of them exceeds six the further entries will not be processed. If no configuration file's given the default textures are generated.


Thresholding threshold value is fixed at 128 intensity level. Gauss filter default kernel size is 3.

The application provides 2-level logging. The configuration of the logger is set within *logger_config.xml* file.

The output can be set between a console and a file output. There levels of logging are: `trace` or `error`. File logging errors only is set by default. The default output file name is *execution.log*. For more information please read *logger_config.xml* file. XML file support has been coded with PugiXML library.

To test the application features unit test cases have been provided. There are 5 of them witin *unit_tests.exe* file (*unit_tests.cpp*). The tests use Boost library test framework.

## Additional information

**Rotation control:**

(make sure the window displaying the cube is active)

"a" or "z" - rotation against x-axis

"s" or "x" - rotation against y-axis

"d" or "c" - rotation against z-axis

"q" - quit the program

Rotation step is 5 degrees.

**The environment setup:**

The environmental PATH variable has to be set in order to find all OpenCV library .dll files needed by the application.

These are following files:

*opencv_core320.dll*

*opencv_imgproc320.dll*

*opencv_highgui320.dll*

*opencv_imgcodecs320.dll*

*opencv_calib3d320.dll*

I can add them into this repository if requested.

## Application files:

### C++ source and header files:

*zadanie_kostka.cpp*

*unit_tests.cpp*

*kostka.hpp*

*logger.hpp*

### Configuration files:

*config.txt*

*config.xml*

*logger_config.xml*

### Additional image files:

These are colour images 300x300 pixels each. 

*Obraz1.png*
*Obraz2.png*
*Obraz3.png*
*Obraz4.png*
*Obraz5.png*
*Obraz6.png*
*Obraz7.png*
*Obraz8.png*
*Obraz9.png*
*Obraz10.png*

*Obraz1.jpg*
*Obraz2.jpg*
*Obraz3.jpg*
*Obraz4.jpg*
*Obraz5.jpg*
*Obraz6.jpg*
*Obraz7.jpg*
*Obraz8.jpg*
*Obraz9.jpg*
*Obraz10.jpg*

## Version change log

###version: 1.1

*January, 2019*

* Loading cube textures using configuration files (i.e. *config.txt* or *config.xml*) added.
* Optional Gauss blur and thresholding filters added.
* 2-level logger added (with configuration from an xml file).
* Unit tests added.
* Minor code issues fixed.

###version: 1.0

*January, 2019*

* The first release. Displaying the cube with default perspective-warped textures and optional rotating it in 3 dimensions.

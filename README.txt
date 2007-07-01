#####################################################
#
#	Readme Cmake file 
#
# 	Project		ARTools
#	author		Henry Portilla
#	place		Barcelona, Spain
#	date		july/1/2007
#	Special 
#	Grant
#
####################################################
#	Project information
####################################################

INSTRUCTIONS

This project use cmake utility to generate makefiles 
on linux systems by now, but can be extended to windows
systems.

the instructions to generates an executable from this dir are

1. Do a checkout from the right folder in the repository in your console

	Example:
	$ svn checkout file:///home/svn/ARtools/RTSPClient/trunk myFolder

2. Create a folder called build within Myfolder

2. Go to the build folder of your recently working copy myFolder
   and executes cmake with -G KDevelop to create a kdevelop project

	Example:
	myFolder/build$ cmake -G KDevelop3 ../

4. Open the generated project with kdevelop and then you will have a copy 
   ready to build and to work with it

	Note: you have to build the project to generate the makefile file
	      after that you will do the make to generate the executable

EXTRA INFORMATION

A. you need add the following arguments in the executable line

	$  ./ARtools  rtsp-path1 rtsp-path2 IOD_distance

	$ ./ARtools rtsp://sonar:7070/cam0 rtsp://sonar:7070/cam3 1.5
	note:  you can try from cam0 to cam3

B. Don forget that you need to be running a video server based on rtsp and rtp

	Note:
	In this case we are using spook from www.litech.org in a different machine
	with a mpeg4 capture card
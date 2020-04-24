This file explains how to use the Visual Studio pre-compiled version of the GL4Dummies Library to program with under Windows 64bits.
By Farès Belhadj, April 24, 2020 - amsi@up8.edu

In order to install and work with GL4Dummies using Visual Studio, you should:
* Copy the DLLs included in "DLLs" into: 
	1) copy all DLLs from DLLs/x86/ to "%systemroot%\SysWoW64" (usualy "C:\Windows\SysWOW64\)
	2) copy all DLLs from DLLs/x64/ to "%systemroot%\System32" (usualy "C:\Windows\System32\)
* Copy the GL4D directory into "C:\", so as to have a directory named "C:\GL4D\" on your computer(**)
* Go to GL4Dummies github, download a sample project into your work directory, 
	use, if available, the sln file - the Visual Studio Solution file - in order to 
	compile and run the sample, otherwise, create your own project and set the good paths for the 
	include and lib directories (x64 for x64 plateform and x86 for Win32 plateform).

(**) You can also modify your Visual Studio options in order to respectively add 
	"C:\GL4D\include;C:\GL4D\include\SDL2" as an additional include directory 
	and "C:\GL4D\lib\x64" & "C:\GL4D\lib\x86" as an additional library directory.

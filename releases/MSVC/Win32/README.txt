This file explains how to use the Visual Studio pre-compiled version of the GL4Dummies Library to program with.
By Farès Belhadj, March 06, 2020 - amsi@up8.edu

In order to install and work with GL4Dummies using Visual Studio, you should:
* Copy the DLLs included in "CopyToSysWOW64_or_System32" into: 
	1) "%systemroot%\SysWoW64" (usualy "C:\Windows\SysWOW64\), if your Windows is 64-bit
	2) "%systemroot%\System32" (usualy "C:\Windows\System32\), if your Windows is 32-bit
* Copy the GL4D directory into "C:\", so as to have a directory named "C:\GL4D\" on your computer(**)
* Go to GL4Dummies github, download a sample project into your work directory, 
	use the sln file - the Visual Studio Solution file (if there is one, 
	if not take inspiration from samples\sample2d_00-1_0.sln) in order to 
	compile and run the sample.

(**) You can also modify your Visual Studio options in order to respectively add 
	"C:\GL4D\include;C:\GL4D\include\SDL2" as an additional include directory 
	and "C:\GL4D\lib" as an additional library directory.

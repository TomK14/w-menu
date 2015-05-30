wMenu's source code is currently hosted at [Googlecode](http://code.google.com/p/w-menu/source/list).

## Compiling on Windows ##

This part of the article deals with compilation under a Windows environment.

Thanks to JGE++, wMenu can be compiled for both Windows and the PSP. Compiling a Windows version helps a lot for debugging.

### Compiling for Windows ###

  1. Download and install Microsoft's [Visual C++ Express 2010](http://www.microsoft.com/express/Downloads/) (freeware)
  1. Download and install an SVN client - [Tortoise](http://tortoisesvn.tigris.org) is a popular one, and step 4 of this guide assumes that you're using either Tortoise or another SVN client that integrates with Windows Explorer.
  1. Somewhere on your hard disk, make a new folder for the wMenu source code. I'll call that folder "wmenuTheSource" in this guide, but you can (and may even want to ;) ) choose a different name.
  1. In Windows Explorer, right-click on the folder "wmenuTheSource". A context menu pops up. Click on "SVN Checkout...".
  1. A dialogue box pops up. In this dialogue box, under "URL of repository", enter the following address: http://w-menu.googlecode.com/svn/trunk/ . Don't change any other settings, and click on "Ok".
  1. Tortoise will now download the latest revision of the wMenu source code to the folder "wmenuTheSource". _Note: If you use any other method to download the source, make sure that you get the source code for wMenu **and** the source code for JGE. If you've followed these instructions, then you don't have to worry about that._
  1. Start Visual C++ Express. In the top menu bar, click on "File -> Open -> Project/Solution". Navigate to the file "wmenuTheSource\projects\wmenu\mtg\_vs2010.sln" and open it.
  1. Visual C++ Express will now read the structure of the source code files and folders, and display that structure in the "Solution Explorer" to the left of the screen.
  1. In the Solution Explorer, right-click on the topmost entry (which should read "Solution 'wmenu' (2 projects)"). A context menu opens, click on "Clean Solution". The Output Window at the bottom on the screen should now show the information "Clean: 2 succeeded, 0 failed, 0 skipped"
  1. Still in the Solution Explorer, right-click on the folder "wmenu" and choose "Build". Wait while Visual C++ compiles the source code. You will probably see some warnings (which you can ignore at this stage). When the process has finished, the bottom line in the output window should start with "Build: 2 succeeded, 0 failed". If the build has failed instead, then TODO.
  1. You have now successfully built the program from the SVN sources.

  1. Enjoy the bleeding edge of wMenu development. Whenever you want to update wMenu, just right-click on the "wmenuTheSource" folder in Windows Explorer, and click on "SVN Update". Tortoise will then automatically update all files that need to. Then repeat steps 10 and 11. If wmenu does not work afterwards, repeat steps 9 to 11. If nothing else helps, one thing to try is to rename the wmenuTheSource folder, create a new one, and checkout the complete sources again as per step (4).

_Note 2: Compiling the sources for the first time may take quite long. Compiling them again after an update will be much quicker, since the compiler will only update the files that are affected by the changes._

### Compiling for the PSP/Vita ###

To run wMenu on the PSP, you need to compile a new EBOOT.PBP. The method described here has successfully been tested on Vista and Windows XP machines (with version 0.11.1 of the minimalist PSP SDK). Please report any issues or success!

  1. Obtain the wMenu source  code as described in steps 2-6 of the "Compiling for Windows" guide above. Make sure that you get both the "JGE" and the "projects" directory.
  1. Download and install a PSPSDK environment for windows, such as the [Minimalist PSP SDK](http://sourceforge.net/project/showfiles.php?group_id=223830) by Heimdall. Let the PSPSDK add its path to the system paths (the Minimalist SDK has an option for that which is on by default, just don't turn it off).
  1. Using a DOS command line shell, navigate to the JGE directory, and type "make clean" (and ENTER). This is to remove the old JGE library. (_Note: If you get a message "command not found" at this stage, then the PSPSDK's path information has not been installed correctly; repeat step (2)._)
  1. In the same directory, type "make" (and ENTER). It should create some files. The important one is named libjge300.a in the lib/psp/ folder. If there is no lib/psp/libjge300.a file in your JGE directory, or if this file is old, then something went bad.
  1. Go to the directory projects/wmenu with a DOS shell
  1. Type "make clean" (and ENTER)
  1. Type "make" (and ENTER). This should create an EBOOT.PBP file in your projects/wmenu folder. That's it!

_Note: If there haven't been any changes to the JGE library itself, then you obviously don't need to recreate it each time, so steps 3 and 4 are not necessary in this case. If you're not sure whether you need to compile JGE, then it's better to do it, to be on the safe side._

_Note 2: The "make clean" step itself is usually not necessary, but recommended if you are not a C++ guru._

_Note 3: If you don't know what a DOS shell is and how to use it, here are some quick instructions:_
  * _To open the DOS shell, click on "Start" in the Windows task bar, then click on "Run", type **cmd** and click on "Ok". This will open a black DOS window with a command line prompt._
  * _To navigate to another drive, type that drive's letter, followed by a colon, and press Enter._
  * _To navigate to a specific directory, type **cd**, press space, then type a **\** sign, and then type the name that directory and press Enter._
  * 
#Release notes for release 0.9.2

##Change Log for Release 0.9.2

Apart from a few minor modifications this is essentially a long overdue bug fix release.

##Installation of Lava/LavaPE 

##A) How to install the Windows binary release of LavaPE

Simply launch the downloaded lavape-0.9.2-win32-bin.exe file and follow the instructions of the installer.

Windows 8: If the "Windows SmartScreen Filter" has been activated in the system settings (that's the default) and prevents execution of the LavaPE installer, then you can click the "More information" link in the pop-up message of the SmartScreen Filter and then the "execute nevertheless" button to enforce the installation.

But this works only if you have administrator privileges. If not, then the "Windows SmartScreen Filter" has to be completely deactivated in the system settings first.

##B) How to install the Mac OS binary release of LavaPE

Download the Mac OS version of the Qt libraries from

* http://qt-project.org/downloads

and install them in the default location.

Download the lavape-0.9.2-macos.zip archive and uncompress it by double-clicking.

A double-click on LavaPE.app or Lava.app will launch the **Lava Programming Environment** or the **Lava** Interpreter, respectively.

A number of sample programs can be opened in LavaPE.app or Lava.app from the "samples" subdirectory of the installation directory "lavape".

##C) How to install LavaPE from the source distribution on LINUX platforms

Download the compressed tar file lavape-0.9.2-src.tar.bz2 of the **LavaPE** source distribution and store it in the parent directory of the envisaged lavape installation directory.

Uncompress it using the command

    tar xjvf lavape-0.9.2-src.tar.bz2

Or download a snapshot of the most recent developer version of **LavaPE** from our GIT repository at 

    http://sourceforge.net/p/lavape/code/ci/master/tree/
    
(or just go to the "Code" section from our SourceForge project summary page).

You need to have GNU gcc/make and the Qt4 libraries and development tools.

Current Linux distributions would offer a software package comprising a rather current version of the Qt4 libraries and the corresponding build tools, particularly qmake, moc, uic, Qt Assistant, and Qt Designer.

Then build the **LavaPE** executables as follows:

    cd lavape
    qmake lavape.pro
    make

On some platforms the Qt4 version of qmake has a more specific name, like "qmake-qt4".

The command

    "qmake -v"
    
will display the Qt version used by qmake.

The build process will take quite a while. You will find the executables of the **Lava** Programming Environment in lavape/bin thereafter: **LavaPE**, **Lava**, and **LPC** (= **Lava** Precompiler, needed only during the build process.)

Go to the lavape/bin directory, launch "./LavaPE", consult the Online Help, open some sample programs from the lavape/samples directory and enjoy the unique structure editing features of **LavaPE**.

You can run the **Lava** interpreter "Lava"

1. either by executing "./Lava" in the lavape/bin directory and opening a .lava or .ldoc file from the file menu or from the file open button, or
2. from **LavaPE** by clicking the "running man" toolbutton, after you have activated a view containing the executable code of a main program or a declaration tree view that contains a main program declaration.

We'd appreciate very much if you would inform us about any problems that you have encounterd during the installation or while using
**Lava/LavaPE**.
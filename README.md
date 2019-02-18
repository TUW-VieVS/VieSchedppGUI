# VieSched++ Graphical User Interface (GUI)

written by Matthias Schartner
contact: matthias.schartner@geo.tuwien.ac.at

# License
> VieSched++ Very Long Baseline Interferometry (VLBI) Scheduling Software
>
> Copyright (C) 2018  Matthias Schartner
>
> This program is free software: you can redistribute it and/or modify
> it under the terms of the GNU General Public License as published by
> the Free Software Foundation, either version 3 of the License, or
> (at your option) any later version.
>
> This program is distributed in the hope that it will be useful,
> but WITHOUT ANY WARRANTY; without even the implied warranty of
> MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
> GNU General Public License for more details.
>
> You should have received a copy of the GNU General Public License
> along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Installation

The software consists of two parts:
* VieSched++: the scheduler, hostet on https://github.com/TUW-VieVS/VieSchedpp
* VieSched++ GUI: the graphical user interface (this repository)

__It is necessary to install the scheduler independently!__

Have a look at https://github.com/TUW-VieVS/VieSchedpp/blob/master/README.md for installing the scheduler.


## Installation of VieSched++ GUI

This section descripes how to install __VieSched++ GUI__ on LINUX. 

While an installation is possible on Windows (tested with Windows 10) it is no
further discussed due to the many possibilieties of different compilers and options.

Pre-build binaries are available for github releases. Visit https://github.com/TUW-VieVS/VieSchedppGUI/releases.

If you run into any problems compiling VieSched++ GUI, contact me under matthias.schartner@geo.tuwien.ac.at and I can send you some pre-build binaries.

The source code is hosted at two git repositories:

https://github.com/TUW-VieVS/VieSchedppGUI.git (public)
https://git.geo.tuwien.ac.at/vievs/VieSchedpp/VieSchedppGUI.git (inside TU Vienna GEO Domain)


## Recommended software folder structure for VieSched++

The full __VieSched++__ software contains 4 different packages which are all available in git under `git.geo.tuwien.ac.at/vievs/VieSchedpp/`

It is recommended to copy the source code into one common directory here called `VieSchedppSource`

    VieSchedppSource/VieSchedpp
    VieSchedppSource/VieSchedppGUI
    VieSchedppSource/IAU_SOFA (only for installation)
    VieSchedppSource/CATALOGS (only for usage)

## Dependencies

I have the VieSchedpp installed before installing the VieSchedppGUI. Please visit https://github.com/TUW-VieVS/VieSchedpp for the software.

* QT and QT charts (tested with 5.11.2)
 * possible installation on Ubuntu: `sudo apt-get install qt5-default libqt5charts5 libqt5charts5-dev` or visit https://www.qt.io/download for GUI version (might take longer to download and install)
* SOFA libraries
  * check out git project `https://git.geo.tuwien.ac.at/vievs/VieSchedpp/IAU_SOFA.git` (Inside TU-Wien GEO domain) or visit http://www.iausofa.org/. 
  * The SOFA library has to be locate in `../IAU_SOFA/Release`, otherwise change search path in `VieSchedppGUI.pro` file manually
* maybe OpenGL (depending on your linux version)
 * possible installation on Ubuntu: `sudo apt-get install libgl1-mesa-dev`
* VieSched++ source files
 * available via the VieSched++ git repository at https://github.com/TUW-VieVS/VieSchedpp

## Build
clone git repository into `VieSchedppSource` or copy source code into `VieSchedppSource/VieSchedppGUI`

    cd VieSchedppSource
    git clone https://github.com/TUW-VieVS/VieSchedppGUI.git

Browse to newly created folder `VieSchedppGUI`

    cd VieSchedppGUI
    
 ## Option 1: Command-line
Run qmake

    qmake -config release VieSchedppGUI.pro

Run make (this process might take a few moments)

    make 

You should now see the 'VieSchedppGUI' file in the directory. Run the GUI using command

    VieSchedppGUI

 ## Option 2: Qt5 GUI
* After QT have been installed. Open QT interface

* File > Open File or Project > "your VieSchedppGUI.pro file"

* Build > Build project "VieSchedppGUI" (this process might take a few moments)

Now you should have a new directory created on the same level as the VieSchedppGUI folder

    cd "path to your new folder"
    VieSchedppGUI

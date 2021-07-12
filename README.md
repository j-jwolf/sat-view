# Satellite Image of Geolocation
## John Wolf, v 0.1.0
***
# Table of Contents
1. Overview
2. How to use
3. Installation
4. Troubleshooting

## API Key Note:
* Using this program as is will limit the number of times you can run main.py per hour. For most users, this shouldn't be an issue. But if you want to run the program more than 30 times per hour <b><u>OR</b></u> 50 times per day from the same id, please consider getting an API key from <a href="https://api.nasa.gov/">NASA's API Website</a>

# Overview
This program requests a satellite image from NASA's Earth API using your IP's geolocation. The install binary file creates a folder in your user documents folder as well as writes a few Python scripts to that location
Once installed, use the main.py Python script to use the program

# How to Use
Use one of the install binary files (more information below) to install the files into your user profile's Documents folder. Then move into the 'SatelliteImage' folder in Documents and run the main.py file

# Installation

## Installing using binary:

### For Windows Users:
Run the install.exe file

### For Debian-based Linux Distributions:
Run the install.bin file

This program will not work for other Linux distributions in its current state

### For Mac OS users:
This program has not been tested on Mac OS. In theory, install.bin should work on Mac but until it has been tested I cannot recommend running it

## Installing via compiling source code:
Download the zip file and in the terminal(or PowerShell with Windows) move to the src folder and run the following:
```
gcc -O3 install.c -o install.ext
```
Replace the ext with exe if you are on Windows and on Linux/Mac OS you can use any binary extension you'd like. You can even leave the .ext part out. I used .bin to make it easier to manage two binary files in the repository

# Troubleshooting
Common issues:

* The program installed correctly but the main.py script is getting a satellite image of somewhere I'm not!
	* Check if you are using a VPN or similar service. These will change your outbound IP address. If, for example, you live in Boston but use VPN and are using a London server, your IP address will be geolocated to London
* I can't run the install.exe file!
	* There could be a few explanations for this
		1. Is your operating system Windows? Install.exe is compiled for windows specifically and will not run on any other OS.
		2. Do you have a C/C++ compiler? Windows does not include one and you will need one to run this program. There are a few options for compilers, MinGW (32/64) or Cygwin
			* It's personal preference on which to install. I am using MinGW-64 on Windows 10

## To report bugs:
* Email me at <a href="mailto:jjwolf0330@tutanota.com">jjwolf@tutanota.com</a> and try to include as much information as you can
	### What to include in a bug report?
	1. The encountered bug
	2. If possible, steps that led to the bug/crash
	3. Your operating system and version of python
		* Note: your version of gcc/g++ would be helpful but is not required

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// OS checks
#ifdef _WIN32
#define WIN 1
#else
#define WIN 0
#endif

#ifdef __APPLE__
#define MAC 1
#else
#define MAC 0
#endif
//============

// Util macros
#define MAX_BUFFER (1 << 20)
#define fl(i, e) for(i = 0; i < e; i++)
//============

// structs
struct File
{
	char* fn;
	char* content;
};

// functions
void destroy(void* ptr)
{
	free(ptr);
	ptr = NULL;
}
void makeFile(const char* fn, const char* content)
{
	size_t nameLen = strlen(fn), dataLen = strlen(content);
	struct File file;
	file.fn = malloc(sizeof(char)*(nameLen+1));
	strcpy(file.fn, fn);
	file.fn[nameLen++] = '\0';
	file.content = malloc(sizeof(char)*(dataLen+1));
	strcpy(file.content, content);
	file.content[dataLen++] = '\0';
	FILE* fp = fopen(file.fn, "w");
	fputs(file.content, fp);
	fclose(fp);
	destroy(file.fn);
	destroy(file.content);
}
int callProcess(const char* command)
{
	FILE* pipe = popen(command, "w");
	return pclose(pipe);
}
char* readFile(const char* fn)
{
	char* buffer = NULL;
	FILE* fp = fopen(fn, "r");
	if(fp)
	{
		if(!fseek(fp, 0L, SEEK_END))
		{
			long buffersize = ftell(fp);
			if(buffersize != -1)
			{
				buffer = malloc(sizeof(char)*(buffersize+1));
				if(fseek(fp, 0L, SEEK_SET)) {destroy(buffer);}
				else
				{
					size_t len = fread(buffer, sizeof(char), buffersize, fp);
					if(ferror(fp)) {destroy(buffer);}
					else {buffer[len++] = '\0';}
				}
			}
		}
		fclose(fp);
	}
	return buffer;
}
char* mergeToPath(const char* path, const char* fn)
{
	size_t len = strlen(path)+strlen(fn), overhead;
	if(WIN) {overhead = 3;}
	else {overhead = 2;}
	char* buffer = malloc(sizeof(char)*(len+overhead));
	strcpy(buffer, path);
	if(WIN) {strcat(buffer, "\\");}
	else {strcat(buffer, "/");}
	strcat(buffer, fn);
	buffer[len+(overhead-1)] = '\0';
	return buffer;
}
int displayWarning()
{
	char userInput, message[] = "Does this operating system meet this requirement? y/n: ";
	puts("This program currently only works with Ubuntu/Debian-based Linux distros");
	printf("%s", message);
	do
	{
		userInput = getchar();
		if(userInput != 'y' && userInput != 'n') {printf("%c is an invalid input, enter 'y' or 'n'\n%s", userInput, message);}
	} while(userInput != 'y' && userInput != 'n');
	return userInput == 'y' ? 1 : 0;
}
int runPyScript(const char* path, const char* fn)
{
	char* pycall = NULL;
	if(WIN) {pycall = "python ";}
	else {pycall = "python3 ";}
	size_t pyLen = strlen(pycall), pathLen = strlen(path), fnLen = strlen(fn), overhead;
	if(WIN) {overhead = 3;}
	else {overhead = 2;}
	size_t total = pyLen+pathLen+fnLen+overhead;
	char command[total+overhead];
	strcpy(command, pycall);
	strcat(command, path);
	if(WIN) {strcat(command, "\\");}
	else {strcat(command, "/");}
	strcat(command, fn);
	command[total+(overhead-1)] = '\0';
	printf("Calling %s\n", command);
	FILE* pipe = popen(command, "w");
	return pclose(pipe);
}
void makeKeyScript()
{
	char folderName[] = "SatelliteImage", *path = getenv("HOME"), *slash = NULL, fn[] = "addApiKey.py";
	size_t folderLen = strlen(folderName), pathLen = strlen(path), fnLen = strlen(fn), overhead, total = folderLen+pathLen+fnLen;
	if(WIN)
	{
		overhead = 5;
		slash = "\\";
	}
	else
	{
		overhead = 3;
		slash = "/";
	}
	char loc[total+overhead];
	strcpy(loc, path);
	strcat(loc, slash);
	strcat(loc, folderName);
	strcat(loc, fn);
	loc[total+(overhead-1)] = '\0';
	char content[] = "";
	makeFile(loc, content);
}
// main
int main()
{
	if(!WIN && !MAC)
	{
		if(!displayWarning()) {exit(-1);}
	}
	struct File install;
	char* up = getenv("HOME"), *fileLoc = NULL;
	size_t upl = strlen(up)+1, overhead;
	if(WIN) {overhead = 3;}
	else {overhead = 2;}
	char userProfile[upl+overhead];
	strcpy(userProfile, up);
	if(WIN) {strcat(userProfile, "\\");}
	else {strcat(userProfile, "/");}
	userProfile[upl+(overhead-1)] = '\0';
	size_t userLen = strlen(userProfile), locLen;
	if(WIN) // windows machine (32 or 64-bit machine)
	{
		char fn[] = "install.bat";
		char content[] = "@echo off\nset rootFolder=%USERPROFILE%\\Documents\nset mainFolder=%rootFolder%\\SatelliteImage\nset srcFolder=%mainFolder%\\src\nIF NOT EXIST %mainFolder% (mkdir %mainFolder )\nIF NOT EXIST %srcFolder% ( mkdir %srcFolder% )\nENDLOCAL";
		makeFile(fn, content);
		fileLoc = "Documents\\SatelliteImage\\src";
		locLen = strlen(fileLoc);
	}
	else // not windows
	{
		char fn[] = "install.sh";
		char* content = NULL;
		// mac os machine
		if(MAC) {content = "#!/bin/bash\ntest -e ~/Documents/SatelliteImage && echo directory exists || mkdir ~/Documents/SatelliteImage\ntest -e ~/Documents/SatelliteImage/src && echo directory exists || mkdir ~/Documents/SatelliteImage/src";}
		// not mac (assuming linux at this point but to be safe, add checks for ios and other operating systems
		else
		{
			char pkgcheck[] = "pkgchk.sh";
			char data[] = "#!/bin/bash\nREQUIRED_PACKAGE=\"imagemagick\"\nPKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PACKAGE|grep \"install ok installed\")\necho Checking for $REQUIRED_PACKAGE: $PKG_OK\nif test \"$PKG_OK\" = \"\"\nthen\n\tsudo apt update && sudo apt-get --yes install $REQUIRED_PACKAGE\nelse\n\techo \"$REQUIRED_PACKAGE WAS FOUND\"\nfi\n";
			makeFile(pkgcheck, data);
			content = "#!/bin/bash\ntest -e ~/Documents/SatelliteImage && echo file exists || mkdir ~/Documents/SatelliteImage\ntest -e ~/Documents/SatelliteImage/src && echo file exists || mkdir ~/Documents/SatelliteImage/src\n";
			callProcess("sh pkgchk.sh");
			callProcess("rm pkgchk.sh");
		}
		makeFile(fn, content);
		fileLoc = "Documents/SatelliteImage/src";
		locLen = strlen(fileLoc);
	}
	fflush(stdout);
	size_t srcLen = locLen+userLen;
	char srcFolder[srcLen+1];
	strcpy(srcFolder, userProfile);
	strcat(srcFolder, fileLoc);
	srcFolder[srcLen++] = '\0';
	char* command = NULL;
	if(WIN) {command = "install.bat";}
	else {command = "sh install.sh";}
	int res = callProcess(command);
	if(res) {printf("%s returned value %d\n", command, res);}
	else {printf("%s ran successfully\n", command);}
	if(WIN) {callProcess("del install.bat");}
	else {callProcess("rm install.sh");}
	makeFile(mergeToPath(srcFolder, "ipinfo.py"), "import os, sys\nfrom cryptography.fernet import Fernet\n\nhomePath = os.environ['HOME']\nfn = homePath+'/Documents/SatelliteImage/ipinfo.txt'\nos.system('curl -s https://ipinfo.io > '+fn)\nkfn = homePath+'/Documents/SatelliteImage/key.key'\nkey = Fernet.generate_key()\nwith open(kfn, 'wb') as file: file.write(key)\nf = Fernet(key)\nwith open(fn, 'rb') as file: data = f.encrypt(file.read())\nwith open(fn, 'wb') as file: file.write(data)\n");
	res = runPyScript(srcFolder, "ipinfo.py");
	if(res) {printf("ipinfo.py returned value %d\n", res);}
	else {printf("ipinfo.py executed successfully");}
	fflush(stdout);
	makeFile(mergeToPath(srcFolder, "main.py"), "import os, sys, json\nfrom ast import literal_eval\nfrom urllib.request import urlopen, Request\nfrom urllib.parse import urlencode\nfrom cryptography.fernet import Fernet\nfrom PIL import Image\n\nheader={'User-Agent': 'Mozilla/5.0'}\ndef encodeUrl(url, data):\n\tif(data == None): return url\n\tparsedData = urlencode(data, True)\n\tif(not url.endswith('?')): url += '?'\n\treturn url+parsedData\ndef getCoords():\n\tkfn = '../key.key'\n\tfn = '../ipinfo.txt'\n\tif(not os.path.isfile(kfn)):\n\t\tkey = Fernet.generate_key()\n\t\twith open(kfn, 'wb') as file: file.write(key)\n\telse:\n\t\twith open(kfn, 'rb') as file: key = file.read()\n\tf = Fernet(key)\n\tif(not os.path.isfile(fn)):\n\t\tos.system('curl -s https://ipinfo.io > '+fn)\n\t\twith open(fn, 'rb') as file: data = f.encrypt(file.read())\n\t\twith open(fn, 'wb') as file: file.write(data)\n\twith open(fn, 'rb') as file: data = json.loads(f.decrypt(file.read()))\n\treturn data['loc']\ndef handleError(error):\n\tprint(error)\n\tsys.exit()\n\treturn\ndef urlJson(url):\n\ttry:\n\t\trequest = Request(url, headers=header)\n\t\twith urlopen(request) as response: data = json.loads(response.read().decode('utf-8'))\n\texcept Exception as e: handleError(e)\n\treturn data\ndef urlBinary(url):\n\ttry:\n\t\trequest = Request(url, headers=header)\n\t\twith urlopen(request) as response: data = response.read()\n\texcept Exception as e: handleError(e)\n\treturn data\ndef getApiKey():\n\tfn = '../apikey.txt'\n\tif(not os.path.isfile(fn)):\n\t\tprint('No API key found, using demo key. Please consider getting an API key from https://api.nasa.gov')\n\t\treturn 'DEMO_KEY'\n\telse:\n\t\twith open(fn) as file: key = file.read()\n\t\treturn key\ndef main():\n\tbaseUrl = 'https://api.nasa.gov/planetary/earth/assets'\n\tcoords = getCoords().split(',')\n\tmyHeader = {'lon': coords[1], 'lat': coords[0], 'api_key': getApiKey(), 'dim': '0.2'}\n\turl = encodeUrl(baseUrl, myHeader)\n\tdirList = urlJson(url)\n\timgLink = dirList['url']\n\ttry:\n\t\timgData = urlBinary(imgLink)\n\t\twith open('test.jpg', 'wb') as file: file.write(imgData)\n\t\tprint('Saved image from', imgLink, 'to test.jpg successfully')\n\texcept Exception as e: handleError(e)\n\ttry:\n\t\timage = Image.open('test.jpg')\n\t\timage.show()\n\t\timage.close()\n\texcept Exception as e: handleError(e)\n\treturn 0\nres = main()\nif(res): print('Main returned', res)\n");
	makeFile(mergeToPath(srcFolder, "addApiKey.py"), "import os, sys\n\nfn = '../apikey.txt'\nargs = sys.argv\nif(len(args) != 2):\n\tprint('Invalid arguments')\n\tsys.exit()\nkey = args[1]\nif(not os.path.isfile(fn)):\n\twith open(fn, 'w') as file: file.write(key)\n");
	printf("Installed files to %s", srcFolder);
	if(!WIN && !MAC) {printf(" and installed required package 'imagemagick'");}
	puts("\n");
	fflush(stdout);
	return 0;
}

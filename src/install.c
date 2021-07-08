#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define WIN 1
#else
#define WIN 0
#endif

#define MAX_BUFFER (1 << 20)
#define fl(i, e) for(i = 0; i < e; i++)

struct File
{
	char* fn;
	char* content;
};

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
int main()
{
	struct File install;
	if(WIN)
	{
		char fn[] = "install.bat";
		char content[] = "@echo off\nset rootFolder=%USERPROFILE%\\Documents\nset mainFolder=%rootFolder%\\SatelliteImage\nset srcFolder=%mainFolder%\\src\nIF NOT EXIST %mainFolder% (mkdir %mainFolder )\nIF NOT EXIST %srcFolder% ( mkdir %srcFolder% )\nENDLOCAL";
		makeFile(fn, content);
	}
	else
	{
		char fn[] = "install.sh";
		char content[] = "test -e ~/Documents/SatelliteImage && echo file exists || mkdir ~/Documents/SatelliteImage\ntest -e ~/Documents/SatelliteImage/src && echo file exists || mkdir ~/Documents/SatelliteImage/src\nsudo apt update && sudo apt install -y imagemagick";
		makeFile(fn, content);
	}
	char* command = NULL;
	if(WIN) {command = "install.bat";}
	else {command = "sh install.sh";}
	int res = callProcess(command);
	if(res) {printf("%s returned value %d\n", command, res);}
	else {printf("%s ran successfully\n", command);}
	makeFile("ipinfo.py", "import os, sys\nfrom cryptography.fernet import Fernet\n\nhomePath = os.environ['HOME']\nfn = homePath+'/Documents/SatelliteImage/ipinfo.txt'\nos.system('curl -s https://ipinfo.io > '+fn)\nkfn = homePath+'/Documents/SatelliteImage/key.key'\nkey = Fernet.generate_key()\nwith open(kfn, 'wb') as file: file.write(key)\nf = Fernet(key)\nwith open(fn, 'rb') as file: data = f.encrypt(file.read())\nwith open(fn, 'wb') as file: file.write(data)\n");
	if(WIN) {command = "python ipinfo.py";}
	else {command = "python3 ipinfo.py";}
	res = callProcess(command);
	if(res) {printf("%s returned value %d\n", command, res);}
	else {printf("%s executed successfully\n", command);}
	makeFile("main.py", "import os, sys, json\nfrom ast import literal_eval\nfrom urllib.request import urlopen, Request\nfrom urllib.parse import urlencode\nfrom cryptography.fernet import Fernet\nfrom PIL import Image\n\nheader={'User-Agent': 'Mozilla/5.0'}\ndef encodeUrl(url, data):\n\tif(data == None): return url\n\tparsedData = urlencode(data, True)\n\tif(not url.endswith('?')): url += '?'\n\treturn url+parsedData\ndef getCoords():\n\tkfn = 'key.key'\n\tfn = 'ipinfo.txt'\n\tif(not os.path.isfile(kfn)):\n\t\tkey = Fernet.generate_key()\n\t\twith open(kfn, 'wb') as file: file.write(key)\n\telse:\n\t\twith open(kfn, 'rb') as file: key = file.read()\n\tf = Fernet(key)\n\tif(not os.path.isfile(fn)):\n\t\tos.system('curl -s https://ipinfo.io > '+fn)\n\t\twith open(fn, 'rb') as file: data = f.encrypt(file.read())\n\t\twith open(fn, 'wb') as file: file.write(data)\n\twith open(fn, 'rb') as file: data = json.loads(f.decrypt(file.read()))\n\treturn data['loc']\ndef handleError(error):\n\tprint(error)\n\tsys.exit()\n\treturn\ndef urlJson(url):\n\ttry:\n\t\trequest = Request(url, headers=header)\n\t\twith urlopen(request) as response: data = json.loads(response.read().decode('utf-8'))\n\texcept Exception as e: handleError(e)\n\treturn data\ndef urlBinary(url):\n\ttry:\n\t\trequest = Request(url, headers=header)\n\t\twith urlopen(request) as response: data = response.read()\n\texcept Exception as e: handleError(e)\n\treturn data\ndef main():\n\tbaseUrl = 'https://api.nasa.gov/planetary/earth/assets'\n\tcoords = getCoords().split(',')\n\tmyHeader = {'lon': coords[1], 'lat': coords[0], 'api_key': 'DEMO_KEY', 'dim': '0.2'}\n\turl = encodeUrl(baseUrl, myHeader)\n\tdirList = urlJson(url)\n\timgLink = dirList['url']\n\ttry:\n\t\timgData = urlBinary(imgLink)\n\t\twith open('test.jpg', 'wb') as file: file.write(imgData)\n\t\tprint('Saved image from', imgLink, 'to test.jpg successfully')\n\texcept Exception as e: handleError(e)\n\ttry:\n\t\timage = Image.open('test.jpg')\n\t\timage.show()\n\t\timage.close()\n\texcept Exception as e: handleError(e)\n\treturn 0\nres = main()\nif(res): print('Main returned', res)\n");
	return 0;
}

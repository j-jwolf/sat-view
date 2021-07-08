import os, sys, json
from ast import literal_eval
from urllib.request import urlopen, Request
from urllib.parse import urlencode
from cryptography.fernet import Fernet
from PIL import Image

header={'User-Agent': 'Mozilla/5.0'}
def encodeUrl(url, data):
	if(data == None): return url
	parsedData = urlencode(data, True)
	if(not url.endswith('?')): url += '?'
	return url+parsedData
def getCoords():
	kfn = 'key.key'
	fn = 'ipinfo.txt'
	if(not os.path.isfile(kfn)):
		key = Fernet.generate_key()
		with open(kfn, 'wb') as file: file.write(key)
	else:
		with open(kfn, 'rb') as file: key = file.read()
	f = Fernet(key)
	if(not os.path.isfile(fn)):
		os.system('curl -s https://ipinfo.io > '+fn)
		with open(fn, 'rb') as file: data = f.encrypt(file.read())
		with open(fn, 'wb') as file: file.write(data)
	with open(fn, 'rb') as file: data = json.loads(f.decrypt(file.read()))
	return data['loc']
def handleError(error):
	print(error)
	sys.exit()
	return
def urlJson(url):
	try:
		request = Request(url, headers=header)
		with urlopen(request) as response: data = json.loads(response.read().decode('utf-8'))
	except Exception as e: handleError(e)
	return data
def urlBinary(url):
	try:
		request = Request(url, headers=header)
		with urlopen(request) as response: data = response.read()
	except Exception as e: handleError(e)
	return data
def main():
	baseUrl = 'https://api.nasa.gov/planetary/earth/assets'
	coords = getCoords().split(',')
	myHeader = {'lon': coords[1], 'lat': coords[0], 'api_key': 'DEMO_KEY', 'dim': '0.2'}
	url = encodeUrl(baseUrl, myHeader)
	dirList = urlJson(url)
	imgLink = dirList['url']
	try:
		imgData = urlBinary(imgLink)
		with open('test.jpg', 'wb') as file: file.write(imgData)
		print('Saved image from', imgLink, 'to test.jpg successfully')
	except Exception as e: handleError(e)
	try:
		image = Image.open('test.jpg')
		image.show()
		image.close()
	except Exception as e: handleError(e)
	return 0
res = main()
if(res): print('Main returned', res)

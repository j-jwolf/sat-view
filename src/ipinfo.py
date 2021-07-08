import os, sys
from cryptography.fernet import Fernet

homePath = os.environ['HOME']
fn = homePath+'/Documents/SatelliteImage/ipinfo.txt'
os.system('curl -s https://ipinfo.io > '+fn)
kfn = homePath+'/Documents/SatelliteImage/key.key'
key = Fernet.generate_key()
with open(kfn, 'wb') as file: file.write(key)
f = Fernet(key)
with open(fn, 'rb') as file: data = f.encrypt(file.read())
with open(fn, 'wb') as file: file.write(data)

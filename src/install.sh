test -e ~/Documents/SatelliteImage && echo file exists || mkdir ~/Documents/SatelliteImage
test -e ~/Documents/SatelliteImage/src && echo file exists || mkdir ~/Documents/SatelliteImage/src
sudo apt update && sudo apt install -y imagemagick
#!/bin/bash

usage()
{
	echo "Usage   : $0 [ OPTIONS ]"
	echo "Options :  "
	echo "     -s : Setup Environment"
	echo "     -c : Clean"
}

echo "
                           ███████████   ██████████  █████                                  
                          ░░███░░░░░███ ░░███░░░░░█ ░░███                                   
  █████   ██████   ██████  ░███    ░███  ░███  █ ░  ███████    ██████   ████████  █████ ████
 ███░░   ███░░███ ███░░███ ░██████████   ░██████   ░░░███░    ░░░░░███ ░░███░░███░░███ ░███ 
░░█████ ░███████ ░███ ░░░  ░███░░░░░███  ░███░░█     ░███      ███████  ░███ ░░░  ░███ ░███ 
 ░░░░███░███░░░  ░███  ███ ░███    ░███  ░███ ░   █  ░███ ███ ███░░███  ░███      ░███ ░███ 
 ██████ ░░██████ ░░██████  █████   █████ ██████████  ░░█████ ░░████████ █████     ░░███████ 
░░░░░░   ░░░░░░   ░░░░░░  ░░░░░   ░░░░░ ░░░░░░░░░░    ░░░░░   ░░░░░░░░ ░░░░░       ░░░░░███ 
                                                                                   ███ ░███ 
                                                                                  ░░██████  
                                                                                   ░░░░░░   
"

if [ $# -eq 0 ]
then
	usage
	exit
fi

while getopts "sc" opt ;do
	case "${opt}" in
		s)
			echo "[+] Installing Intel PIN"
			URL=https://software.intel.com/sites/landingpage/pintool/downloads/pin-3.18-98332-gaebd7b1e6-gcc-linux.tar.gz

			wget $URL -O pin.tar.gz 
			tar -xvf pin.tar.gz
			rm pin.tar.gz
			#Install Ubuntu Dependencies
			sudo apt-get install gcc-multilib g++-multilib libc6-dev-i386
			#Rename pin directory
			mv pin-* pin
			;;
		c)
			rm -rf out/
			rm *.log
			cd test
			make clean
			cd ..
			;;
		*)
			echo "[+] Invalid Option"
			usage
			;;
	esac
done

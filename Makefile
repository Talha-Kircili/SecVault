.DEFAULT_GOAL := compile

backup:
	rsync -av --exclude='vault' --exclude='masterkey' --exclude='secvault' --exclude='access.log' ./ /media/sf_kali/ && clear

compile:
	g++ *.cpp -lssl -lcurl -lcrypto -o secvault && clear && ./secvault

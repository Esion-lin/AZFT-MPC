#! /bin/bash
if [ ! -d "/usr/include/json" ]; then
	cd scons-2.2.0
	python setup.py install
	cd ..
	cd jsoncpp-src-0.5.0
	scons platform=linux-gcc
	mv libs/linux-gcc-4.4.7/libjson_linux-gcc-4.4.7_libmt.so /lib
	ln /lib/libjson_linux-gcc-4.4.7_libmt.so /lib/libjson.so
	mv include/json/ /usr/include/
else
	echo "You have installed jsoncpp"
fi
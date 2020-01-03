#! /bin/bash
if [ ! -n "$1" ] ;then
    echo "you have not input java_file name!"
else
    if command -v javac >/dev/null 2>&1;
	then
		javac ./compiler/Protocols_Analysis/src/mixedProtocolsAnalysis/*.java -d . -cp .:./compiler/Protocols_Analysis/libs/junit-4.10.jar:./compiler/Protocols_Analysis/libs/sootclasses-trunk-sources.jar:./compiler/Protocols_Analysis/libs/sootclasses-trunk-jar-with-dependencies.jar:./compiler/Protocols_Analysis/libs/gson-2.8.5.jar
		javac $1/*.java
		java -cp .:./compiler/Protocols_Analysis/libs/junit-4.10.jar:./compiler/Protocols_Analysis/libs/sootclasses-trunk-sources.jar:./compiler/Protocols_Analysis/libs/sootclasses-trunk-jar-with-dependencies.jar:./compiler/Protocols_Analysis/libs/gson-2.8.5.jar mixedProtocolsAnalysis.Main -w -p wjop on -p wjop.si expansion-factor:1000000 -p wjop.si max-container-size:1000000 -p wjop.si max-inlinee-size:1000000 -f jimple -cp $1/:/usr/lib/jvm/java-8-openjdk-amd64/jre/lib/rt.jar:/usr/lib/jvm/java-8-openjdk-amd64/jre/lib/jce.jar P
	else
		apt-get install openjdk-8-jdk
		javac ./compiler/Protocols_Analysis/src/mixedProtocolsAnalysis/*.java -d . -cp .:./compiler/Protocols_Analysis/libs/junit-4.10.jar:./compiler/Protocols_Analysis/libs/sootclasses-trunk-sources.jar:./compiler/Protocols_Analysis/libs/sootclasses-trunk-jar-with-dependencies.jar:./compiler/Protocols_Analysis/libs/gson-2.8.5.jar
		javac $1/*.java
		java -cp .:./compiler/Protocols_Analysis/libs/junit-4.10.jar:./compiler/Protocols_Analysis/libs/sootclasses-trunk-sources.jar:./compiler/Protocols_Analysis/libs/sootclasses-trunk-jar-with-dependencies.jar:./compiler/Protocols_Analysis/libs/gson-2.8.5.jar mixedProtocolsAnalysis.Main -w -p wjop on -p wjop.si expansion-factor:1000000 -p wjop.si max-container-size:1000000 -p wjop.si max-inlinee-size:1000000 -f jimple -cp $1/:/usr/lib/jvm/java-8-openjdk-amd64/jre/lib/rt.jar:/usr/lib/jvm/java-8-openjdk-amd64/jre/lib/jce.jar P
	fi
	g++ -o file_deal _file_deal.cpp
	./file_deal
	g++ HW/crypto.cpp [^_]*.cpp ./HW/libSG.so -o main  -ljson  -L. -lpthread -g -lssl -lcrypto
fi



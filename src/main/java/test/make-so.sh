JAVA_HOME=/usr/lib/jvm/adoptopenjdk-11-hotspot
gcc -v -lc -shared -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux ../../c/ping.c test_JavaPing.c -o libJavaPing.so
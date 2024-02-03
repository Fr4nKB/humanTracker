test:
	g++ -o test pwmPI5.cpp test.cpp
	sudo ./test
	rm test

lib:
	g++ -DPYTHON_WRAPPER -c -Wall -Werror -fpic pwmPI5.cpp
	g++ -shared -o pwm.so pwmPI5.o
	rm pwmPI5.o

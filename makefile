VPATH = include
CPPFLAGS = -I include
LIBS = libcudd.a
CXX = g++ -g -Wall -Wextra

policy_checker: policy_checker.o
	g++ -g -Wall -Wextra policy_checker.o -o policy_checker -lm ./lib/libcudd.a

policy_checker.o policy_checker.d : policy_checker.cpp policy_checker.hpp rule.hpp 

clean:
	rm -rf *.o *.dSYM

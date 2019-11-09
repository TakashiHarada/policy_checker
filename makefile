VPATH = include
CPPFLAGS = -I include
LIBS = libcudd.a
CXX = g++ -std=c++11 -g -Wall -Wextra

policy_checker: policy_checker.o
	g++ -std=c++11 -g -Wall -Wextra policy_checker.o -o policy_checker -lm ./lib/libcudd.a

policy_checker.o policy_checker.d : policy_checker.cpp policy_checker.hpp rule.hpp 

sugar: policy_checker_sugar.o
	g++ -std=c++11 -g -Wall -Wextra policy_checker_sugar.o -o policy_checker_sugar -lm ./lib/libcudd.a

policy_checker_sugar.o policy_checker_sugar.d : policy_checker_sugar.cpp policy_checker.hpp rule.hpp

clean:
	rm -rf *.o *.dSYM

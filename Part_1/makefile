tester:phase1.o phase2.o tester.o
	g++ tester.o phase1.o phase2.o -o tester
phase1.o:phase1.cpp mainlib.h
	g++ -c phase1.cpp 
phase2.o:phase2.cpp mainlib.h
	g++ -c phase2.cpp 
tester.o:tester.cpp mainlib.h
	g++ -c tester.cpp 
.PHONY:clean

clean:
	rm *.o tester
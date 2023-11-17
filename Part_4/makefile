all: testcpu simulate
testcpu:CPU.v test_CPU.v 
	iverilog -o testcpu CPU.v test_CPU.v
simulate:
	vvp -n testcpu
gtkwave:
	gtkwave test_CPU.vcd


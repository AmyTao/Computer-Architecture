all: testalu simulate
testalu:ALU.v test_ALU.v 
	iverilog -o testalu ALU.v test_ALU.v
simulate:
	vvp -n testalu
gtkwave:
	gtkwave test_ALU.vcd
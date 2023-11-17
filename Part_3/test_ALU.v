`timescale 1ns/1ps

module alu_test;

reg[31:0] instruction,reg_A,reg_B;
wire[31:0] result;
wire[2:0] flags;

alu testalu(instruction,reg_A,reg_B,result,flags);

initial begin
    $dumpfile("test_ALU.vcd");
    $dumpvars(0,instruction,reg_A,reg_B,result,flags);
end


initial
begin
$monitor("%b:%b:%b:%b:%b:%b:%b",
instruction,instruction[31:26],instruction[5:0],testalu.rega,testalu.regb,testalu.flags,testalu.result);
#10 $display("Please noted that I type doesn't have function part, \nso the func shown in the I type is invalid");
#10 $display("add");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0001_0000_0000_0010_0000;
reg_A<=32'b0000_0000_0000_0000_0000_0000_1111_1111;
reg_B<=32'b0000_0000_0000_0000_0000_0000_0000_0001;

#10 $display("addi");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0010_0000_0000_0000_0111_1111_1110_0001;
reg_A<=32'b1000_0000_0000_0000_0000_0000_0000_0000;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;

#10 $display("addu");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0001_0000_0000_0010_0001;
reg_A<=32'b0000_0000_0000_0000_0000_0000_0000_1101;
reg_B<=32'b0000_0000_1111_1111_1111_1111_1111_0010;

#10 $display("addiu");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0010_0100_0010_0000_1111_1111_1111_0001;
reg_A<=32'b0000_0000_0000_0000_0000_0000_0000_1101;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;

#10 $display("sub");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0001_0000_0000_0010_0010;
reg_A<=32'b0000_0000_0000_0000_0000_0000_0000_0000;
reg_B<=32'b0001_1111_1111_1111_1111_1110_1110_0010;

#10 $display("subu");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0010_0000_0000_0000_0010_0011;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1111_1101_1101_1101_0010_0010_0010_0010;

#10 $display("and");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0001_0000_0000_0010_0100;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;
           
#10 $display("andi");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0011_0000_0000_0001_0100_1011_0010_0100;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;

#10 $display("nor");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0010_0000_0100_1000_0010_0111;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;

#10 $display("or");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0010_0000_0100_1000_0010_0101;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;
           
           
#10 $display("ori");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0011_0100_0010_0000_0101_1110_0010_0101;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;

#10 $display("xor");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0010_0000_0101_1000_0010_0110;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;
           
#10 $display("xori");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0011_1000_0000_0000_0101_1000_0010_0110;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;
                               
#10 $display("beq");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0001_0000_0010_0000_0000_0000_0000_0000;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;
                                        
#10 $display("bne");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0001_0100_0010_0000_0000_0000_0000_0000;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;

#10 $display("slt");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0010_0000_0000_0000_0010_1010;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b0111_1101_1101_1101_0010_0010_0010_0010;

#10 $display("slti");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0010_1000_0010_0000_0000_0000_0010_1010;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b0111_1101_1101_1101_0010_0010_0010_0010;

#10 $display("sltiu");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0010_1100_0010_0000_0000_0000_0010_1010;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b0000_1101_1101_1101_0010_0010_0010_0010;

#10 $display("sltu");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0010_0000_1000_0000_0010_1011;
reg_A<=32'b0000_0000_1111_1111_1111_1110_1110_0010;
reg_B<=32'b0000_0001_0001_1101_0010_0010_0010_0010;

#10 $display("lw");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b1000_1100_0010_0000_1000_0000_0010_1011;
reg_A<=32'b1000_0000_1111_1111_1111_1110_1110_0010;
reg_B<=32'b0000_0000_0000_1111_1111_1111_1111_1111;

#10 $display("sw");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b1010_1100_0010_0000_1000_0000_0010_1011;
reg_A<=32'b1000_0000_1111_1111_1111_1110_1110_0010;
reg_B<=32'b0000_0000_0000_1111_1111_1111_1111_1111;

#10 $display("srl");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0000_1000_0000_1100_0010;
reg_A<=32'b1000_0000_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1111_1111_1111_1111_1111_1111_1111_1111;

#10 $display("sra");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0000_1000_0000_1100_0011;
reg_A<=32'b1000_0000_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1111_1111_1111_1111_1111_1111_1111_1111;

#10 $display("srav");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0001_1000_0000_1100_0111;
reg_B<=32'b1000_0000_1111_1111_1111_1110_1110_0010;
reg_A<=32'b0000_0000_0000_0000_0000_0000_0111_1111;

#10 $display("srlv");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0001_1000_0000_1100_0110;
reg_B<=32'b1000_0000_1111_1111_1111_1110_1110_0010;
reg_A<=32'b0000_0000_0000_0000_0000_0000_0010_1111;

#10 $display("sll");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0000_1001_0001_1000_0000;
reg_A<=32'b1101_1101_1101_1101_1101_1101_1101_1101;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;


#10 $display("sllv");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0001_0000_0000_0000_0100;
reg_A<=32'b0000_0000_0000_0000_0000_0000_0010_0010;
reg_B<=32'b0000_0000_0000_0000_0000_0000_0001_1111;

#10 $display("\nNow,test flags\n");
#10 $display("test flags--overflow");
#10 $display("add-overflow");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0001_0000_0000_0010_0000;
reg_A<=32'b1000_0000_0000_0000_0000_0000_0000_0000;
reg_B<=32'b1111_1111_1111_1111_1111_1111_1111_0001;

#10 $display("addi-overflow");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0010_0000_0000_0000_1111_1111_1110_0001;
reg_A<=32'b1000_0000_0000_0000_0000_0000_0000_0000;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;

#10 $display("sub-overflow");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0000_0001_0000_0000_0010_0010;
reg_A<=32'b1000_0000_0000_0000_0000_0000_0000_0000;
reg_B<=32'b0001_1111_1111_1111_1111_1110_1110_0010;

#10 $display("test flags--zero");

#10 $display("beq-zero");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0001_0000_0010_0001_0000_0000_0000_0000;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;
                                        
#10 $display("bne-zero");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0001_0100_0010_0001_0000_0000_0000_0000;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1101_1101_1101_1101_0010_0010_0010_0010;

#10 $display("test flags--negative");
#10 $display("slt-negative");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0010_0000_0000_0000_0010_1010;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1111_1101_1101_1101_0010_0010_0010_0010;

#10 $display("slti-negative");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0010_1000_0010_0000_0000_0000_0010_1010;
reg_A<=32'b0001_1111_1111_1111_1111_1110_1110_0010;
reg_B<=32'b1111_1101_1101_1101_0010_0010_0010_0010;


#10 $display("sltiu-negative");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0010_1100_0010_0000_1000_0000_0010_1010;
reg_B<=32'b0000_0000_0000_0000_0000_0000_1110_0010;
reg_A<=32'b1111_1101_1101_1101_0010_0010_0010_0010;

#10 $display("sltu-negative");
$display("              instruction       :  op  : func :             rega               :              regb             :flags:      result      ");
instruction<=32'b0000_0000_0010_0000_1000_0000_0010_1011;
reg_A<=32'b1000_0000_1111_1111_1111_1110_1110_0010;
reg_B<=32'b0111_1101_1101_1101_0010_0010_0010_0010;


#20 $finish;
end
endmodule
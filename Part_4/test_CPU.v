`timescale 10ns/1ns

module cpu_test;

reg clk=0;

cpu testcpu(clk);
integer handle;
integer o;
integer cycle=0;
integer i;

initial begin
    $dumpfile("test_CPU.vcd");
    $dumpvars(0,clk,testcpu.Writeregister_e,testcpu.Memread_e,testcpu.rs,testcpu.pc4wb,testcpu.pc4mem,testcpu.pc4e,testcpu.pcsrcd,testcpu.branch2,testcpu.branch1,testcpu.flush1,testcpu.pcbranch1,testcpu.pc4d,testcpu.pcsrc,testcpu.pcf,testcpu.RegWrite_mem,testcpu.RegWrite_e,testcpu.RegWrite,testcpu.RegWrite_wb,testcpu.jalwb,testcpu.MemtoReg,testcpu.readins,testcpu.pcsrc,testcpu.pc,testcpu.stall_f,testcpu.pcf,testcpu.extsel_im,testcpu.immediate,testcpu.jal,testcpu.Memtoreg_wb,testcpu.readmem_wb,testcpu.readmem_mem,testcpu.regb_mem,testcpu.aluout,testcpu.busw,testcpu.Writeregister_wb,testcpu.aluop_e,testcpu.extsel_sa_e,testcpu.immediate_e,testcpu.t_rega,testcpu.t_regb,testcpu.result,testcpu.instrd,testcpu.RegDst_e,testcpu.rd1,testcpu.rd2,testcpu.t_rd1d,testcpu.t_rd2d,testcpu.t_rd1e,testcpu.t_rd2e);
end

initial begin
    $monitor("%b:%b:%b:%b:%b",testcpu.instrd,testcpu.t_rd1e,testcpu.result,testcpu.regb,testcpu.t_regb);
    handle=$fopen("output.txt","w");
    $display("start");
    #10 clk=~clk;
    #10 clk=~clk;
    cycle=cycle+1;
    while(testcpu.instrd!=32'b1111_1111_1111_1111_1111_1111_1111_1111)begin
        //$display("here");
        #10 clk=~clk;
        #10 clk=~clk;
        cycle=cycle+1;

    end
    //$display("here1");
    #10 clk=~clk;
    #10 clk=~clk;
    cycle=cycle+1;
    #10 clk=~clk;
    #10 clk=~clk;
    cycle=cycle+1;
    #10 clk=~clk;
    #10 clk=~clk;
    cycle=cycle+1;
    $display("data memory:");
  for(o=0;o<=511;o=o+1)begin
    $display("%b",testcpu.MM.DATA_RAM[o]);
    $fdisplay(handle,"%b",testcpu.MM.DATA_RAM[o]);
  end
      $display("register file");

              for(i=0;i<=31;i=i+1)begin 
        $display("%b",testcpu.RF.RegMem[i]);
    end
    $display("cycle:%d ",cycle);
    $fclose(handle);
    #20 $finish;
end 


endmodule
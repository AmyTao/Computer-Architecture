`timescale 10ns/1ns
module cpu(clk);
input clk;
// all wires in the CPU
wire[31:0] pc,pcf,pc4d,pcbranch1,pcbranch2,instrd,pc4,readins,busw,rd1,rd2,t_rd1d,t_rd2d,t_rd1e,t_rd2e,immediate_d,immediate_e,t_rega,t_regb,result,rega,regb,regb_mem,aluout,aluout_wb,readmem_mem,readmem_wb,pc4e,pc4mem,pc4wb;
wire[5:0] opcode,func;
wire[4:0]rs,rt,rd,rse,rte,rde,sa_e,sa,Writeregister_e,Writeregister_mem,Writeregister_wb;
wire[15:0] immediate;
wire[25:0] address;
wire[3:0] aluop,aluop_e;
wire[1:0]pcselect,pcsrc,forwardrse,forwardrte,forwardrsd,forwardrtd;
wire stall_f,MemtoReg;


//Instruction fetch
Choosepc cpc(clk,pcsrc,pcbranch1,pcbranch2,t_rd1d,pcf,pc);
PC pcregister(clk,stall_f,pc,pcf);
//Instruction memory
InstructionRAM IM(pcf,readins);
//Instruction decode
IFIDRegsiter IfId(clk,flush1,readins,pcf,instrd,pc4d);
assign opcode=instrd[31:26];
assign func=instrd[5:0];
assign rs=instrd[25:21];
assign rt=instrd[20:16];
assign rd=(opcode==6'b000011)?{(5){instrd[27]}}:instrd[15:11];
assign sa=instrd[10:6];
assign immediate=instrd[15:0];
assign address=instrd[25:0];
control_unit CU(opcode,
                    func,
                    RegWrite,
                    MemtoReg,
                    Memwrite,
                    Memread,
                    RegDst,
                    alu_srcA,
                    alu_srcB,
                    extsel_im,
                    extsel_sa,
                    extsel_si,
                    aluop,
                    branch1,
                    branch2,
                    pcselect,
                    jal
                    );
Registerfile RF(clk,
                rs,
                rt,
                Writeregister_wb,
                busw,
                RegWrite_wb,
                rd1,//registerfile‘s first output
                rd2);//reigisterfile's second output
Chooseinput rd1choose(rd1,result,readmem_mem,forwardrsd,t_rd1d);//choose the updated readdata1 from register file
Chooseinput rd2choose(rd2,result,readmem_mem,forwardrtd,t_rd2d);//choose the updated readdata2 from register file
assign pcsrcd=((branch1&&(t_rd1d==t_rd2d))||(branch2&&(t_rd1d!=t_rd2d)))? 1:0;//branch1 for beq, branch2 for bne
assign immediate_d=(extsel_im)?immediate:{{(16){immediate[15]}},immediate};//1=andi/ori/xori
assign pcbranch1=pc4d+{{(16){immediate[15]}},immediate};//sign-extend immediate
assign pcbranch2={{(6){address[25]}},address};//calculate the address of j type
assign pcsrc=(branch1||branch2)?(pcsrcd)? pcselect:2'b00:pcselect;// determine whether the branch meets the requirement

//Execution
IDEXRegister IdEx(clk,
                    flush2,
                    RegWrite,
                    MemtoReg,
                    Memwrite,
                    Memread,
                    RegDst,
                    alu_srcA,
                    alu_srcB,
                    extsel_sa,
                    extsel_si,
                    aluop,
                    rs,rt,rd,
                    t_rd1d, t_rd2d,
                    immediate_d,
                    sa,
                    pc4d,
                    jal,
                    RegWrite_e,
                    MemtoReg_e,
                    Memwrite_e,
                    Memread_e,
                    RegDst_e,
                    alu_srcA_e,
                    alu_srcB_e,
                    extsel_sa_e,
                    extsel_si_e,
                    aluop_e,
                    rse,rte,rde,
                    t_rd1e, t_rd2e,
                    immediate_e,
                    sa_e,
                    pc4e,
                    jale);
Chooseinput firstinput(t_rd1e,aluout,busw,forwardrse,rega);//choose the updated data of Registerfile[rs]
Chooseinput secondinput(t_rd2e,aluout,busw,forwardrte,regb);//choose the updated data of Registerfile[rt]
assign t_rega=(alu_srcA_e)? sa_e:rega;//decide first input of alu
assign t_regb=(alu_srcB_e)? immediate_e:regb;//decide second input of alu
assign Writeregister_e=(RegDst_e)? rde:rte;
alu ALU(t_rega,t_regb,extsel_sa_e,extsel_si_e,aluop_e,result);

//Memory read/write
EXMEMRegister ExMem(clk,
                    jale,
                    pc4e,
                    RegWrite_e, 
                    MemtoReg_e, 
                    Memwrite_e,
                    Memread_e,
                    result,
                    Writeregister_e,
                    regb,
                    jalmem,
                    pc4mem,
                    RegWrite_mem,
                    MemtoReg_mem,
                    Memwrite_mem,
                    Memread_mem,
                    aluout,
                    regb_mem,
                    Writeregister_mem);
MainMemory MM(aluout,regb_mem,Memwrite_mem,Memread_mem,readmem_mem);

//Write back
MEMWBRegister MemWb(clk,
                    RegWrite_mem,
                    MemtoReg_mem,
                    readmem_mem,
                    aluout,
                    Writeregister_mem,
                    jalmem,
                    pc4mem,
                    readmem_wb,
                    aluout_wb,
                    Writeregister_wb,
                    RegWrite_wb,
                    Memtoreg_wb,
                    jalwb,
                    pc4wb);
assign busw=(Memtoreg_wb)?(jalwb)?pc4wb:readmem_wb:aluout_wb;//decide which pcadress to choose

HazardUnit HU(
              rs,
              rt,
              rse,
              rte,
              Memread_e,
              Writeregister_e,
              Writeregister_mem,
              Writeregister_wb,
              RegWrite_e,
              RegWrite_mem,
              RegWrite_wb,
              pcsrc,
              forwardrsd,
              forwardrtd,
              forwardrse,
              forwardrte,
              stall_f,//stall signal
              flush1,//flush if/id register
              flush2);//flush id/ex register

endmodule



module Choosepc(clk,pcsrc,pcbranch1,pcbranch2,t_rd1d,pcf,pc);
input[1:0] pcsrc;
input clk;
input[31:0] pcbranch1,pcbranch2,t_rd1d,pcf;
output reg[31:0] pc;


always@(*)
begin    
    case(pcsrc)
    2'b00: begin
        pc=pcf+1;//pc+1                
    end
    2'b01: pc=pcbranch1;//branch
    2'b10: pc=t_rd1d/4;//jr
    2'b11: pc=pcbranch2;//jal,j
    endcase
    //$display("pc");
    //$display(pc);
end
endmodule


module PC(clk,stall_f,pc,pcf);
input clk,stall_f;
input [31:0]pc;
output reg[31:0]pcf;
reg[31:0] store_pc;
initial begin
    store_pc=0;   //the first address
end

always@(pc)begin
        store_pc=pc;
end

always@(posedge clk)begin
    if(!stall_f)begin
        pcf=store_pc;
        //$display("pcf");
        //$display(pcf);        
    end
end
endmodule


module InstructionRAM(pcf,readins);
input [31:0] pcf;
output reg[31:0]readins;
reg [31:0] RAM [0:512-1];
integer i;
initial begin
  $readmemb("instructions.bin",RAM);
end
always@(pcf) begin    
    readins = RAM[pcf];//always read instruction
end
endmodule


module IFIDRegsiter(clk,flush1,readins,pcf,instrd,pc4d);
input clk,flush1;
input [31:0]readins,pcf;
output reg[31:0]instrd,pc4d;
reg[31:0] store_ins,store_pc;
initial begin
    store_ins=0;
    store_pc=0;
end
always@(readins)begin
        store_pc=pcf+1;
        store_ins=readins;
end
always@(negedge clk)
begin
    pc4d=store_pc;
    instrd=store_ins;
 
end
endmodule



module control_unit(opcode,
                    func,
                    RegWrite,
                    MemtoReg,
                    Memwrite,
                    Memread,
                    RegDst,
                    alu_srcA,
                    alu_srcB,
                    extsel_im,
                    extsel_sa,
                    extsel_si,
                    aluop,
                    branch1,
                    branch2,
                    pcselect,
                    jal
                    );
input [5:0] opcode,func;
output reg alu_srcA,alu_srcB,extsel_im,extsel_sa,extsel_si,RegWrite,MemtoReg,Memwrite,Memread,RegDst,branch1,branch2,jal;
output reg[1:0]pcselect;
output reg[3:0] aluop;

initial begin
         RegWrite=0;
         MemtoReg=0;
         Memwrite=0;
         Memread=0;
         RegDst=0;
         alu_srcA=0;
         alu_srcB=0;
         extsel_im=0;
         extsel_sa=0;
         extsel_si=0;
         aluop=0;
         branch1=0;
         branch2=0;
         pcselect=0;
         jal=0;
end

always @(opcode,func)
    if(opcode==6'b000000) //R type   
        begin
            alu_srcA<=(func==6'b000000||func==6'b000010||func==6'b000011)? 1:0;//1=sll/srl/sra,0=rs
            alu_srcB<=0;//0=rt
            extsel_im<=0;//0=not zero-extend immediate
            extsel_sa<=(func==6'b000011||func==6'b000111)? 1:0;//1=sra,srav
            extsel_si<=(func==6'b100001||func==6'b100011||func==6'b101011)? 0:1;//0=addu,subu,sltu
            jal<=0;
            case(func)
            6'b100000: aluop=4'b0010;//add    
            6'b100001: aluop=4'b0010;//addu
            6'b100010: aluop=4'b0110;//sub
            6'b100011: aluop=4'b0110;//subu
            6'b100100: aluop=4'b0000;//and
            6'b100101: aluop=4'b0001;//or
            6'b100111: aluop=4'b1100;//nor
            6'b100110: aluop=4'b1001;//xor
            6'b000000: aluop=4'b1010;//sll
            6'b000100: aluop=4'b1010;//sllv
            6'b000011: aluop=4'b1011;//sra
            6'b000111: aluop=4'b1011;//srav
            6'b000010: aluop=4'b1011;//srl
            6'b000110: aluop=4'b1011;//srlv
            6'b101010: aluop=4'b0111;//slt
            6'b101011: aluop=4'b0111;//sltu
            6'b001000: aluop=4'b0000;//jr
            default:$display("unexpected input 1");
            endcase
            RegWrite=1'b1;
            MemtoReg=0;
            Memread=0;
            Memwrite=0;
            RegDst=1'b1;//rd
            branch1=0;
            branch2=0;
            pcselect=(func==6'b001000)? 2'b10:2'b00;//jr           
        end
    else if(opcode==6'b000010)//j
    begin
        alu_srcA=0;
        alu_srcB=0;
        extsel_im=0;
        extsel_sa=0;
        extsel_si=0;
        RegWrite=0;
        MemtoReg=0;
        Memwrite=0;
        Memread=0;
        RegDst=0;
        branch1=0;
        branch2=0;
        pcselect=2'b11;
        aluop=4'b0000;
        jal=0;
    end
    else if(opcode==6'b000011)//jal
    begin
        alu_srcA=0;
        alu_srcB=0;
        extsel_im=0;
        extsel_sa=0;
        extsel_si=0;
        RegWrite=1;
        MemtoReg=1;
        Memwrite=0;
        Memread=0;
        RegDst=1;
        branch1=0;
        branch2=0;
        pcselect=2'b11;
        aluop=4'b0000;
        jal=1;
    end
    else
    begin
        alu_srcA=0;//0=rs
        alu_srcB<=(opcode==6'b000100||opcode==6'b000101)? 0:1;//0=rt(beq/bne), 1=immediate
        extsel_im=(opcode==6'b001100||opcode==6'b001101||opcode==6'b001110)? 1:0;//1=andi/ori/xori
        extsel_sa=0;//not zero-extend sa
        extsel_si=(opcode==6'b001001||opcode==6'b001011)? 0:1;//0=addiu/sltiu
        case(opcode)
        6'b001000: aluop=4'b0010;//addi
        6'b001001: aluop=4'b0010;//addiu
        6'b001100: aluop=4'b0000;//andi
        6'b100011: aluop=4'b0010;//lw
        6'b101011: aluop=4'b0010;//sw
        6'b001101: aluop=4'b0001;//ori
        6'b001110: aluop=4'b1001;//xori
        6'b000100: aluop=4'b0110;//beq
        6'b000101: aluop=4'b0110;//bne
        6'b001010: aluop=4'b0111;//slti
        6'b001011: aluop=4'b0111;//sltiu
        default:$display("unexpected input 2");
        endcase
        RegWrite<=(opcode==6'b101011||opcode==6'b000100||opcode==6'b000101)?0:1;//sw,beq,bne
        MemtoReg<=(opcode==6'b100011)?1:0;//lw
        Memread<=(opcode==6'b100011)?1:0;//lw
        Memwrite<=(opcode==6'b101011)?1:0;//sw
        RegDst<=0;//0=rt
        branch1<=(opcode==6'b000100)?1:0;//1=beq
        branch2<=(opcode==6'b000101)?1:0;//1=bne
        pcselect<=(opcode==6'b000100||opcode==6'b000101)? 2'b01:2'b00;
        jal<=0;
    end

endmodule

module Registerfile(clk,rs,rt,Writeregister_wb,busw,RegWrite_wb,rd1,rd2);
input clk,RegWrite_wb;
input [4:0]rs,rt,Writeregister_wb;
input [31:0] busw;
output reg[31:0] rd1,rd2;
reg[31:0] RegMem[31:0];

integer i;
initial begin   
    for(i=0;i<=31;i=i+1)begin 
    RegMem[i]=0;//initialization
    end

end

always@(rs,rt,Writeregister_wb,busw)begin
    if(RegWrite_wb==1&&Writeregister_wb!=0)RegMem[Writeregister_wb]=busw;
    rd1=RegMem[rs];
    rd2=RegMem[rt];
    //$display(RegMem[rs]);
    //$display(RegMem[rt]);
end

always@(posedge clk)begin   
    if(RegWrite_wb==1&&Writeregister_wb!=0)RegMem[Writeregister_wb]=busw;
end
endmodule


module IDEXRegister (clk,
                    flush2,
                    RegWrite,
                    MemtoReg,
                    Memwrite,
                    Memread,
                    RegDst,
                    alu_srcA,
                    alu_srcB,
                    extsel_sa,
                    extsel_si,
                    aluop,
                    rs,rt,rd,
                    t_rd1d, t_rd2d,
                    immediate_d,
                    sa,
                    pc4_d,
                    jal,
                    RegWrite_e,
                    MemtoReg_e,
                    Memwrite_e,
                    Memread_e,
                    RegDst_e,
                    alu_srcA_e,
                    alu_srcB_e,
                    extsel_sa_e,
                    extsel_si_e,
                    aluop_e,
                    rse,rte,rde,
                    t_rd1e, t_rd2e,
                    immediate_e,
                    sa_e,
                    pc4_e,
                    jal_e);
input clk,flush2,RegWrite,MemtoReg,Memwrite,Memread,RegDst,alu_srcA,alu_srcB,extsel_sa,extsel_si,jal;
input [3:0]aluop;
input[4:0] rs,rt,rd,sa;
input [31:0] t_rd1d, t_rd2d,immediate_d,pc4_d;
output reg RegWrite_e,MemtoReg_e,Memwrite_e,Memread_e,RegDst_e,alu_srcA_e,alu_srcB_e,extsel_sa_e,extsel_si_e,jal_e;
output reg[3:0]aluop_e;
output reg[4:0] rse,rte,rde,sa_e;
output reg[31:0] t_rd1e, t_rd2e,immediate_e,pc4_e;
//store the passed data
reg store_RegWrite,store_MemtoReg,store_Memwrite,store_Memread,store_RegDst,store_alu_srcA,store_alu_srcB,store_extsel_sa,store_extsel_si,store_jal;
reg [3:0]store_aluop;
reg[4:0] store_rsd,store_rtd,store_rdd,store_sad;
reg [31:0] store_t_rd1d, store_t_rd2d,store_immediate_d,store_pc4d;

initial begin
    store_RegWrite=0;
    store_MemtoReg=0;
    store_Memwrite=0;
    store_Memread=0;
    store_RegDst=0;
    store_alu_srcA=0;
    store_alu_srcB=0;
    store_extsel_sa=0;
    store_extsel_si=0;
    store_aluop=0;
    store_rsd=0;
    store_rtd=0;
    store_rdd=0;
    store_sad=0;
    store_t_rd1d=0;
    store_t_rd2d=0;
    store_immediate_d=0;
    store_pc4d=0;
    store_jal=0;
end
always@(posedge clk)begin
    if(flush2)begin
            store_RegWrite=0;
    store_MemtoReg=0;
    store_Memwrite=0;
    store_Memread=0;
    store_RegDst=0;
    store_alu_srcA=0;
    store_alu_srcB=0;
    store_extsel_sa=0;
    store_extsel_si=0;
    store_aluop=0;
    store_rsd=0;
    store_rtd=0;
    store_rdd=0;
    store_sad=0;
    store_t_rd1d=0;
    store_t_rd2d=0;
    store_immediate_d=0;
    store_pc4d=0;
    store_jal=0;
    end
    else
    store_RegWrite<=RegWrite;
    store_MemtoReg<=MemtoReg;
    store_Memwrite<=Memwrite;
    store_Memread<=Memread;
    store_RegDst<=RegDst;
    store_alu_srcA<=alu_srcA;
    store_alu_srcB<=alu_srcB;
    store_extsel_sa<=extsel_sa;
    store_extsel_si<=extsel_si;
    store_aluop<=aluop;
    store_rsd<=rs;
    store_rtd<=rt;
    store_rdd<=rd;
    store_sad<=sa;
    store_t_rd1d<=t_rd1d;
    store_t_rd2d<=t_rd2d;
    store_immediate_d<=immediate_d;
    store_pc4d<=pc4_d;
    store_jal<=jal;
end
always@(negedge clk)begin
    RegWrite_e<=store_RegWrite;
    MemtoReg_e<=store_MemtoReg;
    Memwrite_e<=store_Memwrite;
    Memread_e<=store_Memread;
    RegDst_e<=store_RegDst;
    alu_srcA_e<=store_alu_srcA;
    alu_srcB_e<=store_alu_srcB;
    extsel_sa_e<=store_extsel_sa;
    extsel_si_e<=store_extsel_si;
    aluop_e<=store_aluop;
    rse<=store_rsd;
    rte<=store_rtd;
    rde<=store_rdd;
    sa_e<=store_sad;
    t_rd1e<=store_t_rd1d;
    t_rd2e<=store_t_rd2d;
    immediate_e<=store_immediate_d;
    pc4_e=store_pc4d*4;
    jal_e<=store_jal;
end
endmodule

module alu (rega,regb,extsel_sa,extsel_si,aluop,result);
    
input [31:0] rega,regb;
input extsel_sa,extsel_si;
input[3:0] aluop;
output reg[31:0] result;

always @(*)
begin
    case(aluop)
    4'b0000://and,andi 
    result = rega&regb;
    4'b0001://or,ori
    result=rega|regb;
    4'b0010://add,addi,addu,addiu,lw,sw
    begin
    if(extsel_si)//add,addi,lw,sw
    begin
    result=$signed(rega)+$signed(regb);
    //$display("add");
    //$display(result);
    end
    else
    result=rega+regb;//addu,addiu  
    end
    4'b0110://sub,subu,beq,bne
    begin
    if(extsel_si)//sub
    begin
    result=$signed(rega)-$signed(regb);
    end
    else
    result=rega-regb;//subu
    end
    4'b0111://slt,sltu,slti,sltiu
    begin
    if(extsel_si)//slt,slti
    begin
    result=($signed(rega)<$signed(regb))? 1:0;
    end
    else//sltu,sltiu
    begin
    result=(rega<regb)? 1:0;
    end
    end
    4'b1100://nor
    result=~(rega|regb);
    4'b1010://sll,sllv
    begin
    result=regb<<rega[4:0];
    end
    4'b1001://xor,xori
    result=rega^regb;
    4'b1011://srl,srlv,sra,srav
    begin
    if(extsel_sa) 
    begin
    result=$signed(regb)>>>rega[4:0];
    end
    else
    result=regb>>rega[4:0];
    end
    endcase
end
endmodule

module Chooseinput(t_rd1e,aluout,busw,forwardrse,rega);
input[31:0] t_rd1e,aluout,busw;
input[1:0] forwardrse;
output reg[31:0] rega;
always@(*)
begin
    case(forwardrse)
    2'b00: rega=t_rd1e;
    2'b01: rega=aluout;
    2'b10: rega=busw;
endcase
end
endmodule



module EXMEMRegister(
                    clk,
                    jale,
                    pc4e,
                    RegWrite_e, 
                    MemtoReg_e, 
                    Memwrite_e,
                    Memread_e,
                    result,
                    Writeregister_e,
                    regb,
                    jalmem,
                    pc4mem,
                    RegWrite_mem,
                    MemtoReg_mem,
                    Memwrite_mem,
                    Memread_mem,
                    aluout,
                    regb_mem,
                    Writeregister_mem);       
input clk,RegWrite_e, MemtoReg_e,Memwrite_e,Memread_e,jale;
input [4:0] Writeregister_e;
input [31:0] result,regb,pc4e;
output reg RegWrite_mem,MemtoReg_mem,Memwrite_mem,jalmem,Memread_mem;
output reg[4:0] Writeregister_mem;
output reg[31:0] aluout,regb_mem,pc4mem;
reg store_RegWrite_e,store_MemtoReg_e,store_Memwrite_e,store_Memread_e,store_jale;
reg[4:0]store_Writeregister_e;
reg[31:0]store_result,store_regb,store_pc4e;

initial begin
    store_RegWrite_e=0;
    store_MemtoReg_e=0;
    store_Memwrite_e=0;
    store_jale=0;
    store_Memread_e=0;
    store_Writeregister_e=0;
    store_result=0;
    store_regb=0;
    store_pc4e=0;
end
always@(posedge clk)begin
    store_RegWrite_e<=RegWrite_e;
    store_MemtoReg_e<=MemtoReg_e;
    store_Memwrite_e<=Memwrite_e;
    store_jale<=jale;
    store_Memread_e<=Memread_e;
    store_Writeregister_e<=Writeregister_e;
    store_result<=result;
    store_regb<=regb;
    store_pc4e<=pc4e;
end
always@(negedge clk)begin
    RegWrite_mem=store_RegWrite_e;
    MemtoReg_mem=store_MemtoReg_e;
    Memwrite_mem=store_Memwrite_e;
    jalmem=store_jale;
    Memread_mem=store_Memread_e;
    Writeregister_mem=store_Writeregister_e;
    aluout=store_result;
    regb_mem=store_regb;
    pc4mem=store_pc4e;
end
endmodule


module MainMemory(aluout,regb_mem,Memwrite_mem,Memread_mem,readmem_mem);
input Memwrite_mem,Memread_mem;
input [31:0] aluout,regb_mem;
output reg[31:0] readmem_mem;
reg [31:0] DATA_RAM [0:512-1];

integer i;
initial begin
  
  for(i=0;i<=511;i=i+1)begin
    DATA_RAM[i]=0;
  end
end

always@(*)begin
  DATA_RAM[aluout/4] = (Memwrite_mem)?regb_mem:DATA_RAM[aluout/4];
  readmem_mem = DATA_RAM[aluout/4];
end
endmodule


module MEMWBRegister(clk,
                    RegWrite_mem,
                    MemtoReg_mem,
                    readmem_mem,
                    aluout,
                    Writeregister_mem,
                    jalmem,
                    pc4mem,
                    readmem_wb,
                    aluout_wb,
                    Writeregister_wb,
                    RegWrite_wb,
                    Memtoreg_wb,
                    jalwb,
                    pc4wb);
input clk,RegWrite_mem,MemtoReg_mem,jalmem;
input [4:0] Writeregister_mem;
input [31:0]aluout,readmem_mem,pc4mem;
output reg RegWrite_wb,Memtoreg_wb,jalwb;
output reg[4:0] Writeregister_wb;
output reg[31:0]  readmem_wb,aluout_wb,pc4wb;

reg store_RegWrite_mem,store_MemtoReg_mem,store_jalmem;
reg [4:0] store_Writeregister_mem;
reg [31:0]store_aluout,store_readmem_mem,store_pc4mem;

initial begin
    store_RegWrite_mem<=0;
    store_MemtoReg_mem<=0;
    store_jalmem<=0;
    store_Writeregister_mem<=0;
    store_aluout<=0;
    store_readmem_mem<=0;
    store_pc4mem<=0;
end

always@(posedge clk)begin
    store_RegWrite_mem<=RegWrite_mem;
    store_MemtoReg_mem<=MemtoReg_mem;
    store_jalmem<=jalmem;
    store_Writeregister_mem<=Writeregister_mem;
    store_aluout<=aluout;
    store_readmem_mem<=readmem_mem;
    store_pc4mem<=pc4mem;
end

always@(negedge clk)begin
    RegWrite_wb<=store_RegWrite_mem;
    Memtoreg_wb<=store_MemtoReg_mem;
    jalwb<=store_jalmem;
    Writeregister_wb<=store_Writeregister_mem;
    readmem_wb<=store_readmem_mem;
    aluout_wb<=store_aluout;
    pc4wb<=store_pc4mem;
end

endmodule

module HazardUnit (
              rs,
              rt,
              rse,
              rte,
              Memread_e,
              Writeregister_e,
              Writeregister_mem,
              Writeregister_wb,
              RegWrite_e,
              RegWrite_mem,
              RegWrite_wb,
              pcsrc,
              forwardrsd,
              forwardrtd,
              forwardrse,
              forwardrte,
              stall_f,
              flush1,
              flush2);
input [4:0]rs,rt,rse,rte,Writeregister_e,Writeregister_mem,Writeregister_wb;
input RegWrite_e,RegWrite_mem,RegWrite_wb,Memread_e;
input [1:0]pcsrc;
output reg stall_f,flush1,flush2;
output reg[1:0]forwardrse,forwardrte,forwardrsd,forwardrtd;
initial begin
    forwardrsd<=0;
    forwardrtd<=0;
    forwardrse<=0;
    forwardrte<=0;
    stall_f<=0;
    flush1<=0;
    flush2<=0;
end
always@(*)begin
        if(rs==Writeregister_e&&Writeregister_e!=0&&RegWrite_e)begin
        forwardrsd=2'b01;
    end
    else if(rs==Writeregister_mem&&Writeregister_mem!=0&&RegWrite_mem) forwardrsd=2'b10;
    else forwardrsd=2'b00;

        if(rt==Writeregister_e&&Writeregister_e!=0&&RegWrite_e)begin
        forwardrtd=2'b01;
    end
    else if(rt==Writeregister_mem&&Writeregister_mem!=0&&RegWrite_mem) forwardrtd=2'b10;
    else forwardrtd=2'b00;


    if(rse==Writeregister_mem&&Writeregister_mem!=0&&RegWrite_mem)begin
        forwardrse=2'b01;
    end
    else if(rse==Writeregister_wb&&Writeregister_wb!=0&&RegWrite_wb) forwardrse=2'b10;
    else forwardrse=2'b00;
    if(rte==Writeregister_mem&&Writeregister_mem!=0&&RegWrite_mem)begin
    forwardrte=2'b01;
end
else if(rte==Writeregister_wb&&Writeregister_wb!=0&&RegWrite_wb) forwardrte<=2'b10;
else forwardrte<=2'b00;
    stall_f=(Memread_e&&(Writeregister_e==rs||Writeregister_e==rt))?1:0;
    flush2=(Memread_e&&(Writeregister_e==rs||Writeregister_e==rt))?1:0;
    flush1=(pcsrc==2'b01||pcsrc==2'b10||pcsrc==2'b11)?1:0;
    
end
endmodule





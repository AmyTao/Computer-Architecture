module alu(instruction,regA,regB,result,flags);

input [31:0] instruction,regA,regB;
output [31:0] result;
output [2:0] flags;

wire ALU_SRCA, ALU_SRCB,EXTSEL_IM, EXTSEL_SA, EXTSEL_SI;
wire [3:0] ALUOP;
wire [31:0] rega,regb,regc,regd;
wire [15:0] immediate;
assign immediate=instruction[15:0];

control_unit cu(instruction[31:26],instruction[5:0],ALU_SRCA, ALU_SRCB,EXTSEL_IM, EXTSEL_SA, EXTSEL_SI,ALUOP);

assign regc=(instruction[25:21]==5'b00000)? regA:regB;
//always @(regc) $display("ins: %b rega: %b regc: %b",instruction[25:21]==5'b00000,regA,regc);
assign rega=(ALU_SRCA)? instruction[10:6]:regc;//1=sa,0=rs
assign regd=(instruction[20:16]==5'b00000)? regA:regB;
//always @(regd) $display("regb: %b",regB);
assign regb=(ALU_SRCB)?(EXTSEL_IM)? instruction[15:0]:{{(16){immediate[15]}},immediate}:regd;//1=immediate, 0=rt
//always @(regb) $display("alu: %b ext %b regb: %b",ALU_SRCB,EXTSEL_IM,regb);


alu_calculate ac(instruction[31:26],instruction[5:0],rega,regb,ALU_SRCA,ALU_SRCB,EXTSEL_IM, EXTSEL_SA, EXTSEL_SI,ALUOP,result,flags);


endmodule

module control_unit(opcode,func,alu_srcA,alu_srcB,extsel_im,extsel_sa,extsel_si,aluop);
input [5:0] opcode,func;
output reg alu_srcA,alu_srcB,extsel_im,extsel_sa,extsel_si;
output reg[3:0] aluop;

always @(opcode,func)
    if(opcode==6'b000000) //R type   
        begin
            alu_srcA=(func==6'b000000||func==6'b000010||func==6'b000011)? 1:0;//1=sll/srl/sra,0=rs
            alu_srcB=0;//0=rt
            extsel_im=0;//0=not zero-extend immediate
            extsel_sa=(func==6'b000011||func==6'b000111)? 1:0;//1=sra,srav
            extsel_si=(func==6'b100001||func==6'b100011||func==6'b101011)? 0:1;//0=addu,subu,sltu
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
            default:$display("unexpected input 1");
            endcase
        end
    else
    begin
        alu_srcA=0;//0=rs
        alu_srcB<=(opcode==6'b000100||opcode==6'b000101)? 0:1;//0=rt, 1=immediate
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


    end

endmodule

module alu_calculate (opcode,func,rega,regb,alu_srcA,alu_srcB,extsel_im,extsel_sa,extsel_si,aluop,result,flags);
    
input [31:0] rega,regb;
input clk,alu_srcA,alu_srcB,extsel_im,extsel_sa,extsel_si;
input[3:0] aluop;
input [5:0] opcode,func;
output reg[2:0] flags;
output reg[31:0] result;

always @(*)
begin
    //$display("aluop: %b",aluop);
    flags<=3'b000;
    case(aluop)
    4'b0000://and,andi 
    result <= rega&regb;
    4'b0001://or,ori
    result<=rega|regb;
    4'b0010://add,addi,addu,addiu,lw,sw
    begin
    if(extsel_si)//add,addi,lw,sw
    begin
    result<=$signed(rega)+$signed(regb);
    //add
    if(opcode==6'b000000&&func==6'b100000)flags<=(($signed(rega)>=0&&$signed(regb)>=0&&$signed(result)<0)||($signed(rega)<=0&&$signed(regb)<=0&&$signed(result)>0))? 3'b001:3'b000;
    //addi
    if(opcode==6'b001000)flags<=(($signed(rega)>=0&&$signed(regb)>=0&&$signed(result)<0)||($signed(rega)<=0&&$signed(regb)<=0&&$signed(result)>0))? 3'b001:3'b000;
    end
    else
    //$display("rega: %b, regb: %b",rega,regb);
    result<=rega+regb;//addu,addiu  
    end
    4'b0110://sub,subu,beq,bne
    begin
    if(extsel_si)//sub
    begin
    result<=$signed(rega)-$signed(regb);
    if(opcode!=6'b000100&&opcode!=6'b000101)   
    flags<=(($signed(rega)>=0&&$signed(regb)<=0&&$signed(result)<0)||($signed(rega)<=0&&$signed(regb)>=0&&$signed(result)>0))? 3'b001:3'b000;    
    else //beq,bne   
    flags<=(($signed(rega)-$signed(regb))==0)? 3'b100:3'b000;
    end
    else
    result<=rega-regb;//subu
    end
    4'b0111://slt,sltu,slti,sltiu
    begin
    if(extsel_si)//slt,slti
    begin
    result<=$signed(rega)-$signed(regb);
    flags<=($signed(rega)<$signed(regb))? 3'b010:3'b000;
    end
    else//sltu,sltiu
    begin
    result<=rega-regb;
    flags<=(rega<regb)? 3'b010:3'b000;
    end
    end
    4'b1100://nor
    result<=~(rega|regb);
    4'b1010://sll,sllv
    begin
    //$display("rega: %b, regb: %b",rega,regb);
    result<=regb<<rega[4:0];
    //$display("%b",result);
    end
    4'b1001://xor,xori
    result<=rega^regb;
    4'b1011://srl,srlv,sra,srav
    begin
    if(extsel_sa) 
    begin
    result<=$signed(regb)>>>rega[4:0];
    //$display("%b",result);
    end
    else
    result<=regb>>rega[4:0];
    end
    endcase


end


endmodule


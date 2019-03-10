/*
 *  beta module of table's version
 *  Author: Guojun Tang 
 */
module  serial_beta( x, y);

parameter       BLOCK_SIZE = 256;
parameter       SIDE_SIZE = BLOCK_SIZE/2;
parameter       COLUMN_SIZE  = SIDE_SIZE/4;
parameter       SBOX_SIZE  = 4;

input   [0:(SIDE_SIZE-1)]   x;
output  [0:(SIDE_SIZE-1)]   y;


wire  [0:(SIDE_SIZE-1)]   y;

//input 
reg [0:SBOX_SIZE-1] s0;
reg [0:SBOX_SIZE-1] s1;
reg [0:SBOX_SIZE-1] s2;
reg [0:SBOX_SIZE-1] s3;
reg [0:SBOX_SIZE-1] s4;
reg [0:SBOX_SIZE-1] s5;
reg [0:SBOX_SIZE-1] s6;
reg [0:SBOX_SIZE-1] s7;
reg [0:SBOX_SIZE-1] s8;
reg [0:SBOX_SIZE-1] s9;
reg [0:SBOX_SIZE-1] s10;
reg [0:SBOX_SIZE-1] s11;
reg [0:SBOX_SIZE-1] s12;
reg [0:SBOX_SIZE-1] s13;
reg [0:SBOX_SIZE-1] s14;
reg [0:SBOX_SIZE-1] s15;
reg [0:SBOX_SIZE-1] s16;
reg [0:SBOX_SIZE-1] s17;
reg [0:SBOX_SIZE-1] s18;
reg [0:SBOX_SIZE-1] s19;
reg [0:SBOX_SIZE-1] s20;
reg [0:SBOX_SIZE-1] s21;
reg [0:SBOX_SIZE-1] s22;
reg [0:SBOX_SIZE-1] s23;
reg [0:SBOX_SIZE-1] s24;
reg [0:SBOX_SIZE-1] s25;
reg [0:SBOX_SIZE-1] s26;
reg [0:SBOX_SIZE-1] s27;
reg [0:SBOX_SIZE-1] s28;
reg [0:SBOX_SIZE-1] s29;
reg [0:SBOX_SIZE-1] s30;
reg [0:SBOX_SIZE-1] s31;


//connect input wires
//assign s0 = {x[31], x[23], x[15], x[7]} ;
//assign s1 = {x[30], x[22], x[14], x[6]} ;
//assign s2 = {x[29], x[21], x[13], x[5]} ;
//assign s3 = {x[28], x[20], x[12], x[4]} ;
//assign s4 = {x[27], x[19], x[11], x[3]} ;
//assign s5 = {x[26], x[18], x[10], x[2]} ;
//assign s6 = {x[25], x[17], x[9], x[1]}  ;
//assign s7 = {x[24], x[16], x[8], x[0]}  ;
//
//sbox a0(s0,{y[31], y[23], y[15], y[7]}) ;
//sbox a1(s1,{y[30], y[22], y[14], y[6]}) ;
//sbox a2(s2,{y[29], y[21], y[13], y[5]}) ;
//sbox a3(s3,{y[28], y[20], y[12], y[4]}) ;
//sbox a4(s4,{y[27], y[19], y[11], y[3]}) ;
//sbox a5(s5,{y[26], y[18], y[10], y[2]}) ;
//sbox a6(s6,{y[25], y[17], y[9],  y[1]}) ;
//sbox a7(s7,{y[24], y[16], y[8],  y[0]}) ;

always @(x)
fork
    s0 <=  {x[0],  x[32], x[64], x[96]};
    s1 <=  {x[1],  x[33], x[65], x[97]};
    s2 <=  {x[2],  x[34], x[66], x[98]};
    s3 <=  {x[3],  x[35], x[67], x[99]}; 
    s4 <=  {x[4],  x[36], x[68], x[100]};
    s5 <=  {x[5],  x[37], x[69], x[101]};
    s6 <=  {x[6],  x[38], x[70], x[102]};  
    s7 <=  {x[7],  x[39], x[71], x[103]};  
    s8 <=  {x[8],  x[40], x[72], x[104]};
    s9 <=  {x[9],  x[41], x[73], x[105]};
    s10 <= {x[10], x[42], x[74], x[106]};
    s11 <= {x[11], x[43], x[75], x[107]}; 
    s12 <= {x[12], x[44], x[76], x[108]};
    s13 <= {x[13], x[45], x[77], x[109]};
    s14 <= {x[14], x[46], x[78], x[110]};  
    s15 <= {x[15], x[47], x[79], x[111]};  
    s16 <= {x[16], x[48], x[80], x[112]};
    s17 <= {x[17], x[49], x[81], x[113]};
    s18 <= {x[18], x[50], x[82], x[114]};
    s19 <= {x[19], x[51], x[83], x[115]}; 
    s20 <= {x[20], x[52], x[84], x[116]};
    s21 <= {x[21], x[53], x[85], x[117]};
    s22 <= {x[22], x[54], x[86], x[118]};  
    s23 <= {x[23], x[55], x[87], x[119]};  
    s24 <= {x[24], x[56], x[88], x[120]};
    s25 <= {x[25], x[57], x[89], x[121]};
    s26 <= {x[26], x[58], x[90], x[122]};
    s27 <= {x[27], x[59], x[91], x[123]}; 
    s28 <= {x[28], x[60], x[92], x[124]};
    s29 <= {x[29], x[61], x[93], x[125]};
    s30 <= {x[30], x[62], x[94], x[126]};  
    s31 <= {x[31], x[63], x[95], x[127]};  
join


sbox a0(s0, {y[0],  y[32], y[64], y[96]});
sbox a1(s1, {y[1],  y[33], y[65], y[97]});
sbox a2(s2, {y[2],  y[34], y[66], y[98]});
sbox a3(s3, {y[3],  y[35], y[67], y[99]}); 
sbox a4(s4, {y[4],  y[36], y[68], y[100]});
sbox a5(s5, {y[5],  y[37], y[69], y[101]});
sbox a6(s6, {y[6],  y[38], y[70], y[102]});
sbox a7(s7, {y[7],  y[39], y[71], y[103]});
sbox a8(s8, {y[8],  y[40], y[72], y[104]});
sbox a9(s9, {y[9],  y[41], y[73], y[105]});
sbox a10(s10, {y[10], y[42], y[74], y[106]});
sbox a11(s11, {y[11], y[43], y[75], y[107]});
sbox a12(s12, {y[12], y[44], y[76], y[108]});
sbox a13(s13, {y[13], y[45], y[77], y[109]});
sbox a14(s14, {y[14], y[46], y[78], y[110]});
sbox a15(s15, {y[15], y[47], y[79], y[111]});
sbox a16(s16, {y[16], y[48], y[80], y[112]});
sbox a17(s17, {y[17], y[49], y[81], y[113]});
sbox a18(s18, {y[18], y[50], y[82], y[114]});
sbox a19(s19, {y[19], y[51], y[83], y[115]});
sbox a20(s20, {y[20], y[52], y[84], y[116]});
sbox a21(s21, {y[21], y[53], y[85], y[117]});
sbox a22(s22, {y[22], y[54], y[86], y[118]});
sbox a23(s23, {y[23], y[55], y[87], y[119]});
sbox a24(s24, {y[24], y[56], y[88], y[120]});
sbox a25(s25, {y[25], y[57], y[89], y[121]});
sbox a26(s26, {y[26], y[58], y[90], y[122]});
sbox a27(s27, {y[27], y[59], y[91], y[123]});
sbox a28(s28, {y[28], y[60], y[92], y[124]});
sbox a29(s29, {y[29], y[61], y[93], y[125]});
sbox a30(s30, {y[30], y[62], y[94], y[126]});
sbox a31(s31, {y[31], y[63], y[95], y[127]});




endmodule


/*
 *  beta module of table's version
 *  Author: Guojun Tang 
 */
module  beta_table( x, y);

parameter       BLOCK_SIZE = 64;
parameter       SIDE_SIZE = BLOCK_SIZE/2;
parameter       COLUMN_SIZE  = SIDE_SIZE/4;
parameter       SBOX_SIZE  = 4;

input   [0:(SIDE_SIZE-1)]   x;
output  [0:(SIDE_SIZE-1)]   y;


wire  [0:(SIDE_SIZE-1)]   y;

//input 
wire [0:SBOX_SIZE-1] s0;
wire [0:SBOX_SIZE-1] s1;
wire [0:SBOX_SIZE-1] s2;
wire [0:SBOX_SIZE-1] s3;
wire [0:SBOX_SIZE-1] s4;
wire [0:SBOX_SIZE-1] s5;
wire [0:SBOX_SIZE-1] s6;
wire [0:SBOX_SIZE-1] s7;


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

assign s0 ={x[0], x[8], x[16],  x[24]};
assign s1 ={x[1], x[9], x[17],  x[25]};
assign s2 ={x[2], x[10], x[18], x[26]};
assign s3 ={x[3], x[11], x[19], x[27]}; 
assign s4 ={x[4], x[12], x[20], x[28]};
assign s5 ={x[5], x[13], x[21], x[29]};
assign s6 ={x[6], x[14], x[22], x[30]};  
assign s7 ={x[7], x[15], x[23], x[31]};  

sbox a0(s0, {y[0], y[8],  y[16], y[24]});
sbox a1(s1, {y[1], y[9],  y[17], y[25]});
sbox a2(s2, {y[2], y[10], y[18], y[26]});
sbox a3(s3, {y[3], y[11], y[19], y[27]}); 
sbox a4(s4, {y[4], y[12], y[20], y[28]});
sbox a5(s5, {y[5], y[13], y[21], y[29]});
sbox a6(s6, {y[6], y[14], y[22], y[30]}); 
sbox a7(s7, {y[7], y[15], y[23], y[31]}); 





endmodule


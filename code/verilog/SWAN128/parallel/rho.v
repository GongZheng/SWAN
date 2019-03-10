//Top level module rho
//Author: Weijie Li(weijie.ia.li@gmail.com), Guojun Tang(tangguojun@m.scnu.edu.cn)

module  rho( x, y);

parameter       BLOCK_SIZE = 128;
parameter       SIDE_SIZE = BLOCK_SIZE/2;
parameter       COLUMN_SIZE  = SIDE_SIZE/4;

input   [0:(SIDE_SIZE-1)]   x;
output  [0:(SIDE_SIZE-1)]   y;

wire    [0:(COLUMN_SIZE-1)]   a[0:3];
wire    [0:(COLUMN_SIZE-1)]   b[0:3], t;

assign  a[0] = x[(0*COLUMN_SIZE):(1*COLUMN_SIZE-1)];
assign  a[1] = x[(1*COLUMN_SIZE):(2*COLUMN_SIZE-1)];
assign  a[2] = x[(2*COLUMN_SIZE):(3*COLUMN_SIZE-1)];
assign  a[3] = x[(3*COLUMN_SIZE):(4*COLUMN_SIZE-1)];

assign t = a[0] ^ a[1] ^ a[2] ^ a[3];

assign b[0] = a[0] ^ t;
assign b[1] = a[1] ^ t;
assign b[2] = a[2] ^ t;
assign b[3] = a[3] ^ t;

assign y = {b[0], b[1], b[2], b[3]};

endmodule

//Top level module vartheta
//Author: Weijie Li(weijie.ia.li@gmail.com), Guojun Tang(tangguojun@m.scnu.edu.cn)

module  serial_vartheta( x, y);

parameter       BLOCK_SIZE = 256;
parameter       SIDE_SIZE = BLOCK_SIZE/2;       
parameter       COLUMN_SIZE  = SIDE_SIZE/4;     

parameter       PA  = 1;
parameter       PB  = 9;
parameter       PC  = 19;

input   [0:(SIDE_SIZE-1)]   x;
output  [0:(SIDE_SIZE-1)]   y;

wire    [0:(COLUMN_SIZE-1)]   a[0:3];
reg    [0:(COLUMN_SIZE-1)]   b[0:3];

assign  a[0] = x[(0*COLUMN_SIZE):(1*COLUMN_SIZE-1)];
assign  a[1] = x[(1*COLUMN_SIZE):(2*COLUMN_SIZE-1)];
assign  a[2] = x[(2*COLUMN_SIZE):(3*COLUMN_SIZE-1)];
assign  a[3] = x[(3*COLUMN_SIZE):(4*COLUMN_SIZE-1)];

always @(x)
fork
    b[3] <= a[3];
    b[2] <= {a[2][(COLUMN_SIZE-PA):(COLUMN_SIZE-1)], a[2][0:(COLUMN_SIZE-1-PA)] };
    b[1] <= {a[1][(COLUMN_SIZE-PB):(COLUMN_SIZE-1)], a[1][0:(COLUMN_SIZE-1-PB)] };
    b[0] <= {a[0][(COLUMN_SIZE-PC):(COLUMN_SIZE-1)], a[0][0:(COLUMN_SIZE-1-PC)] };
join


assign y = {b[0], b[1], b[2], b[3]};

endmodule


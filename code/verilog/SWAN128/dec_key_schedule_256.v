//Top level module key_schedule
//Author: Weijie Li(weijie.ia.li@gmail.com) , Guojun Tang(tangguojun@m.scnu.edu.cn)

module  dec_key_schedule_256(key, delta, next_key, next_delta, sk);

parameter       BLOCK_SIZE = 128;
parameter       SIDE_SIZE = BLOCK_SIZE/2;
parameter       COLUMN_SIZE  = SIDE_SIZE/4;
parameter       KEY_SIZE = 256;

parameter       PD  = 56; 
parameter       INV_PD  = KEY_SIZE - PD; 
parameter       DELTA0 = 64'h9e3779b97f4a7c15; 

input   [0:(KEY_SIZE-1)]    key;
input   [0:(SIDE_SIZE-1)]    delta;

output  [0:(KEY_SIZE-1)]    next_key;
output [0:(SIDE_SIZE-1)]    next_delta;
output [0:(SIDE_SIZE-1)]    sk;


wire  [0:(KEY_SIZE-1)]    next_key;
wire  [0:(SIDE_SIZE-1)]    next_delta;
wire    [0:(KEY_SIZE-1)]    next_key0;
wire    [0:(SIDE_SIZE-1)]   sk;

// assign sk
assign sk = key[(KEY_SIZE - SIDE_SIZE):(KEY_SIZE-1)];
assign next_key0 = {key[0:KEY_SIZE -SIDE_SIZE -1], sk - delta};

//update k, delta
assign next_key = {next_key0[(KEY_SIZE-INV_PD):(KEY_SIZE-1)], next_key0[0:(KEY_SIZE-1-INV_PD)]};
assign next_delta = delta - DELTA0;

endmodule
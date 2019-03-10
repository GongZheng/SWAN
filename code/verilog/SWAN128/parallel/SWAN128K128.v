// Top level module for SWAN64K128
//Author: Weijie Li(weijie.ia.li@gmail.com), Guojun Tang(tangguojun@m.scnu.edu.cn)

module SWAN128K128_ENC(start, rst, inp, key, clk, ready, out);

//describe parameter

parameter       BLOCK_SIZE = 128;
parameter       SIDE_SIZE = BLOCK_SIZE/2;
parameter       COLUMN_SIZE  = SIDE_SIZE/4;

parameter       KEY_SIZE = 128;
parameter       DELTA0 = 64'h9e3779b97f4a7c15; 

parameter       ROUNDS = 48;     //  32  ROUND
parameter       HALF_ROUNDS = ROUNDS * 2 ;     //  64  ROUND

input   start;          //1 for input, 0 for execute
input   [0:BLOCK_SIZE-1]  inp;    //input data
input   rst;            //reset signal
input   [0:KEY_SIZE-1] key;
input   clk;            //clock

output  ready;
output  [0:BLOCK_SIZE-1]  out;    //output data

reg ready; 
reg [0:KEY_SIZE - 1] master_key;       
reg [0:SIDE_SIZE - 1] rd;       //round delta
reg [7:0] half_round;              //counter of half round
reg [0:SIDE_SIZE - 1] L;
reg [0:SIDE_SIZE - 1] R;
reg [0:SIDE_SIZE - 1] D;

wire [0:SIDE_SIZE - 1] key_operation_input;
wire [0:SIDE_SIZE - 1] beta_input;
wire [0:SIDE_SIZE - 1] theta_input;
wire [0:SIDE_SIZE - 1] rho_input;
wire [0:SIDE_SIZE - 1] round_output;
wire [0:SIDE_SIZE - 1] round_input;
wire [0:BLOCK_SIZE - 1] out;

wire [0:SIDE_SIZE - 1] sk;   //round key 
wire [0:SIDE_SIZE - 1] next_rd;       //round delta
wire [0:KEY_SIZE - 1] next_key;       



/*
 * key schedule circuit 
 */
enc_key_schedule_128 u0(master_key, rd, next_key, next_rd, sk);

/*
 * encryption circuit
 */
assign out = {R,L};
assign round_input = (half_round[0])?L : R;
vartheta u1(round_input, key_operation_input);
assign beta_input = key_operation_input ^ sk;
beta_table u2(beta_input, theta_input);
vartheta u3(theta_input, rho_input);
rho u4(rho_input, round_output);


/*
 * logic
 */
always @(posedge clk) 
    if (!rst)  
        begin
            half_round =  0;
        end
    else if(start)
        begin
            ready = 0;
            half_round =  HALF_ROUNDS - 1;
        end
    else if(!ready)
        begin
            if(half_round[0])
                begin
                    D = round_output;
                    R = R ^ D;
                end
            else
                begin
                    D = round_output;
                    L = L ^ D;
                end
            

            //$display("round:%d",half_round);
            //$display("round input:%x",round_input);
            //$display("sk:%x",sk);
            //$display("key input:%x",key_operation_input);
            //$display("beta input:%x",beta_input);
            //$display("theta input:%x",theta_input);
            //$display("rho input:%x",rho_input);
            //$display("L:%x",L);
            //$display("R:%x",R);
            //$display("D:%x",D);
            //update key and round constant 
            master_key =  next_key;
            rd = next_rd;
            ready = ~|half_round & !start;
            half_round =  half_round - 1;
        end



always @(posedge clk) 
    if(start)
        begin
            rd = 0;
            master_key = key;
            L = inp[SIDE_SIZE:BLOCK_SIZE-1]; 
            R = inp[0:SIDE_SIZE - 1];
        end
endmodule


module SWAN128K128_DEC(start, rst, inp, key, clk, ready, out);
parameter       BLOCK_SIZE = 128;
parameter       SIDE_SIZE = BLOCK_SIZE/2;
parameter       COLUMN_SIZE  = SIDE_SIZE/4;

parameter       KEY_SIZE = 128;
parameter       PD  = 56; 
parameter       DELTA0 = 64'h9e3779b97f4a7c15; 

parameter       ROUNDS = 48;     //  32  ROUND
parameter       HALF_ROUNDS = ROUNDS * 2 ;     //  64  ROUND

input   start;          //1 for input, 0 for execute
input   [0:BLOCK_SIZE-1]  inp;    //input data
input   rst;            //reset signal
input   [0:KEY_SIZE-1] key;
input   clk;            //clock

output  ready;
output  [0:BLOCK_SIZE-1]  out;    //output data

reg ready; 
reg precompute;                  //1 for precomutation done
reg [0:KEY_SIZE - 1] master_key;       
reg [0:SIDE_SIZE - 1] rd;       //round delta
reg [7:0] half_round;              //counter of half round
reg [0:SIDE_SIZE - 1] L;
reg [0:SIDE_SIZE - 1] R;
reg [0:SIDE_SIZE - 1] D;

wire [0:SIDE_SIZE - 1] key_operation_input;
wire [0:SIDE_SIZE - 1] beta_input;
wire [0:SIDE_SIZE - 1] theta_input;
wire [0:SIDE_SIZE - 1] rho_input;
wire [0:SIDE_SIZE - 1] round_output;
wire [0:SIDE_SIZE - 1] round_input;
wire [0:BLOCK_SIZE - 1] out;

wire [0:SIDE_SIZE - 1] sk;   //round key 
wire [0:SIDE_SIZE - 1] next_rd;       //round delta
wire [0:KEY_SIZE - 1] next_key;       



/*
 * key schedule circuit 
 */
dec_key_schedule_128 u0(master_key, rd, next_key, next_rd, sk);

/*
 * encryption circuit
 */
assign out = {R,L};
assign round_input = (half_round[0])?R : L;
vartheta u1(round_input, key_operation_input);
assign beta_input = key_operation_input ^ sk;
beta_table u2(beta_input, theta_input);
vartheta u3(theta_input, rho_input);
rho u4(rho_input, round_output);
integer i;


/*
 * logic
 */
always @(posedge clk) 
    if (!rst)  
        begin
            half_round =  0;
        end
    else if(start)
        begin
            ready = 0;
            precompute = 0;
            half_round =  HALF_ROUNDS - 1;
        end
    else if(!ready)
        begin
            if(!precompute)
                begin
                for(i = 0; i< HALF_ROUNDS;i = i + 1)
                    begin
                        rd = rd + DELTA0;
                        master_key = {master_key[(KEY_SIZE-PD):(KEY_SIZE-1)], master_key[0:(KEY_SIZE-1-PD)]};
                        master_key[(KEY_SIZE-SIDE_SIZE):(KEY_SIZE-1)] = master_key[(KEY_SIZE-SIDE_SIZE):(KEY_SIZE-1)]+ rd;
                    end
                precompute = 1;
                end
            else    
                begin
                    if(!half_round[0])
                        begin
                            D = round_output;
                            R = R ^ D;
                        end
                    else
                        begin
                            D = round_output;
                            L = L ^ D;
                        end
                    
                    //$display("round:%d",half_round);
                    //$display("key:%x",master_key);
                    //$display("rd:%x",rd);
                    //$display("sk:%x",sk);

                    //$display("round input:%x",round_input);
                    //$display("key input:%x",key_operation_input);
                    //$display("beta input:%x",beta_input);
                    //$display("theta input:%x",theta_input);
                    //$display("rho input:%x",rho_input);
                    //$display("L:%x",L);
                    //$display("R:%x",R);
                    //$display("D:%x",D);
                    //update key and round constant 
                    master_key =  next_key;
                    rd = next_rd;
                    ready = ~|half_round & !start;
                    half_round =  half_round - 1;
                end
        end

always @(posedge clk) 
    if(start)
        begin
            rd = 0;
            master_key = key;
            L = inp[SIDE_SIZE:BLOCK_SIZE-1]; 
            R = inp[0:SIDE_SIZE - 1];
        end

endmodule
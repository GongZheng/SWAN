`timescale 1ns / 10ps

////////////////////////////////////////////////////////////////////////////////
// 
// encrypt test
// 
// test data 1: 
// key:    0x0
// in: 0x8877665544332211
// out:0x3249d350bc89337c
//
// test data 2: 
// KEY:0x1111111111111111111111111111111111111111111111111111111111111111
// in:0xefcdab9078563412
// out:0x15cebc3b6e457d2b
//
// test data 3: 
// KEY:0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
// in:0xf0debc9a78563412
// out:0xb57ffcf7a5449b9a
// 
////////////////////////////////////////////////////////////////////////////////

module serial_swan64k256_enc_test;

	parameter       BLOCK_SIZE = 64;
	parameter       KEY_SIZE = 256;
	// Inputs
	reg clk;
	reg start;
	reg rst;
	reg [0:KEY_SIZE-1] key;
	reg [0:BLOCK_SIZE-1] inp;

	// Outputs
	wire [0:BLOCK_SIZE-1] out;
	wire ready;
	

	serial_SWAN64K256_ENC  test(
		.start(start),
		.rst(rst),
		.inp(inp), 
		.key(key), 
		.clk(clk), 
		.ready(ready),
		.out(out)
	);

	initial begin
		// Initialize Inputs
		clk = 0;
		rst = 1;
		start = 1;
		//key = 256'h1111111111111111111111111111111111111111111111111111111111111111;
 		//inp = 64'hefcdab9078563412;
		
		//key = 0 ;
		//inp = 64'h8877665544332211;

		key = 256'hffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
		inp = 64'hf0debc9a78563412;
		#10;
		start = 0;
		
        
	end
	
    always 
    begin
        #5;
        clk = !clk;
    end 
	// stop simulating
	always @(posedge clk)
		if (ready) 
			begin 
				$display("out:%x", out);
				$stop;
			end
      
endmodule


module serial_swan64k256_dec_test;

	parameter       BLOCK_SIZE = 64;
	parameter       KEY_SIZE = 256;
	// Inputs
	reg clk;
	reg start;
	reg rst;
	reg [0:KEY_SIZE-1] key;
	reg [0:BLOCK_SIZE-1] inp;

	// Outputs
	wire [0:BLOCK_SIZE-1] out;
	wire ready;
	

	serial_SWAN64K256_DEC  test(
		.start(start),
		.rst(rst),
		.inp(inp), 
		.key(key), 
		.clk(clk), 
		.ready(ready),
		.out(out)
	);

	initial begin
		// Initialize Inputs
		clk = 0;
		rst = 1;
		start = 1;
		//key = 256'h1111111111111111111111111111111111111111111111111111111111111111;
		//inp = 64'h15cebc3b6e457d2b;
		
		//key = 0 ;
		//inp = 64'h3249d350bc89337c;

		key = 256'hffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
		inp = 64'hb57ffcf7a5449b9a;
		#10;
		start = 0;
		
        
	end
	
    always 
    begin
        #5;
        clk = !clk;
    end 
	// stop simulating
	always @(posedge clk)
		if (ready) 
			begin 
				$display("out:%x", out);
				$stop;
			end
      
endmodule


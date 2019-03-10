`timescale 1ns / 10ps

////////////////////////////////////////////////////////////////////////////////
// 
// encrypt test
// 
// test data 1: 
// key:    0x0
// in: 0x78563412785634127856341278563412
// out:0x7471e75b14c448bbafc68d66170d1ac9
//
// test data 2: 
// key: 0x78563412785634127856341278563412
// in:  0x11111111111111111111111111111111
// out: 0xfaf0220b1ac0c6ce55a98207d26e67ec
// 
// test data 3: 
// key: 0xffffffffffffffffffffffffffffffff
// in:  0xf0debc9a78563412f0debc9a78563412
// out: 0x8b9d520a184f4dc8189263a70c0eebba
////////////////////////////////////////////////////////////////////////////////

module serial_swan128k128_enc_test;

	parameter       BLOCK_SIZE = 128;
	parameter       KEY_SIZE = 128;
	// Inputs
	reg clk;
	reg start;
	reg rst;
	reg [0:KEY_SIZE-1] key;
	reg [0:BLOCK_SIZE-1] inp;

	// Outputs
	wire [0:BLOCK_SIZE-1] out;
	wire ready;
	
	serial_SWAN128K128_ENC  test(
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

		//key = 0;
 		//inp = 128'h78563412785634127856341278563412;

 		//key = 128'h78563412785634127856341278563412;
 		//inp = 128'h11111111111111111111111111111111;
		
		//key = 128'hffeeddccbbaa00998877665544332211;
		//inp = 128'hffffffffffffffffffffffffffffffff;

		key = 128'hffffffffffffffffffffffffffffffff;
		inp = 128'hf0debc9a78563412f0debc9a78563412;

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

module serial_swan128k128_dec_test;

	parameter       BLOCK_SIZE = 128;
	parameter       KEY_SIZE = 128;
	// Inputs
	reg clk;
	reg start;
	reg rst;
	reg [0:KEY_SIZE-1] key;
	reg [0:BLOCK_SIZE-1] inp;

	// Outputs
	wire [0:BLOCK_SIZE-1] out;
	wire ready;
	
	serial_SWAN128K128_DEC  test(
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

		//key = 0;
		//inp = 128'h7471e75b14c448bbafc68d66170d1ac9;

 		//key = 128'h78563412785634127856341278563412;
		//inp = 128'hfaf0220b1ac0c6ce55a98207d26e67ec;
		
		//key = 128'hffeeddccbbaa00998877665544332211;
		//inp = 128'h6f63ea4d832b7ab471080cd8fe4519bc;

		key = 128'hffffffffffffffffffffffffffffffff;
		inp = 128'h8b9d520a184f4dc8189263a70c0eebba;

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


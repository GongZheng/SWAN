`timescale 1ns / 10ps

////////////////////////////////////////////////////////////////////////////////
// 
// encrypt test
// 
// test data 1: 
// key:    0x0
// in: 0x78563412785634127856341278563412
// out:0xf4875ea888a3c604e37f518527312c60
//
// test data 2: 
// key: 0x7856341278563412785634127856341278563412785634127856341278563412
// in:  0x11111111111111111111111111111111
// out: 0x515c0ec39623549ca5d50422f2863aad
// 
// test data 3: 
// key: 0xffeeddccbbaa00998877665544332211ffeeddccbbaa00998877665544332211
// in:  0xffffffffffffffffffffffffffffffff
// out: 0x171f0914f3ca2de46e566bddc4402d44
//
// test data 4: 
// key: 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
// in:  0xf0debc9a78563412f0debc9a78563412
// out: 0xbc2b49e1f1407a5d9cf78ff7db6c0634
////////////////////////////////////////////////////////////////////////////////

module serial_swan128k256_enc_test;

	parameter       BLOCK_SIZE = 128;
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
	

	SWAN128K256_ENC  test(
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

 		//key = 256'h7856341278563412785634127856341278563412785634127856341278563412;
 		//inp = 128'h11111111111111111111111111111111;
		
		//key = 256'hffeeddccbbaa00998877665544332211ffeeddccbbaa00998877665544332211;
		//inp = 128'hffffffffffffffffffffffffffffffff;


		key = 256'hffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
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



module serial_swan128k256_dec_test;

	parameter       BLOCK_SIZE = 128;
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
	

	SWAN128K256_DEC  test(
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
		//inp = 128'hf4875ea888a3c604e37f518527312c60;

 		//key = 256'h7856341278563412785634127856341278563412785634127856341278563412;
		//inp = 128'h515c0ec39623549ca5d50422f2863aad;
		
		//key = 256'hffeeddccbbaa00998877665544332211ffeeddccbbaa00998877665544332211;
		//inp = 128'h171f0914f3ca2de46e566bddc4402d44;


		key = 256'hffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
		inp = 128'hbc2b49e1f1407a5d9cf78ff7db6c0634;
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
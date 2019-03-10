`timescale 1ns / 10ps

////////////////////////////////////////////////////////////////////////////////
// 
// encrypt test
// 
// test data 1: 
// key:    0x0
// in:  0x7856341278563412785634127856341278563412785634127856341278563412
// out: 0x7bfc52c91831bb1925dfeb802f78e65949fca8b47b830fcb46b3b57620e3775c
//
// test data 2: 
// key: 0x7856341278563412785634127856341278563412785634127856341278563412
// in:  0x2222222222222222222222222222222211111111111111111111111111111111
// out: 0x5a1c0060f87906728590c57c01987ae8758914cb6ffaa5da7fe1fe1dc5cb5961
// 
// test data 3: 
// key: 0xffeeddccbbaa00998877665544332211ffeeddccbbaa00998877665544332211
// in:  0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaffffffffffffffffffffffffffffffff
// out: 0x55d7810f3881d576eef5df2374428adc1f9bba0e3cd6bc2cee264d10428bbf7e
//
// test data 4: 
// key: 0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff
// in:  0xf0debc9a78563412f0debc9a78563412f0debc9a78563412f0debc9a78563412
// out: 0x5e7f7837ab855ba2666046be47c2b93a435db79615506e3128b7fd3d1a0f22c2
////////////////////////////////////////////////////////////////////////////////

module serial_swan256_enc_test;

	parameter       BLOCK_SIZE = 256;
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
	

	serial_SWAN256_ENC  test(
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
 		//inp = 256'h7856341278563412785634127856341278563412785634127856341278563412;

 		//key = 256'h7856341278563412785634127856341278563412785634127856341278563412;
 		//inp = 256'h2222222222222222222222222222222211111111111111111111111111111111;
		
		//key = 256'hffeeddccbbaa00998877665544332211ffeeddccbbaa00998877665544332211;
		//inp = 256'haaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaffffffffffffffffffffffffffffffff;

		key = 256'hffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
		inp = 256'hf0debc9a78563412f0debc9a78563412f0debc9a78563412f0debc9a78563412;

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


module serial_swan256_dec_test;

	parameter       BLOCK_SIZE = 256;
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
	

	serial_SWAN256_DEC  test(
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
		//inp = 256'h7bfc52c91831bb1925dfeb802f78e65949fca8b47b830fcb46b3b57620e3775c;

 		//key = 256'h7856341278563412785634127856341278563412785634127856341278563412;
		//inp = 256'h5a1c0060f87906728590c57c01987ae8758914cb6ffaa5da7fe1fe1dc5cb5961;
		
		//key = 256'hffeeddccbbaa00998877665544332211ffeeddccbbaa00998877665544332211;
		//inp = 256'h55d7810f3881d576eef5df2374428adc1f9bba0e3cd6bc2cee264d10428bbf7e;

		key = 256'hffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff;
		inp = 256'h5e7f7837ab855ba2666046be47c2b93a435db79615506e3128b7fd3d1a0f22c2;

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
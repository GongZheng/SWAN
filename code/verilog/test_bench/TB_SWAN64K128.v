`timescale 1ns / 10ps

////////////////////////////////////////////////////////////////////////////////
// 
// encrypt test
// 
// test data 1: 
// key:    0x0
// in: 0x8877665544332211
// out:0x94db436fab46b0ca
//
// test data 2: 
// KEY:0x78563412785634127856341278563412
// in:0xefcdab9078563412
// out:0xb933b6ea2bc9455d
// 
// test data 3: 
// key:0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
// in:0xf0debc9a78563412
// out:0x9728ec401eb2271e
////////////////////////////////////////////////////////////////////////////////

module swan64k128_enc_test;

	parameter       BLOCK_SIZE = 64;
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
	

	SWAN64K128_ENC  test(
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
		//key = 128'h78563412785634127856341278563412;
 		//inp = 64'hfdbc9ac35a5b3351;
		
		//key = 0 ;
		//inp = 64'h8877665544332211;

		//key = 128'hffeeddccbbaa00998877665544332211 ;
		//inp = 64'h8877665544332211;
		key = 128'hffffffffffffffffffffffffffffffff;
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


module swan64k128_dec_test;

	parameter       BLOCK_SIZE = 64;
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
	
	SWAN64K128_DEC  test(
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
		//key = 128'h78563412785634127856341278563412;
		//inp = 64'hb933b6ea2bc9455d;
		
		//key = 0 ;
		//inp = 64'h94db436fab46b0ca;

		//key = 128'hffeeddccbbaa00998877665544332211 ;
		//inp = 64'he54f6f24ce27a660;
		key = 128'hffffffffffffffffffffffffffffffff;
		//inp = 64'hf0debc9a78563412;
		inp = 64'h9728ec401eb2271e;

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
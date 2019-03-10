/*
 *  THETA TEST BENCH:
 *      IN: 0x78563412     
 *      OUT: 0xf0951a12
 */
`timescale 1ns / 100ps

module theta_test;
    parameter       BLOCK_SIZE = 64;
    parameter       SIDE_SIZE = BLOCK_SIZE/2;
    parameter       COLUMN_SIZE  = SIDE_SIZE/4;

    reg   [0:(SIDE_SIZE-1)]   x;
    wire   [0:(SIDE_SIZE-1)]   y;

    vartheta u0(x, y);

    initial begin
        x = 32'h78563412;
        $monitor("value = %h", y);
    end



endmodule

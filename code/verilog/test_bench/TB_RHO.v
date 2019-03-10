
/*
 *  THETA TEST BENCH:
 *      IN: 0xc8110af6   
 *      OUT: 0xed342fd3
 */
`timescale 1ns / 100ps

module rho_test;
    parameter       BLOCK_SIZE = 64;
    parameter       SIDE_SIZE = BLOCK_SIZE/2;
    parameter       COLUMN_SIZE  = SIDE_SIZE/4;

    reg   [0:(SIDE_SIZE-1)]   x;
    wire   [0:(SIDE_SIZE-1)]   y;

    serial_rho u0(x, y);

    initial begin
        x = 32'hc8110af6;
        $monitor("value = %h", y);
    end



endmodule

/*
 *  THETA TEST BENCH:
 *      IN: 0xc3ca1a12
 *      OUT: 0xc8110af6   

 *      IN: 0xffffffff
 *      OUT: 0x0000ffff  
 */
`timescale 1ns / 100ps

module beta_test;
    parameter       BLOCK_SIZE = 64;
    parameter       SIDE_SIZE = BLOCK_SIZE/2;
    parameter       COLUMN_SIZE  = SIDE_SIZE/4;

    reg   [0:(SIDE_SIZE-1)]   x;
    wire   [0:(SIDE_SIZE-1)]   y;

    beta_table x0(.x(x), .y(y));

    initial begin
        x = 32'hc3ca1a12;
        $monitor("value = %h", y);
        //x = 32'hffffffff;
        //$monitor("value = %h", y);
    end



endmodule
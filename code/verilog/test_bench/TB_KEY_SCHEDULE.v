`timescale 1ns / 100ps

/*
 *     
 *  KEY:0
 *  result of each half round:
 *  round0:9e3779b9
 *  round1:3c6ef410
 *  round2:daa66d67
 *  round3:78dde7be
 *  round4:17156115
 *  round5:ecc6936d
 *  round6:c27864fb
 *  round7:9829358a
 *  round8:6ddb0619
 *  round9:438bd6a7
 *  round10:92f5a836
 *  round11:e2feb03e
 *  round12:3206b947
 *  round13:820ec150
 *  round14:d116c959
 *  round15:d91fd261
 *  round16:805f5422
 *  round17:269fd682
 *  round18:ccdf58e1
 *  round19:731edc40
 *  round20:1a5f5da0
 *  round21:f8189800
 *  round22:d6d27297
 *  round23:b48c4b2e
 *  round24:924723c5
 *  round25:70fffd5c
 *  round26:c871d6f3
 *  round27:2083e804
 *  round28:7893f815
 *  round29:d1a40926
 *  round30:28b51a38
 *  round31:38c62a48
 *  round32:e90eb511
 *  round33:9756407b
 *  round34:459ecae2
 *  round35:f4e75649
 *  round36:a42ee0b1
 *  round37:8af1231a
 *  round38:70b406b9
 *  round39:5775e758
 *  round40:3e38c7f8
 *  round41:23faa998
 *  round42:84748c36
 *  round43:e58fa550
 *  round44:45a7be6a
 *  round45:a6bfd783
 *  round46:06d8f19d
 *  round47:1ef308b6
 *  round48:d8439c87
 *  round49:8e9430fa
 *  round50:44e4c369
 *  round51:fc3656d8
 *  round52:b484ea49
 *  round53:a35034ba
 *  round54:921c2162
 *  round55:80e60a0a
 *  round56:70b0f2b1
 *  round57:5d7bdd5a
 *  round58:c6fdc800
 *  round59:3121ea22
 *  round60:99420b46
 *  round61:02622c67
 *  round62:6b844e89
 *  round63:8ba66eab
 */

module key_schedule_test;

    parameter       BLOCK_SIZE = 128;
    parameter       SIDE_SIZE = BLOCK_SIZE/2;
    parameter       COLUMN_SIZE  = SIDE_SIZE/4;
    parameter       KEY_SIZE = 128;
    parameter       DELTA0 = 64'h9e3779b97f4a7c15; 

    reg [0:KEY_SIZE-1] key;
    reg [7:0] counter;
    reg clk;
    reg ready;
    reg [0:SIDE_SIZE-1] delta;

    wire [0:SIDE_SIZE-1] sk;
    wire [0:KEY_SIZE-1] next_key;
    wire [0:SIDE_SIZE-1] next_delta;

    enc_key_schedule_128 u0(key, delta, next_key, next_delta, sk);

    always 
    begin
        #5;
        clk = !clk;
    end 

    initial begin
        clk = 0;
        counter = 95;
        key = 0;
        //key = 128'hffeeddccbbaa00998877665544332211;
 		//key = 128'h11111111111111111111111111111111;
        //delta = 0;
        delta = 0;
    end

    always @(posedge clk) 
        begin
            if(ready)begin
                //$display(counter);
                $stop;
                end
        end

    always @(posedge clk)
        begin
            $display("round%d:%x", counter, sk);
            key = next_key;
            delta = next_delta;
            ready = ~|counter;
            counter = counter - 1;
            $display("key:%x", key);
            $display("delta:%x",delta);
        end




endmodule
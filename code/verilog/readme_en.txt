1.verilog directory tree:
©¦  
©¦
©À©¤SWAN128
©¦  ©¦  dec_key_schedule_128.v
©¦  ©¦  dec_key_schedule_256.v
©¦  ©¦  enc_key_schedule_128.v
©¦  ©¦  enc_key_schedule_256.v
©¦  ©¦  sbox.v
©¦  ©¦
©¦  ©À©¤parallel
©¦  ©¦      beta_table.v
©¦  ©¦      rho.v
©¦  ©¦      SWAN128K128.v
©¦  ©¦      SWAN128K256.v
©¦  ©¦      vartheta.v
©¦  ©¦
©¦  ©¸©¤serial
©¦          serial_beta.v
©¦          serial_rho.v
©¦          serial_SWAN128K128.v
©¦          serial_SWAN128K256.v
©¦          serial_theta_key.v
©¦          serial_vartheta.v
©¦
©À©¤SWAN256
©¦  ©¦  dec_key_schedule.v
©¦  ©¦  enc_key_schedule.v
©¦  ©¦  sbox.v
©¦  ©¦
©¦  ©À©¤parallel
©¦  ©¦      beta_table.v
©¦  ©¦      rho.v
©¦  ©¦      SWAN256.v
©¦  ©¦      vartheta.v
©¦  ©¦
©¦  ©¸©¤serial
©¦          serial_beta.v
©¦          serial_rho.v
©¦          serial_SWAN256.v
©¦          serial_theta_key.v
©¦          serial_vartheta.v
©¦
©À©¤SWAN64
©¦  ©¦  dec_key_schedule_128.v
©¦  ©¦  dec_key_schedule_256.v
©¦  ©¦  enc_key_schedule_128.v
©¦  ©¦  enc_key_schedule_256.v
©¦  ©¦  sbox.v
©¦  ©¦
©¦  ©À©¤parallel
©¦  ©¦      beta_table.v
©¦  ©¦      rho.v
©¦  ©¦      SWAN64K128.v
©¦  ©¦      SWAN64K256.v
©¦  ©¦      vartheta.v
©¦  ©¦
©¦  ©¸©¤serial
©¦          serial_beta.v
©¦          serial_rho.v
©¦          serial_SWAN64K128.v
©¦          serial_SWAN64K256.v
©¦          serial_theta_key.v
©¦          serial_vartheta.v
©¦
©¸©¤test_bench
        TB_BETA.v
        TB_KEY_SCHEDULE.v
        TB_RHO.v
        TB_serial_SWAN128K128.v
        TB_serial_SWAN128K256.v
        TB_serial_SWAN256.v
        TB_serial_SWAN64K128.v
        TB_serial_SWAN64K256.v
        TB_SWAN128K128.v
        TB_SWAN128K256.v
        TB_SWAN256.v
        TB_SWAN64K128.v
        TB_SWAN64K256.v
        TB_THETA.v

2.Directory Structure:
   As SWAN256£º
      In the root directory, "sbox.v", "dec_key_schedule.v" and "enc_key_schedule.v" for sbox implementation and round key generating.
           (1) In serial directory, it is the serial implementation.
           (2) In parallel directory, it is the parallel implementation.
           (3) In test_bench directory, it is the test bench code.

3.Code Test:
   For test SWAN256 under modelsim:
       (1) Create a "swan256" project£¬copy all the files under the SWAN256 directory£¬and the test_bench directory of "TB_SWAN256.v" and "TB_serial_SWAN256.v".
       (2) Compile the source file£¬choose "swan256_enc_test" at "start simulate".
       (3) Input "run -all" at console£¬the console outputs the ciphertext.
       (4) Choose "swan256_dec_test" at "start simulate" for decryption test.
       (5) "serial_swan256_enc_test" and "serial_swan256_dec_test" are compiled for serial encryption and decryption test.
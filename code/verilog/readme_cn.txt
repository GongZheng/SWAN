1.Verilog代码目录结构
│  
│
├─SWAN128
│  │  dec_key_schedule_128.v
│  │  dec_key_schedule_256.v
│  │  enc_key_schedule_128.v
│  │  enc_key_schedule_256.v
│  │  sbox.v
│  │
│  ├─parallel
│  │      beta_table.v
│  │      rho.v
│  │      SWAN128K128.v
│  │      SWAN128K256.v
│  │      vartheta.v
│  │
│  └─serial
│          serial_beta.v
│          serial_rho.v
│          serial_SWAN128K128.v
│          serial_SWAN128K256.v
│          serial_theta_key.v
│          serial_vartheta.v
│
├─SWAN256
│  │  dec_key_schedule.v
│  │  enc_key_schedule.v
│  │  sbox.v
│  │
│  ├─parallel
│  │      beta_table.v
│  │      rho.v
│  │      SWAN256.v
│  │      vartheta.v
│  │
│  └─serial
│          serial_beta.v
│          serial_rho.v
│          serial_SWAN256.v
│          serial_theta_key.v
│          serial_vartheta.v
│
├─SWAN64
│  │  dec_key_schedule_128.v
│  │  dec_key_schedule_256.v
│  │  enc_key_schedule_128.v
│  │  enc_key_schedule_256.v
│  │  sbox.v
│  │
│  ├─parallel
│  │      beta_table.v
│  │      rho.v
│  │      SWAN64K128.v
│  │      SWAN64K256.v
│  │      vartheta.v
│  │
│  └─serial
│          serial_beta.v
│          serial_rho.v
│          serial_SWAN64K128.v
│          serial_SWAN64K256.v
│          serial_theta_key.v
│          serial_vartheta.v
│
└─test_bench
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

2.目录结构说明
	以SWAN256为例：
		根目录下的sbox.v dec_key_schedule.v enc_key_schedule.v 分别为sbox实现以及加解密轮密钥生成
		serial目录下为串行实现
		parallel目录下为并行实现
		
	test_bench目录下是各个模块的测试代码

3.代码测试
	以在modelsim下运行SWAN256为例
	(1) 创建一个名为swan256的项目，并将SWAN256目录下的所有代码，以及test_bench下的TB_SWAN256.v TB_serial_SWAN256.v
	(2) 编译源文件 ，在start simulate中选择swan256_enc_test
	(3) 控制台中输入run -all ，控制台输出对应的密文
 	(4) 在start simulate中选择swan256_dec_test可选择对应解密模块进行测试，serial_swan256_enc_test和serial_swan256_dec_test为对应的串行化实现
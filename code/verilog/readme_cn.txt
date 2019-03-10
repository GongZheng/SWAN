1.Verilog����Ŀ¼�ṹ
��  
��
����SWAN128
��  ��  dec_key_schedule_128.v
��  ��  dec_key_schedule_256.v
��  ��  enc_key_schedule_128.v
��  ��  enc_key_schedule_256.v
��  ��  sbox.v
��  ��
��  ����parallel
��  ��      beta_table.v
��  ��      rho.v
��  ��      SWAN128K128.v
��  ��      SWAN128K256.v
��  ��      vartheta.v
��  ��
��  ����serial
��          serial_beta.v
��          serial_rho.v
��          serial_SWAN128K128.v
��          serial_SWAN128K256.v
��          serial_theta_key.v
��          serial_vartheta.v
��
����SWAN256
��  ��  dec_key_schedule.v
��  ��  enc_key_schedule.v
��  ��  sbox.v
��  ��
��  ����parallel
��  ��      beta_table.v
��  ��      rho.v
��  ��      SWAN256.v
��  ��      vartheta.v
��  ��
��  ����serial
��          serial_beta.v
��          serial_rho.v
��          serial_SWAN256.v
��          serial_theta_key.v
��          serial_vartheta.v
��
����SWAN64
��  ��  dec_key_schedule_128.v
��  ��  dec_key_schedule_256.v
��  ��  enc_key_schedule_128.v
��  ��  enc_key_schedule_256.v
��  ��  sbox.v
��  ��
��  ����parallel
��  ��      beta_table.v
��  ��      rho.v
��  ��      SWAN64K128.v
��  ��      SWAN64K256.v
��  ��      vartheta.v
��  ��
��  ����serial
��          serial_beta.v
��          serial_rho.v
��          serial_SWAN64K128.v
��          serial_SWAN64K256.v
��          serial_theta_key.v
��          serial_vartheta.v
��
����test_bench
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

2.Ŀ¼�ṹ˵��
	��SWAN256Ϊ����
		��Ŀ¼�µ�sbox.v dec_key_schedule.v enc_key_schedule.v �ֱ�Ϊsboxʵ���Լ��ӽ�������Կ����
		serialĿ¼��Ϊ����ʵ��
		parallelĿ¼��Ϊ����ʵ��
		
	test_benchĿ¼���Ǹ���ģ��Ĳ��Դ���

3.�������
	����modelsim������SWAN256Ϊ��
	(1) ����һ����Ϊswan256����Ŀ������SWAN256Ŀ¼�µ����д��룬�Լ�test_bench�µ�TB_SWAN256.v TB_serial_SWAN256.v
	(2) ����Դ�ļ� ����start simulate��ѡ��swan256_enc_test
	(3) ����̨������run -all ������̨�����Ӧ������
 	(4) ��start simulate��ѡ��swan256_dec_test��ѡ���Ӧ����ģ����в��ԣ�serial_swan256_enc_test��serial_swan256_dec_testΪ��Ӧ�Ĵ��л�ʵ��
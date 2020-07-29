/*	Interrupt priority configuration

The lower the value, the higher the priority, with 0 being the highest priority

exti---3	uart1--4	uart3---5

*/


/*	 �ļ���������

app_button: 							��ʼ��������һ�ξͷ�һ��ֵ����λ�������£����븴λOK���ܽ�����������
													
app_gpio_dido:						IO��ʼ��+�ⲿ�жϳ�ʼ��; ��ʱ�ɼ�����״̬	;	�����жϴ���

app_motion:								����˶�����+������λ�Ĵ���	

app_motor_home:						�����ԭ��ͻ���λ����

app_serial:								Z��δֹͣʱ������3�ɿ������ݴ�ӡ�Ŀ���

app_sys_control:				  �������� EEPROM����ȡ���� SPI�������� ������ٶ����� ��ԭ�㶨ʱ�������� �����λԤ����

inc_mbtmc429:				      inc_mbtmc429�忨��Դ��CMD������

inc_mb1616dev6:				    inc_mb1616dev6�忨��Դ��CMD������

*/

/*  v3.6.1 20190729  none rtos code-28.7K  

---The motor back to the origin cannot be interrupted by the motor stop command, that is, the motor back to the origin does not respond to the motion command
---When the motor is reset, all kinds of motor movement operation cannot be carried out, and it is not affected by pressure alarm
---When motor return to origin action failure , may send motor stop reset to jump out of the operation


1.Reorganize the standard library project,
  re-optimize the serial port command
	add the command stop printdata 
	switch serial port 3 no longer USES interlock

2.add iic io PCA9539A driver


*/


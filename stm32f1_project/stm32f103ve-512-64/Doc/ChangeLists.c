/*	Interrupt priority configuration

The lower the value, the higher the priority, with 0 being the highest priority

exti---3	uart1--4	uart3---5

*/


/*	 文件功能描述

app_button: 							开始按键按下一次就发一次值，复位按键按下，必须复位OK才能进行其他动作
													
app_gpio_dido:						IO初始化+外部中断初始化; 定时采集按键状态	;	输入中断处理

app_motion:								电机运动控制+触发限位的处理	

app_motor_home:						电机回原点和回限位操作

app_serial:								Z轴未停止时，串口3可控制数据打印的开关

app_sys_control:				  重启控制 EEPROM数存取控制 SPI操作定义 电机加速度配置 回原点定时器的配置 电机复位预处理

inc_mbtmc429:				      inc_mbtmc429板卡资源和CMD的配置

inc_mb1616dev6:				    inc_mb1616dev6板卡资源和CMD的配置

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


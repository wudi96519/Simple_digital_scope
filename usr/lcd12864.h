#ifndef __LCD12864_H_
#define __LCD12864_H_

#define  TOAST_TIME  20                               //吐司时间（不精确）
#define  TOAST_TEXT_TIME  5

extern void DELAY_LOOP();
extern void lcd_Init();                                     //初始化LCD
extern void lcd_PutStr(unsigned char row, \
                       unsigned char col, char *puts);      //显示字符串至指定位置,etc:```  lcd_PutStr(0,0,"hello world\0");
extern void Lcd_ClearBMP(void);                             //清空图像
extern void Lcd_ClearTXT(void);                             //清空汉字

extern void lcd_Sin_test();                                 //测试函数：绘制正弦波
extern void lcd_Draw_Sample();                              //绘制ADC采样结果
extern void lcd_Toast(char *puts);                          //Toast提示
extern void lcd_Show_Freq_Volt();                           //显示频率，幅值

#endif

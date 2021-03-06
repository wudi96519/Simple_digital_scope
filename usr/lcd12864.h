#ifndef __LCD12864_H_
#define __LCD12864_H_

#define TOAST_TEXT_TIME 5
#define TOAST_TIME 20

extern void DELAY_LOOP();
extern void lcd_Init();                                     //初始化LCD
extern void lcd_Clear_frambuff();                           //清空缓存
extern void lcd_PutStr(unsigned char row, \
                       unsigned char col, char *puts);      //显示字符串至指定位置,etc:```  lcd_PutStr(0,0,"hello world\0");
extern void Lcd_ClearBMP(void);                             //清空图像
extern void Lcd_ClearTXT(void);                             //清空汉字
extern void lcd_PutPointtoBuffer(int x,int y);              //画点至缓存

extern void lcd_Toast(char *puts);                          //Toast提示
extern void lcd_Clean_Screnn_With_Buffer();
extern void lcd_Show_Freq_Volt(double freq,double Voltage,double maxVoltage);
extern void lcd_Draw_With_Buffer();
extern void lcd_Draw_Curve(Uint16 * SampleTable,int INDEX_TO_DRAW,int LENGTH_TO_DRAW,int lessen_times);
#endif

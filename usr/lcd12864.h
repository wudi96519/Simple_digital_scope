#ifndef __LCD12864_H_
#define __LCD12864_H_

#define  TOAST_TIME  20                               //��˾ʱ�䣨����ȷ��
#define  TOAST_TEXT_TIME  5

extern void DELAY_LOOP();
extern void lcd_Init();                                     //��ʼ��LCD
extern void lcd_PutStr(unsigned char row, \
                       unsigned char col, char *puts);      //��ʾ�ַ�����ָ��λ��,etc:```  lcd_PutStr(0,0,"hello world\0");
extern void Lcd_ClearBMP(void);                             //���ͼ��
extern void Lcd_ClearTXT(void);                             //��պ���

extern void lcd_Sin_test();                                 //���Ժ������������Ҳ�
extern void lcd_Draw_Sample();                              //����ADC�������
extern void lcd_Toast(char *puts);                          //Toast��ʾ
extern void lcd_Show_Freq_Volt();                           //��ʾƵ�ʣ���ֵ

#endif


#ifndef USR_CONTROLCENTER_H_
#define USR_CONTROLCENTER_H_

extern int INDEX_TO_DRAW;
extern Uint16 up_or_down_flag;

extern Uint16 num_Of_index;
extern Uint16 sample_index;
extern int votage_Level;
extern int DISPLAY_CURVE;
extern int KEY5PRESSED;
extern int UP_OR_DOWN_DECIDE_VALUE;
extern void cc_all_Sys_Init();

extern void ISR_key1();
extern void ISR_key2();
extern void ISR_key3();
extern void ISR_key4();
extern void ISR_key5();
extern void ISR_key6();
extern void ISR_key7();
extern void ISR_key8();
extern void ISR_key9();
extern void cc_main_loop();




#endif /* USR_CONTROLCENTER_H_ */

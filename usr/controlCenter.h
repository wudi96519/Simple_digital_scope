
#ifndef USR_CONTROLCENTER_H_
#define USR_CONTROLCENTER_H_

/**
 * just for debug..
 */
#define EN_ADC 1
#define EN_KEY 1
#define EN_LCD 1
#define EN_DEBUG 0

extern int INDEX_TO_DRAW;

extern Uint16 num_Of_index;
extern Uint16 sample_index;
extern int votage_Level;
extern int DISPLAY_CURVE;

extern void all_Sys_Init();
extern void get_trigger_index(void);

extern void ISR_key1();
extern void ISR_key2();
extern void ISR_key3();
extern void ISR_key4();
extern void ISR_key5();
extern void ISR_key6();
extern void ISR_key7();
extern void ISR_key8();
extern void ISR_key9();




#endif /* USR_CONTROLCENTER_H_ */

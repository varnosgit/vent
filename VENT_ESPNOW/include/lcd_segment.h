#include <Arduino.h>
// #include <math.h>
// #include <string.h>
// #include <stdint.h>

//uint8_t cfStat = 0;
//int SP=25;
//HT1621_Map Seg_LCD; 

#define  BIAS     0x52             //0b1000 0101 0010  1/3duty 4com
#define  SYSDIS   0X00             //0b1000 0000 0000  关振系统荡器和LCD偏压发生器
#define  SYSEN    0X02             //0b1000 0000 0010 打开系统振荡器
#define  LCDOFF   0X04             //0b1000 0000 0100  关LCD偏压
#define  LCDON    0X06             //0b1000 0000 0110  打开LCD偏压
#define  XTAL     0x28             //0b1000 0010 1000 外部接时钟
#define  RC256    0X30             //0b1000 0011 0000  内部时钟
#define  TONEON   0X12             //0b1000 0001 0010  打开声音输出
#define  TONEOFF  0X10             //0b1000 0001 0000 关闭声音输出
#define  WDTDIS1  0X0A             //0b1000 0000 1010  禁止看门狗
#define  BUFFERSIZE 12


#define  ComMode        0x52  //4commons, 1/3bias  1000 010 10 0 1 0  
#define  RCosc          0x30  //Clock set: On chip RC 1000 0011 0000 
#define  LCD_on         0x06  //Turn on  LCD 1000 0000 0110 
#define  LCD_off        0x04  //Turn off LCD 1000 0000 0100
#define  Sys_en         0x02  //Turn oscil.on 1000 0000 0010 
#define  Sys_dis        0x00  //Turn oscil.off 1000 0000 0000 AO!
#define  CTRl_cmd   0x80  //Next byte is a command.
#define  Data_cmd   0xa0  //Next byte is a data

typedef struct {
//Define the character (digit) to LCD memory address mapping
// For example, to display '0' we must write 0x7D :

  uint8_t digit[10][2];     //Individual digits on the display.
  uint8_t degree1,degree2; //Location of the decimal point.
  uint8_t bat1,bat2,bat3;
  uint8_t ant1,ant2,ant3;
  uint8_t register_data[13];
}HT1621_Map;



//  functions:
void HT1621_SendBits(uint8_t sdata,uint8_t cnt); //Send cnt MSB bits of sdata.
void HT1621_SendCmd(uint8_t command); //Write to the command registers.
void HT1621_Write(uint8_t addr,uint8_t sdata); //Write sdata to address addr:
void HT1621_AllOff(uint8_t number); //Clear all segments.
void HT1621_AllOn(uint8_t number); //Set all segments.
void HT1621_Init(void);
void LCD_PW_Ctrl(bool s);
void LCD_LED(int s);

void LCD_Digit(int d,char s);
void LCD_Data_l(int ad,int s);       // this function Hold C8,C4,C2 status and change C1 status as a Sign Bit
void LCD_Data_h(int ad,uint8_t sd);    // this function Hold C1 status and change C8,C4,C2 status as a Digital Bits
void LCD_OFF(void);

void LCD_Initialize(void);
void LCD_test_run(void);









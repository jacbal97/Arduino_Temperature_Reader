#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);
#define Temp_pin 2
String string = "Temp : ";
byte counter = 0;
int tRead;
int signBit;
float value;
void OneWireReset(int Pin) // reset
{
 digitalWrite(Pin, LOW);
 pinMode(Pin, OUTPUT);
 delayMicroseconds(500); // bring low for 500 us
 pinMode(Pin, INPUT);
 delayMicroseconds(500);
}
void OneWireOutByte(int Pin, byte d) // output byte d (least sig bit first).
{
 byte n;
 for (n = 8; n != 0; n--)
 {
 if ((d & 0x01) == 1) // test least sig bit
 {
 digitalWrite(Pin, LOW);
 pinMode(Pin, OUTPUT);
 delayMicroseconds(5);
 pinMode(Pin, INPUT);
 delayMicroseconds(60);
 }
 else
 {
 digitalWrite(Pin, LOW);
 pinMode(Pin, OUTPUT);
 delayMicroseconds(60);
 pinMode(Pin, INPUT);
 }
 d = d >> 1; // now the next bit is in
 // the least sig bit position.
 }
}
byte OneWireInByte(int Pin) // read byte, least sig byte first
{
 byte d, b;
 d = 0;
 for (counter = 0; counter < 8; counter++)
 {
 digitalWrite(Pin, LOW);
 pinMode(Pin, OUTPUT);
 delayMicroseconds(5);
 pinMode(Pin, INPUT);
 delayMicroseconds(5);
 b = digitalRead(Pin);
 delayMicroseconds(50);
 d = (d >> 1) | (b << 7); // shift d to right and
 //insert b in most sig bit position
 }
 return (d);
}
void tempConvert(byte Pin) //request to convert temp
{
 OneWireReset(Pin);
 OneWireOutByte(Pin, 0xcc); //skip rom
 OneWireOutByte(Pin, 0x44); // Convert T [44h]
}
void tempRead(byte Pin) //request to transmit temp
{
 int HighByte, LowByte;
 OneWireReset(Pin);
 OneWireOutByte(Pin, 0xcc); //skip rom
 OneWireOutByte(Pin, 0xbe); // Read Scratchpad [BEh]
 LowByte = OneWireInByte(Pin);
 HighByte = OneWireInByte(Pin);
 tRead = (HighByte << 8) + LowByte;
 signBit = tRead & 0x8000; // test most sig bit
 if (signBit) // negative
 {
 tRead = (tRead ^ 0xffff) + 1; // 2's comp
 value = ((6.0 * tRead) + tRead / 4.0)/-100;
 }
 else
 value = ((6.0 * tRead) + tRead / 4.0)/100;
}
void setup()
{
 //lcd init
 lcd.init();
 lcd.backlight();
}
void loop()
{
 tempConvert(Temp_pin);
 delay(1900); //recommended wait time for temp conversion
 tempRead(Temp_pin);
 delay(100);
 lcd.setCursor(1, 0);
 lcd.print(string);
 lcd.print(value);

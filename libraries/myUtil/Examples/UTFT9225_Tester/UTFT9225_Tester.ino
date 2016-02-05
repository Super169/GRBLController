#include "myUtil.h"


#include "SuperUI_UTFT9225.h"
#include <SPI.h>
#include "UTFT.h"
UTFT myUTFT(QD220A,A2,A1,A5,A4,A3);
SuperUI_UTFT9225 myUI(myUTFT);

void setup() {

  Serial.begin(57600);
  
  myUI.println("Before myUI.begin()");
  myUI.begin();

  myUI.println("After myUI.begin()");
  myUI.whoAmI();
  myUI.println("After whoAmI");

  myUtil::myDelay(1000);

  myUI.print("Part I");
  myUI.print(" | Part II");
  myUI.print(" | Part III");
  myUI.print(myUI.getVersion());
  myUI.println();

  myUI.print(123);
  myUI.print(123, 6);
  myUI.print(123, 6, 2);
  myUI.println();

  myUI.println(123);
  myUI.println(123, 6);
  myUI.println(123, 6, 2);

  myUI.print(123.456, 2, 5, 0);
  myUI.print(123.456, 2, 10, 0);
  myUI.print(123.456, 2, 10, 1);
  myUI.print(123.456, 2, 10, 2);
  myUI.println();

  myUI.println(123.456, 2, 5, 0);
  myUI.println(123.456, 2, 10, 0);
  myUI.println(123.456, 2, 10, 1);
  myUI.println(123.456, 2, 10, 2);

  myUI.println("The End");

  myUtil::myDelay(1000);

  myUI.cls(4);
  myUI.show(4, 12, "at (4,12)");
  myUI.showXY(67, 111, "xy(67,111)");
  myUtil::myDelay(1000);

  myUI.show(5, "at line 5");
  myUI.show(9, "at line 9");
  myUtil::myDelay(1000);

  myUI.fill();
  myUI.show(5,0,"012345678901234567890123456");
  myUI.show(6,0,12.3456,2,8,0);
  myUI.show(7,0,12.3456,2,8,1);
  myUI.show(8,0,12.3456,2,8,2);

  myUI.showXY(84,78,12.3456,2,8,0);
  myUI.showXY(84,90,12.3456,2,8,1);
  myUI.showXY(84,102,12.3456,2,8,2);

  myUI.show(10,2,15);
  myUI.show(11,2,16,8);
  myUI.show(12,2,17,8,2);

  myUI.showXY(84,126,18);
  myUI.showXY(84,138,19,8);
  myUI.showXY(84,150,20,8,2);

}


void loop() {

}
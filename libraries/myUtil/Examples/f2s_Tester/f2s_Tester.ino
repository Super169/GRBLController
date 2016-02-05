#include "myUtil.h"

void setup() {
  Serial.begin(57600);
  Serial.println(myUtil::availableMemory(2048));

  myUtil::myDelay(1000);
  
  showData(1.2345,3,9, 1);
  showData(1.2345,3,8, 1);
  showData(1.2345,3,7, 1);
  showData(1.2345,3,6, 1);
  showData(1.2345,3,5, 1);
  showData(1.2345,3,4, 1);
  showData(1.2344,3,4, 1);
  showData(1.2344,3,3, 1);
  showData(1.2344,3,2, 1);
  showData(1.2344,3,1, 1);

  showData(123.2345,4,10, 1);
  showData(123.2345,4,9, 1);
  showData(123.2345,4,8, 1);
  showData(123.2345,4,7, 1);
  showData(123.2345,4,6, 1);
  showData(123.2345,4,5, 1);
  showData(123.2345,4,4, 1);
  showData(123.2345,4,3, 1);
  showData(123.2345,4,2, 1);
  showData(123.2345,4,1, 1);

  showData(-123.2345,4,10, 1);
  showData(-123.2345,4,9, 1);
  showData(-123.2345,4,8, 1);
  showData(-123.2345,4,7, 1);
  showData(-123.2345,4,6, 1);
  showData(-123.2345,4,5, 1);
  showData(-123.2345,4,4, 1);
  showData(-123.2345,4,3, 1);
  showData(-123.2345,4,2, 1);
  showData(-123.2345,4,1, 1);

  showData(999.9933,4,10, 1);
  showData(999.9933,4,9, 1);
  showData(999.9933,4,8, 1);
  showData(999.9933,4,7, 1);
  showData(999.9933,4,6, 1);
  showData(999.9933,4,5, 1);
  showData(999.9933,4,4, 1);
  showData(999.9933,4,3, 1);
  showData(999.9933,4,2, 1);
  showData(999.9933,4,1, 1);

  showData(-999.9933,4,10, 1);
  showData(-999.9933,4,9, 1);
  showData(-999.9933,4,8, 1);
  showData(-999.9933,4,7, 1);
  showData(-999.9933,4,6, 1);
  showData(-999.9933,4,5, 1);
  showData(-999.9933,4,4, 1);
  showData(-999.9933,4,3, 1);
  showData(-999.9933,4,2, 1);
  showData(-999.9933,4,1, 1);

  showData(4294967000,3,15, 1);
  showData(4294967000,3,14, 1);
  showData(4294967000,3,13, 1);
  showData(4294967000,3,12, 1);
  showData(4294967000,3,11, 1);
  showData(4294967000,3,10, 1);
  showData(4294967000,3,9, 1);
  showData(4294967000,3,8, 1);

  showData(-2147480000,3,15, 1);
  showData(-2147480000,3,14, 1);
  showData(-2147480000,3,13, 1);
  showData(-2147480000,3,12, 1);
  showData(-2147480000,3,11, 1);
  showData(-2147480000,3,10, 1);
  showData(-2147480000,3,9, 1);
  showData(-2147480000,3,8, 1);

  showData(1.2345,3,9, 0);
  showData(1.2345,3,9, 1);
  showData(1.2345,3,9, 2);
  Serial.println();

  showData(123.2345,4,10, 0);
  showData(123.2345,4,10, 1);
  showData(123.2345,4,10, 2);
  Serial.println();

  showData(-123.2345,4,10, 0);
  showData(-123.2345,4,10, 1);
  showData(-123.2345,4,10, 2);
  Serial.println();

  showData(999.9933,4,10, 0);
  showData(999.9933,4,10, 1);
  showData(999.9933,4,10, 2);
  Serial.println();

  showData(-999.9933,4,10, 0);
  showData(-999.9933,4,10, 1);
  showData(-999.9933,4,10, 2);
  Serial.println();

  showData(4294967000,3,15, 0);
  showData(4294967000,3,15, 1);
  showData(4294967000,3,15, 2);
  Serial.println();

  showData(-2147480000,3,15, 0);
  showData(-2147480000,3,15, 1);
  showData(-2147480000,3,15, 2);
  Serial.println();


}

boolean showData(double data, uint8_t decimal, uint8_t width, uint8_t padSpace) {

  char buffer[20];
  myUtil::f2s(buffer, data, decimal, width, padSpace);

  Serial.print(data, 6);
  Serial.print(" ( ");
  Serial.print(decimal);
  Serial.print(" : ");
  Serial.print(width);
  Serial.print(" ) -> [");
  Serial.print(buffer);
  Serial.print("]");
  Serial.println();
}


void loop() {

}
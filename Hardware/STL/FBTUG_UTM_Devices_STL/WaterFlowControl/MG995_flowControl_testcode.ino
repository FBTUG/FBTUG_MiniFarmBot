#include <Servo.h>

Servo myservo;  // 建立一個 servo 物件，最多可建立 12個 servo

int pos = 0;    // 設定 Servo 位置的變數
const int CloseWaterFlow = 0;           // 設0度為關閉水流量開關
const int FullWaterFlow = 70;           // 設70度為最大水流量

void setup() {
  myservo.attach(9);  // 將 servo 物件連接到 pin 9
}

void loop() {
  // 正轉 70度:打開水流量到最大
  for (pos = CloseWaterFlow; pos <= FullWaterFlow; pos += 1) // 從 0 度旋轉到 70 度，每次 1 度 
  {     
    myservo.write(pos);               // 告訴 servo 走到 'pos' 的位置
    delay(15);                        // 等待 15ms 讓 servo 走到指定位置
  }
   delay(1000);  
   //反轉 70度: 關閉水流量
   for (pos = FullWaterFlow; pos >= CloseWaterFlow; pos -= 1) // 從 70 度旋轉到 0 度，每次 1 度 
  { 
    myservo.write(pos);               // 告訴 servo 走到 'pos' 的位置

    delay(15);                        // 等待 15ms 讓 servo 走到指定位置
  }
  delay(1000);  

}

/*
SSID        S1:TV_GAME
PASSWORD    S2:1234567890
ServerIP    S3:192.168.1.161
ServerPort  S4:9000
LocalPort   S5:8900
AP_SSID     S6:miniFarmBot
AP_PASSWORD S7:1234567890
*/

#include <WiFi.h>

#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

//CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

//--------------------------------------------------------
// Webserver / Controls Function -
void startCameraServer();

//------------------------------
#include <WiFiUdp.h>

//- 要把空間, 空出來, 否則會有覆蓋,錯亂的問題
char vSSID[30];             //  your network SSID (name)
char vPASSWORD[30];         // your network password
char vServerIP[20]; 

int vServerPort = 9000;
int vLocalPort = 8990;

char vAP_SSID[30];             //  your network SSID (name)
char vAP_PASSWORD[30];         // your network password

char incomingPacket[255];

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
IPAddress maddr;

//----------------------------------------  
#include <EEPROM.h>

//----------------------------------------
char inChar;

int packetSize = 0;
int len = 0;

//---------------------------
uint addr = 0;

struct{
    uint val = 0;
    char str[20] = ""; 
} data;
    
char vStr[20] = "";
String vSS = "";
byte vLen = 0;
byte vStrLen = 0;

int vWiFi_OK_Count = 0;

String vMSG = "";

//- Serial -
int vInSeriaCount = 0;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

String pPC_CMD = "";
   
//--------------------------------------------------------------
void setup() {
    delay(1000);    //-電容蓄電-wait Ready  
      
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  //掉電檢測器重啟ESP32-關
   
    Serial.begin(115200);
    //Serial.begin(9600);   //BT  

    //默認情況下當你調用 Serial.begin，Wifi庫診斷輸出是禁用的。
    Serial.setDebugOutput(false);   //-BUG 設 false 一樣有 Wifi庫診斷輸出

    Serial.println();    

    //------------------------------------- 
    fEEPROM_Set();  

    //-------------------------------------
    // reserve 200 bytes for the inputString
    inputString.reserve(200);  
    
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;

    //Format of the pixel data: PIXFORMAT_ + YUV422|GRAYSCALE|RGB565|JPEG 
    config.pixel_format = PIXFORMAT_JPEG;
    
    /*
    FRAMESIZE_96x96,    // 96x96
    FRAMESIZE_QQVGA,    // 160x120
    FRAMESIZE_QQVGA2,   // 128x160
    FRAMESIZE_QCIF,     // 176x144
    FRAMESIZE_HQVGA,    // 240x176
    FRAMESIZE_240x240,  // 240x240
    FRAMESIZE_QVGA,     // 320x240
    FRAMESIZE_CIF,      // 400x296
    FRAMESIZE_VGA,      // 640x480
    FRAMESIZE_SVGA,     // 800x600
    FRAMESIZE_XGA,      // 1024x768
    FRAMESIZE_SXGA,     // 1280x1024
    FRAMESIZE_UXGA,     // 1600x1200
    FRAMESIZE_QXGA,     // 2048*1536
    */
        
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);     
        delay(1000);
        ESP.restart();    //return;
    }
  
    //drop down frame size for higher initial frame rate
    sensor_t * s = esp_camera_sensor_get();
    s->set_quality(s, 10);   //10, 12, 0-63 lower number means higher quality
    s->set_framesize(s, FRAMESIZE_VGA);  //VGA640x480, UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA

    //s->set_vflip(s, 1);
    //s->set_hmirror(s, 1);

    // Initialize Flash, GPIO4 LED
    ledcSetup(4, 5000, 8);  //PWM Ch4, 5000 hz PWM, 8-bit resolution
    ledcAttachPin(4, 4);    //GPIO4, Mapping to Ch4  

    //-----------------------------------------------
    // We start by connecting to a WiFi network
    fWiFi_Connect();
         
    //------------------------------------------------
    startCameraServer();
}

//--------------------------------------------------------------
void loop() {
    fUDP_Proc();

    fSerial_Proc();
}

//-----------------------------------------------------
void fEEPROM_Set()
{
    EEPROM.begin(512);

    // load EEPROM data into RAM, see it
    //-------------------------------------
    addr = 0; EEPROM.get(addr, vLen); vStrLen = vLen;
        
    addr = 0 + 1; strncpy(data.str, "", 30);
    EEPROM.get(addr, data);

    vSS = data.str;
    vSS = vSS.substring(0, vStrLen);  //取第0到第1之前    
    //char* vSSID     = "TV_GAME";       //-Max 32
    strcpy(vSSID, vSS.c_str());      
    Serial.println("SSID: " + String(vStrLen) + "," + String(vSSID));  
    
    //-------------------------------------
    addr = 32; EEPROM.get(addr, vLen); vStrLen = vLen;
    
    addr = 32 + 1; strncpy(data.str, "", 30);
    EEPROM.get(addr, data);    

    vSS = data.str;
    vSS = vSS.substring(0, vStrLen);  //取第0到第1之前
    //char* vPASSWORD = "591220591220";  //-Max 32
    strcpy(vPASSWORD, vSS.c_str());
    Serial.println("PASSWORD: " + String(vStrLen) + "," + String(vPASSWORD));  
   
    //-------------------------------------
    addr = 64; EEPROM.get(addr, vLen); vStrLen = vLen;
    
    addr = 64 + 1; strncpy(data.str, "", 20);
    EEPROM.get(addr, data);    

    vSS = data.str;
    vSS = vSS.substring(0, vStrLen);  //取第0到第1之前
    //char* vServerIP = "192.168.1.160";  //-Max 20
    strcpy(vServerIP, vSS.c_str());
    Serial.println("ServerIP: " + String(vStrLen) + "," + String(vServerIP));  

    //-------------------------------------
    addr = 90; EEPROM.get(addr, vServerPort);   //int vServerPort = 9000;
    Serial.println("ServerPort: " + String(vServerPort));  
    
    addr = 94; EEPROM.get(addr, vLocalPort);    //int vLocalPort = 8990;
    Serial.println("LocalPort: " + String(vLocalPort));  

    //-------------------------------------
    addr = 128; EEPROM.get(addr, vLen); vStrLen = vLen;
        
    addr = 128 + 1; strncpy(data.str, "", 30);
    EEPROM.get(addr, data);

    vSS = data.str;
    vSS = vSS.substring(0, vStrLen);  //取第0到第1之前    
    //char* AP_SSID     = "miniFarmBot";       //-Max 32
    strcpy(vAP_SSID, vSS.c_str());      
    Serial.println("AP_SSID: " + String(vStrLen) + "," + String(vAP_SSID));  

    //-------------------------------------
    addr = 160; EEPROM.get(addr, vLen); vStrLen = vLen;
    
    addr = 160 + 1; strncpy(data.str, "", 30);
    EEPROM.get(addr, data);    

    vSS = data.str;
    vSS = vSS.substring(0, vStrLen);  //取第0到第1之前
    //char* vAP_PASSWORD = "1234567890";  //-Max 32
    strcpy(vAP_PASSWORD, vSS.c_str());
    Serial.println("AP_PASSWORD: " + String(vStrLen) + "," + String(vAP_PASSWORD)); 
    
    //-------------------------------------  
    Serial.println("");
}

//-----------------------------------------------------
void fWiFi_Connect(){
int iii = 0;
  
    // We start by connecting to a WiFi network
    Serial.print("Connecting to "); Serial.println(vSSID);

    //WIFI_AP :     AP 模式
    //WIFI_STA :    Client 模式
    //WIFI_AP_STA : 同時是 AP 也是 Client
    //WIFI_OFF :    關閉
    
    //WiFi.mode(WIFI_STA);
    //WiFi.mode(WIFI_AP_STA);   //-109.02.03
    
    WiFi.begin(vSSID, vPASSWORD);
    
    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(F("."));
          
        vWiFi_OK_Count++;
        if(vWiFi_OK_Count > 20){    //10's-TimeOut
            break;  
        }
    }
    
    Serial.println("");
    Serial.print("Camera Ready! Use 'http://");
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print(WiFi.localIP());
        Serial.println("' to connect");
        
        WiFi.softAP((WiFi.localIP().toString() + "_" + (String)vAP_SSID).c_str(), vAP_PASSWORD);    
        
        for(iii = 0; iii < 5; iii++){    //閃燈5次, WiFi 連線 OK
            ledcWrite(4, 10);
            delay(200);
            ledcWrite(4, 0);
            delay(200);    
        }        
    }
    else {
        Serial.println("softAPI : " + WiFi.softAPIP().toString() + "_" + (String)vAP_SSID);
        Serial.println("Password : " + (String)vAP_PASSWORD);
    
        Serial.print(WiFi.softAPIP());
        Serial.println("' to connect");
        
        WiFi.softAP((WiFi.softAPIP().toString() + "_" + (String)vAP_SSID).c_str(), vAP_PASSWORD);    
        
        for (iii = 0; iii < 2; iii++) {   //閃燈2次, 等 Soft AP 連線
            ledcWrite(4, 10);
            delay(1000);
            ledcWrite(4, 0);
            delay(1000);    
        }  
    } 
    
    //-----------------------------------------------------------
    Udp.begin(vLocalPort);
    delay(100);
    maddr.fromString(vServerIP);  

    //-ESP32 'class WiFiUDP' has no member named 'localPort' ???
    vMSG = "IP: " +  WiFi.localIP().toString();
    fReturnUDP(vMSG);
}

//-----------------------------------------------------
void fUDP_Proc()
{
    packetSize = Udp.parsePacket();
    
    if(packetSize){        
        len = Udp.read(incomingPacket, 255);
        if (len > 0) incomingPacket[len] = 0;        
        Serial.printf("%s\n", incomingPacket);  //- UDP 收到 送到 UART    
    }    
}
    
//-----------------------------------------------------
void fReturnUDP(String vMSG){    
char vCharArray_Buf[vMSG.length() + 1];
    
    vMSG.toCharArray(vCharArray_Buf, vMSG.length() + 1);
    udpWriteCmd(vCharArray_Buf);  
}

//-------------------------------------------------------------------
void udpWriteCmd(char* cmd){
  
    Udp.beginPacket(maddr, vServerPort);
    Udp.write((const uint8_t*)cmd, strlen(cmd));
    Udp.endPacket();
}
  
//-----------------------------------------------------
void fSerial_Proc(){
    if(Serial.available()) {
        // get the new byte:
        inChar = (char)Serial.read(); 
    
        vInSeriaCount++;
    
        if(inChar == '\n'){    //ASCII End Char
            stringComplete = true;
            vInSeriaCount = 0;
        } 
        else{
            inputString += inChar;  
        }
    }
    
    if(stringComplete == true){       
        pPC_CMD = inputString.substring(0, 2);
    
        //--------------------------------------------------------------------                
        vLen = inputString.length();        
        String vData = inputString.substring(3, vLen);
        
        vLen = vData.length();  
        char comdata[vLen + 1];
        vData.toCharArray(comdata, vLen + 1);
    
        //-寫入 EEPROM
        if(pPC_CMD == "S1"){    //-vSSID S1:TV_GAME
            strncpy(data.str, "", 30);            
            vSS = vData; strcpy(vSSID, vSS.c_str());  
            vLen = vSS.length();  //-Len(byte-1Byte)
            vSS.toCharArray(vStr, vLen + 1);
            
            addr = 0; EEPROM.put(addr, vLen);
               
            addr = 0 + 1; data.val = vLen; //-Max 30
            strncpy(data.str, vStr, data.val);      
    
            //Serial.println(String(vLen) + "," + String(vSSID));
            Serial.println("SSID: " + String(vSSID));
            
            EEPROM.put(addr, data);   // replace values in EEPROM                
            EEPROM.commit();    //-寫入EEPROM         
        }
        else if(pPC_CMD == "S2"){    //-vPASSWORD S2:591220591220
            strncpy(data.str, "", 30);
            vSS = vData; strcpy(vPASSWORD, vSS.c_str());
            vLen = vSS.length();  //-Len(byte-1Byte)
            vSS.toCharArray(vStr, vLen + 1);
    
            addr = 32; EEPROM.put(addr, vLen);
               
            addr = 32 + 1; data.val = vLen; //-Max 30
            strncpy(data.str, vStr, data.val);      
    
            //Serial.println(String(vLen) + "," + String(vPASSWORD));
            Serial.println("PASSWORD: " + String(vPASSWORD));
            
            EEPROM.put(addr, data);   // replace values in EEPROM          
            EEPROM.commit();    //-寫入EEPROM   
        }
        else if(pPC_CMD == "S3"){    //ServerIP S3:192.168.1.160
            strncpy(data.str, "", 20);
            vSS = vData; strcpy(vServerIP, vSS.c_str());
            vLen = vSS.length();  //-Len(byte-1Byte)
            vSS.toCharArray(vStr, vLen + 1);
    
            addr = 64; EEPROM.put(addr, vLen);
               
            addr = 64 + 1; data.val = vLen; //-Max 20
            strncpy(data.str, vStr, data.val);      
    
            //Serial.println(String(vLen) + "," + String(vServerIP));
            Serial.println("ServerIP: " + String(vServerIP));
            
            EEPROM.put(addr, data);   // replace values in EEPROM
            EEPROM.commit();    //-寫入EEPROM   
        }
        else if(pPC_CMD == "S4"){    //-ServerPort
            addr = 90;  //-90,91
            vServerPort = atoi(comdata);  
            
            //Serial.println(String(vServerPort));
            Serial.println("ServerPort: " + String(vServerPort));
            
            EEPROM.put(addr, vServerPort);   //int vServerPort = 9000; (int-2Byte)          
            EEPROM.commit();    //-寫入EEPROM        
        }
        else if(pPC_CMD == "S5"){    //-LocalPort
            addr = 94;  //-94,95
            vLocalPort = atoi(comdata);  
            
            //Serial.println(String(vLocalPort));
            Serial.println("LocalPort: " + String(vLocalPort));
            
            EEPROM.put(addr, vLocalPort);    //int vLocalPort = 8900;
            EEPROM.commit();    //-寫入EEPROM   
        }
        if(pPC_CMD == "S6"){    //-vAP_SSID S6:PowerCAR-01
            strncpy(data.str, "", 30);            
            vSS = vData; strcpy(vAP_SSID, vSS.c_str());  
            vLen = vSS.length();  //-Len(byte-1Byte)
            vSS.toCharArray(vStr, vLen + 1);
            
            addr = 128; EEPROM.put(addr, vLen);
               
            addr = 128 + 1; data.val = vLen; //-Max 30
            strncpy(data.str, vStr, data.val);      

            Serial.println("AP_SSID: " + String(vAP_SSID));
            
            EEPROM.put(addr, data);   // replace values in EEPROM                
            EEPROM.commit();    //-寫入EEPROM         
        }
        else if(pPC_CMD == "S7"){    //-vAP_PASSWORD S7:1234567890
            strncpy(data.str, "", 30);
            vSS = vData; strcpy(vAP_PASSWORD, vSS.c_str());
            vLen = vSS.length();  //-Len(byte-1Byte)
            vSS.toCharArray(vStr, vLen + 1);
    
            addr = 160; EEPROM.put(addr, vLen);
               
            addr = 160 + 1; data.val = vLen; //-Max 30
            strncpy(data.str, vStr, data.val);      
    
            Serial.println("AP_PASSWORD: " + String(vAP_PASSWORD));
            
            EEPROM.put(addr, data);   // replace values in EEPROM          
            EEPROM.commit();    //-寫入EEPROM   
        }        
        else{   //-UDP
            char vCharArray_Buf[inputString.length() + 1];
            inputString.toCharArray(vCharArray_Buf, inputString.length() + 1);
            
            udpWriteCmd(vCharArray_Buf);    //- UART 收到 送到 UDP      
        }
          
        stringComplete = false;        
        inputString = ""; 
    }  
}

//------------------------------------------------------------


#include <WiFi.h>
#include <SimplePortal.h>
//==================================================================
#include <EEPROM.h>
//==================================================================
//==================================================================
#include <PubSubClient.h>
#include <WiFiClient.h>

const char* mqtt_server = "m6.wqtt.ru"; // replace with your broker url
const char* mqtt_username = "eu1abg";  // you don't need the username and password for public connection
const char* mqtt_password = "13051973";
const int mqtt_port = 14516;

WiFiClient espClient;
PubSubClient client(espClient);
//-----------------------------------------------------------------------------------------------------------------
String incommingMessage="";
    const char* RRSI_topic = "p_RRSI";
    const char* osv_topic = "p_osv";
    const char* svet_topic = "p_svet";
    const char* vkl_topic = "p_vkl"; 
    const char* vkl1_topic = "p_vkl1";                             
    const char* led_topic = "p_led";                              
    const char* ugol_topic = "p_ugol";                          
    const char* slider_topic = "p_slider";                             
    const char* indik_topic = "p_indik"; 
// const char* vklpomp_topic = "web_vklpomp";
// const char* tust_topic = "web_tust";  
// const char* shim1_topic = "web_shim1";
   const char* stroka_topic = "p_stroka";
// const char* switch12_topic="web_vkl1";
// const char* ventil_topic="web_ventil";
// //-----------------------------------------------------------------------------------------------------------------
// const char* switch1_topic="web_vkl"; 
// const char* shim2_topic="web_shim2";
// const char* tust1_topic="web_tust1"; 
//const char* edit11_topic="fen_edit11";

//===================================================================================================================
//=========================================================================================================================================== 
 
 #include <TimerMs.h>
 #include <ServoSmooth.h>
ServoSmooth servo(180);
//================================================================================================//=========================================================================================================================================== 
TimerMs tmr1(3000, 1, 0);   // проверка освещенности
TimerMs tmr2;   // таймер свечения
TimerMs tmr5;   // включение
TimerMs tmr7(30000,0,1);   // свечение в ручном режиме
//=========================================================================================================================================== 
int tonePin = 17;
uint32_t sek; //  время свечения
uint32_t sek1;  // время в право
uint32_t sek2; //  время в лево
uint32_t sek3; uint32_t sek4;
float svet = 3900; //3900  темнота полная темнота 1000  полумрак 500  (порг 700 проверено)
float osv; int vkl=0; int vkl1; bool led=0; int ugol; int slider; int indik; String stroka ="";
 int n=0; // повторы поворота 
 int r=0; int i =1; 
uint32_t elapsedTime; uint32_t remainingTime;
uint32_t sec; uint32_t timer1; uint32_t minute; uint32_t second; uint32_t hour; uint32_t now;
int LEDPin = 22;  
int portal=0; uint32_t timerwifi; int RRSI; 


void setup() { 
  
Serial.begin(115200); EEPROM.begin(500); 
servo.attach(21,600, 2400,90); servo.smoothStart(); //EEPROM.read(300)
servo.setDirection(false);
servo.setSpeed(300); servo.setAccel(0.1);
servo.setAutoDetach(true);
//=============================================================================================================================
wifisel();  
//==============================================================================================================================
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
//=============================================================================================================================


//==========================================================================================================================================servo.smoothStart();
pinMode(LEDPin, OUTPUT);    // пин реле лампа
pinMode(16, OUTPUT);   // лампа
pinMode(36, INPUT);   //  освещенность
 pinMode(2, OUTPUT);  //  светодиод режим




 tmr2.setTimerMode();tmr5.setTimerMode();tmr7.setTimerMode();
 if (analogRead(36) > svet) { morg(); delay(3000);    } else {lampON(); delay(3000);lampOFF(); }
 
midi();

}
//=========================================================================================================================================== 


void loop() { 
   servo.tick();
   if (!client.connected()) reconnect(); 
   client.loop(); 
//---------------------------------------------------------------------------------------------
//Serial.print("stroka = "); Serial.println(stroka);


//---------------------------------------------------------------------------------------------
  if (tmr1.tick()) { 
    //osv = random(100,1000);
    vkl1=vkl; RRSI= 0-WiFi.RSSI();
    osv = analogRead(36); 
    //Serial.println(analogRead(36)); //Serial.println(servo.getCurrentDeg());
  //------------------------------------------------------------------------------------------------------------------- 
    digitalWrite(2,HIGH);
      
    publishMessage(RRSI_topic,String(RRSI),true); 
    publishMessage(osv_topic,String(osv),true); 
    publishMessage(svet_topic,String(svet),true);
    publishMessage(vkl1_topic,String(vkl1),true);    
    publishMessage(led_topic,String(led),true);
    publishMessage(ugol_topic,String(ugol),true);
    //publishMessage(slider_topic,String(slider),true);
    publishMessage(indik_topic,String(indik),true);
//  publishMessage(vklpomp_topic,String(vklpomp),true);
//  publishMessage(tust_topic,String(tust),true); 
//  publishMessage(shim1_topic,String(shim1*0.39),true);
    publishMessage(stroka_topic,stroka.c_str(),true);
//  publishMessage(switch12_topic,String(switch1),true);  
//  publishMessage(ventil_topic,String(ventil),true);

 digitalWrite(2,LOW);
 }
 //------------------------------------------------------------------------------------------------------------------- 
   if (tmr7.tick()) lampOFF(); 
  //================================РУЧНОЕ или АВТО============================================================================================
  if  (vkl == 1 ) {  stroka=" Manual !";
   servo.setTargetDeg(slider);  if (servo.tick()) ugol= servo.getCurrentDeg();  //EEPROM.put(300, slider);EEPROM.commit();
   if(servo.tick() != 1) { tmr7.start();lampON();}
} 
       else  {  //Serial.println("DJN");
 //===================================РАБОЧИЙ ЦИКЛ =========================================================================================
  if(osv > svet ) {  digitalWrite(16,HIGH);
    if (tmr2.tick()) {lampOFF();  timerstop(); n = 1; i=1; sek3 = 60000*random(10, 30); tmr5.setTime(sek3);tmr5.start();sek4=sek3/1000;timer1 = millis();}
    if (tmr5.tick()) {  n = 0; timerstop(); stroka = " Begin ! ";   }

 switch (n) {
  case 0: lampON();sek = random(60000, 180000); tmr2.setTime(sek);tmr2.start(); n=4; break;
  //=====================================================================================================================
  
  case 1: elapsedTime = millis() - timer1; remainingTime = (sek4 * 1000 - elapsedTime) / 1000; hour = remainingTime / 3600;remainingTime %= 3600;
          second = remainingTime % 60; minute = remainingTime / 60; stroka = String("Wait ! ") + hour +":"+minute+":"+ second ;  break;  
  case 2:    if (servo.tick()) { ugol= servo.getCurrentDeg(); n = 4; } break;
  case 3:  break;
  case 4:  stroka = "Search simulation";
          sek1 = random(0, 180); stroka = String("Go - ")+sek1+" grad";      ///(" Go - %d grad.", sek1 );
          servo.setTargetDeg(sek1);   n = 2; lampON();  break;
  
  
  //case 5:  servo.tickManual(); stroka = "ЕДЕМ"; n=4; break;   //if(servo.tick() ==  1) {}  else {  n=5;  } if(servo.tick() ==  1) {Serial.print("ждем");}  else  n=5;  
   
   
   }
//===========================Стоп моторы ==========================================================================================itoa (now1, RemoteXY.text_1, 10);
//if (tmr3.tick() or tmr4.tick()) {lampOFF();digitalWrite(D7,LOW); digitalWrite(D5,LOW);delay(100);i=1;n=4;}
//=====================================================================================================================
} 
else {stroka = "Light!"; } 
//==========================Конец авто режима===========================================================================================
} 


if (digitalRead(22)==1) {led=1;  }
 else led=0;

}
//==========================      Фунции    ===========================================================================================
//==============================================================================================================================
void rekonektt() { 
if (WiFi.status() != WL_CONNECTED) { WiFi.disconnect(); WiFi.reconnect(); }
}
//==============================================================================================================================
//==============================================================================================================================
void callback(char* topic, byte* payload, unsigned int length) { String incommingMessage = "";

      for (int i = 0; i < length; i++) incommingMessage+=(char)payload[i];
       Serial.println("Message arrived ["+String(topic)+"]"+incommingMessage);
    
    //  if( strcmp(topic,switch1_topic) == 0) {
    //   if (incommingMessage.equals("0")) { tone(tonePin, 783, 165.441);}
    //   //if (incommingMessage.equals("1")) { tone(tonePin, 783, 165.441);}
    //   //if (incommingMessage.equals("2")) { tone(tonePin, 783, 165.441);}
    //   //if (incommingMessage.equals("3")) { tone(tonePin, 783, 165.441);}
    //   //if (incommingMessage.equals("4")) {tone(tonePin, 783, 165.441);} 
          
    //  }
    //  if( strcmp(topic,vkl_topic) == 0) {
    //    if (incommingMessage.equals("0"))  vkl=0;
    //    if (incommingMessage.equals("333"))  vkl=1;
    //  }

     if (strcmp(topic,vkl_topic) == 0) {vkl= incommingMessage.toInt();  }
     if (strcmp(topic,slider_topic) == 0) { slider= incommingMessage.toInt(); }  ////tust = incommingMessage.toInt();
  Serial.print("___________________");   
  Serial.print("vkl =  "); Serial.println(vkl);
  Serial.print("slider =  "); Serial.println(slider);
  }
//==============================================================================================================================
void publishMessage(const char* topic, String payload , boolean retained){
  if (client.publish(topic, payload.c_str(), true))
      Serial.println("Message publised ["+String(topic)+"]: "+payload);
}
//==============================================================================================================================
void reconnect(){ 
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "EClient-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {Serial.println("connected");
    //client.subscribe(osv_topic); 
    client.subscribe(slider_topic);
    client.subscribe(vkl_topic);
    //subscribe the topics here
      //client.subscribe(command2_topic);   
      } else {
      Serial.print("failed, rc=");Serial.print(client.state());Serial.println(" try again in 5 seconds"); 
      
      } 
  }

}
//==============================================================================================================================


//=====================================================================================================================servo.setTargetDeg(newPos);   
void timerstop(){tmr2.stop();tmr5.stop(); }
//=====================================================================================================================
void lampON()  { digitalWrite(LEDPin,HIGH);}
void lampOFF() { digitalWrite(LEDPin,LOW);digitalWrite(16,LOW); }
//=====================================================================================================================
void morg() { 
digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); delay(50);
digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); delay(50);
digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); delay(50);
digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); 
 delay(200);

digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); delay(50);
digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); delay(50);
digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); delay(50);
digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); 
 delay(200);

digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); delay(50);
digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); delay(50);
digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); delay(50);
digitalWrite(LEDPin,HIGH); delay(30); digitalWrite(LEDPin,LOW); 
  }
//===============================================================================================================
void morg1() {  
 digitalWrite(LEDPin,HIGH); delay(500); digitalWrite(LEDPin,LOW); delay(500);
 digitalWrite(LEDPin,HIGH); delay(500); digitalWrite(LEDPin,LOW); delay(500);
 digitalWrite(LEDPin,HIGH); delay(500); digitalWrite(LEDPin,LOW); delay(500); 
 }
//=====================================================================================================================//===========================================================================================
 void delayFunction(int t) {  now = millis(); while ((millis() - now) < (t+1)) { yield(); }  }       // функция delay yield();ui.updateInt("sw2", valSwitch2); ui.update(); 
 //===========================================================================================
 void wifisel(){
label0:
 if(portal==0){
   EEPROM.get(0, portalCfg.SSID); EEPROM.get(150, portalCfg.pass); WiFi.mode(WIFI_STA); WiFi.begin(portalCfg.SSID, portalCfg.pass);
      Serial.println(portalCfg.SSID); Serial.print(portalCfg.pass);  delay(3000); 
      Serial.println("Подключение.");

  timerwifi = millis(); 
  while (WiFi.status() != WL_CONNECTED) {Serial.print("."); delay(500);
    if((millis()-timerwifi) > 25000) { portal=1; WiFi.disconnect(); 
   Serial.print("  ESP-conf Start !  ");goto label0;} }
    
  }
if (portal==1) {
 
  portalRun(180000); 
   
 

 switch (portalStatus()) { 
   
    

        case SP_SUBMIT: portal=0; EEPROM.put(0,portalCfg.SSID);EEPROM.put(150,portalCfg.pass); EEPROM.commit();
        Serial.println(portalCfg.SSID);Serial.print(portalCfg.pass); delay(3000);
        char SSI[32]; 
  EEPROM.get(0, SSI);Serial.println(SSI);
  EEPROM.get(150, SSI);Serial.print(SSI); delay(3000);                                          goto label0;  break;
        case SP_SWITCH_AP: portal=2;WiFi.mode(WIFI_AP); WiFi.softAP("LabadaSto", "12345678");                 break;  
        case SP_SWITCH_LOCAL: portal=0;                                                                       break;
        case SP_EXIT:  portal=0; goto label0;                                                                 break;
        case SP_TIMEOUT: portal=2; portal=1; WiFi.mode(WIFI_AP); WiFi.softAP("LabadaSto", "12345678");        break;
        case SP_ERROR:   portal=1; goto label0;                                                               break;
 }
}
char SSI[32]; 
  EEPROM.get(0, SSI);Serial.println(SSI);
  EEPROM.get(150, SSI);Serial.print(SSI);  

    
}

 //===========================================================================================
 void midi(){
    tone(tonePin, 783, 165.441);
    delay(183.823333333);
    tone(tonePin, 659, 165.441);
    delay(183.823333333);
    tone(tonePin, 880, 165.441);
    delay(183.823333333);
    tone(tonePin, 659, 165.441);
    delay(183.823333333);
    tone(tonePin, 739, 165.441);
    delay(183.823333333);
    tone(tonePin, 783, 165.441);
    delay(183.823333333);
    tone(tonePin, 659, 165.441);
    delay(183.823333333);
    tone(tonePin, 987, 165.441);
    delay(183.823333333);
    tone(tonePin, 880, 165.441);
    delay(183.823333333);
    tone(tonePin, 783, 165.441);
    delay(183.823333333);
    tone(tonePin, 739, 165.441);
    delay(183.823333333);
    tone(tonePin, 659, 165.441);
    delay(183.823333333);
    tone(tonePin, 587, 165.441);
    delay(183.823333333);
    tone(tonePin, 783, 165.441);
    delay(183.823333333);
    tone(tonePin, 659, 165.441);
    delay(183.823333333);
    tone(tonePin, 880, 165.441);
    delay(183.823333333);
    tone(tonePin, 659, 165.441);
    delay(183.823333333);
    tone(tonePin, 739, 165.441);
    delay(183.823333333);
    tone(tonePin, 783, 165.441);
    delay(183.823333333);
    tone(tonePin, 659, 165.441);
    delay(183.823333333);
    tone(tonePin, 987, 165.441);
    delay(183.823333333);
    tone(tonePin, 880, 165.441);
    delay(183.823333333);
    tone(tonePin, 783, 165.441);
    delay(183.823333333);
    tone(tonePin, 739, 165.441);
    delay(183.823333333);
    tone(tonePin, 659, 165.441);
    delay(183.823333333);
    tone(tonePin, 587, 165.441);
}




#include <WiFi.h>
#include <SimplePortal.h>
//==================================================================
#include <EEPROM.h>
#include <AutoOTA.h>
AutoOTA ota("2.1", "eu1abg/Progektor_Servo_VIRTUINOO"); // eu1abg/Webasto_virtuino   https://github.com/GyverLibs/AutoOTA
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
// String incommingMessage="";
//     const char* RRSI_topic = "p_RRSI";
//     const char* osv_topic = "p_osv";
//     const char* svet_topic = "p_svet";
//     const char* vkl_topic = "p_vkl"; 
//     const char* vkl1_topic = "p_vkl1";                             
//     const char* led_topic = "p_led";                              
//     const char* ugol_topic = "p_ugol";                          
//     const char* slider_topic = "p_slider";                             
//     const char* indik_topic = "p_indik"; 
//     const char* vklmic_topic = "web_vklmic";
// const char* tust_topic = "web_tust";  
// const char* shim1_topic = "web_shim1";
//   const char* stroka_topic = "p_stroka";
// const char* switch12_topic="web_vkl1";
// const char* ventil_topic="web_ventil";
// //-----------------------------------------------------------------------------------------------------------------
// const char* switch1_topic="web_vkl"; 
// const char* shim2_topic="web_shim2";
// const char* tust1_topic="web_tust1"; 
//const char* edit11_topic="fen_edit11";

//===================================================================================================================

#include <GyverPortal.h>
GyverPortal ui;

//=========================================================================================================================================== 
 
 #include <TimerMs.h>
//  #include <ServoSmooth.h>
//  ServoSmooth servo(180);

//================================================================================================
#include <ESP32Servo.h>
Servo servo; 
//===================================================================================================================
#define tsv 30      //  время свечения прожектора в мануале в секундах 
#define skorost 10  //  скорость вращения сервы чем больше тем медленнее
#define svet 3900  //  3900  темнота полная темнота 1000  полумрак 500  (порг 700 проверено)
#define port 180000  // время работы ЕСП Конфиг 180000
#define TIME_reconekt 10  //  минты время реконекта к интернет и переход на точку доступа
#define TIME_reconekt_WIFI 5  //  минуты время реконекта к если нет вайфай
//===================================================================
//=========================================================================================================================================== 
TimerMs tmr1(3000, 1, 0);   // проверка освещенности
TimerMs tmr2;   // таймер свечения
TimerMs tmr3(21000, 1, 0);   // переподключение интернет и вайфай
TimerMs tmr5;   // включение
TimerMs tmr7;   // свечение в ручном режиме
TimerMs tmr8(60000, 1, 0);   // обновление
TimerMs tmr9(1000, 1, 0);   // пауза после вращения
TimerMs tmr10(1000, 1, 0);   // задержка вкл прожектора в мануале 
TimerMs tmr11;   // 
TimerMs tmr12;   // 
//=========================================================================================================================================== 
//===========================================================================================
 bool ch1,ch2,ch3,ch4,ch5,ch6,ch7,ef1,ef2,ef3,ef4,ef5,ef6,ef7;
 bool btn=0;    int sld; int on;int on1; bool tim=0; bool tim1=0;
 int hour1, minute1, second1; uint32_t pr; uint32_t now1; uint32_t t =0; 
 GPtime valTime; GPtime valTime1; GPtime valTime2; 
 GPdate valDate;
 String timeON="";String timeOFF=""; String timeREAL=""; int v=0; String aip="";
//===========================================================================================
int tonePin = 17;
uint32_t sek; //  время свечения
int sek1;  int sek1OLD; // время в право
uint32_t sek2; //  время в лево
uint32_t sek3; uint32_t sek4; bool str=0; //   вкл страницы
float osv; bool vkl=0; bool vkl1; bool led=0;  int ugol; int slider; int sliderOLD;
 int indik; String stroka ="";
 int n=0; // повторы поворота 
 int r=0; int i =1; bool l1=0; bool raz=1; int pos;
uint32_t elapsedTime; uint32_t remainingTime;
uint32_t sec; uint32_t timer1; uint32_t minute; uint32_t second; uint32_t hour; uint32_t now;
int LEDPin = 22;  
int portal=0; uint32_t timerwifi; int RRSI;  bool mic=1; bool vklmic; bool manual; bool micled;
//--------------------------------ТОПИКИ и АйДи -----------------------------------------
String incommingMessage="";     uint32_t chipId = 0; 
String RRSI_top;                const char* RRSI_topic; 
String osv_top;                 const char* osv_topic; 
String svet_top;                const char* svet_topic;                        
String vkl_top;                 const char* vkl_topic;                         
String vkl1_top;                const char* vkl1_topic;                    
String led_top;                 const char* led_topic;                      
String ugol_top;                const char* ugol_topic;                     
String slider_top;              const char* slider_topic;
String indik_top;               const char* indik_topic;
String vklmic_top;              const char* vklmic_topic; 
String micled_top;             const char* micled_topic;
String stroka_top;             const char* stroka_topic;
String manual_top;             const char* manual_topic;


void preSetupChipId() {
  uint64_t mac = ESP.getEfuseMac();
  chipId = (uint32_t)(mac >> 24);  // Берем 4 байта MAC-адреса

 RRSI_top = String(chipId)+"/"+"RRSI";               RRSI_topic= RRSI_top.c_str();  
 osv_top = String(chipId)+"/"+ "osv";                osv_topic= osv_top.c_str();
 svet_top = String(chipId)+"/"+  "svet";             svet_topic= svet_top.c_str();                       
 vkl_top = String(chipId)+"/"+ "vkl";                vkl_topic= vkl_top.c_str();                      
 vkl1_top = String(chipId)+"/"+ "vkl1";              vkl1_topic= vkl1_top.c_str();                  
 led_top = String(chipId)+"/"+ "led";                led_topic=led_top.c_str();                   
 ugol_top = String(chipId)+"/"+   "ugol";            ugol_topic=  ugol_top.c_str();                 
 slider_top = String(chipId)+"/"+ "slider";          slider_topic= slider_top.c_str();
 indik_top = String(chipId)+"/"+  "indik";           indik_topic= indik_top.c_str();
 vklmic_top = String(chipId)+"/"+ "vklmic";          vklmic_topic= vklmic_top.c_str();
 stroka_top = String(chipId)+"/"+ "stroka";          stroka_topic= stroka_top.c_str();
 micled_top = String(chipId)+"/"+ "micled";          micled_topic= micled_top.c_str();
 manual_top = String(chipId)+"/"+ "manual";          manual_topic= manual_top.c_str();

}
    


//==============================================================================================================================
 void __attribute__((constructor)) beforeSetup() {preSetupChipId();}
//========================================================================================
void setup() { 
  
Serial.begin(115200); EEPROM.begin(500); 
//=====================================================================================================
// servo.attach(21,600, 2400,90); servo.smoothStart(); //EEPROM.read(300)
// servo.setDirection(false);
// servo.setSpeed(300); servo.setAccel(0.1);
// servo.setAutoDetach(false);
//=============================================================================================================================
  ESP32PWM::allocateTimer(2);
	servo.setPeriodHertz(50);    // standard 50 hz servo
	servo.attach(21,400, 3100); 
  

//=============================================================================================================================
pinMode(LEDPin, OUTPUT);    // пин реле ПРОЖЕКТОР
pinMode(16, OUTPUT);   // лампочка освещения
pinMode(36, INPUT);   //  освещенность
 pinMode(2, OUTPUT);  //  светодиод режим
pinMode(39,INPUT);   // мик
//=============================================================================================================================

wifisel();  
//==============================================================================================================================
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
//=============================================================================================================================


 tmr2.setTimerMode();tmr5.setTimerMode();tmr7.setTimerMode();
 if (analogRead(36) > svet) { morg(); delay(3000);    } else {lampON(); delay(3000);lampOFF(); }
 
midi();
//sportlotoMelody();
//============================================================================================================================================   
  
  //GPtime(hour1, minute1, second1); GPdate();
  //GPtime(int hour, int minute, int second);   // из трёх чисел
  //GPtime(String str);                         // из строки вида hh:mm:ss
//============================================================================================================================================ 
   ui.attachBuild(build); ui.attach(action); ui.start(); //ui.log.start(500);
//=========================================================================================================================================
}
//=========================================================================================================================================== 


void loop() { 
//-------------------------------- Страница ---------------------------------------------------------------
if(str==1) { ui.tick();  }
//-------------------------------------------------------------------------------------------
if(portal!=2){if (!client.connected()) reconnect(); client.loop(); }
//-------------------------------------------------------------------------------------------
   if(digitalRead(39)==0) {micled = 1; }
   
//---------------------------------------------------------------------------------------------mic=0;
  if(vklmic==1 && mic==1)  { if(digitalRead(39)==0) {  n=0; }    }
//Serial.print("mic = "); Serial.println(mic);
//---------------------------------------------------------------------------------------------
   //if(raz==1) {osv = analogRead(36); }
    

if (tmr1.tick()) { 
  Serial.println(stroka);
   Serial.print("MIC = ");Serial.println(digitalRead(39));
   Serial.print("micled = ");Serial.println(micled);
   Serial.print("slier = ");Serial.print(slider); 
   Serial.print("       slierOLD = ");Serial.println(sliderOLD);
  Serial.print(" ugol= ");Serial.println(ugol);
  vkl1=vkl; RRSI= 0-WiFi.RSSI();
    if(l1==0) osv = analogRead(36); // если вкл фара то освещенность не мереется
    Serial.print("osv = ");Serial.println(analogRead(36)); //Serial.println(servo.getCurrentDeg());
     
  //------------------------------------------------------------------------------------------------------------------- 
 if(portal!=2){
  digitalWrite(2,HIGH);
    publishMessage(micled_topic,String(micled),true); 
    publishMessage(RRSI_topic,String(RRSI),true); 
    publishMessage(osv_topic,String(osv),true); 
    publishMessage(svet_topic,String(svet),true);
    publishMessage(vkl1_topic,String(vkl1),true);    
    publishMessage(led_topic,String(led),true);
    publishMessage(ugol_topic,String(ugol),true);
    publishMessage(vklmic_topic,String(vklmic),true);
    publishMessage(indik_topic,String(indik),true);
    publishMessage(stroka_topic,stroka.c_str(),true);
    publishMessage(manual_topic,String(manual),true);
    digitalWrite(2,LOW); 
    } 
  micled = 0; 
 }
 //------------------------------------------------------------------------------------------------------------------- 
   if (tmr7.tick()) {lampOFF(); servo.detach(); } //servo.detach(); 
  //================================РУЧНОЕ или АВТО============================================================================================
  if  (vkl == 1 ) {  stroka=String("Ver. ")+ota.version()+" Manual !"; manual=1; mic = 1; digitalWrite(16,LOW); //servo.attach(21,600, 2400); 
   ugol=servo.read(); //Serial.print("ugol=");Serial.println(servo.read());

    if(raz==1) lampON();
    

   if( abs(slider-ugol) < 5 && raz==1) { if (tmr10.tick()) { lampON();  sliderOLD=slider;  tmr7.setTime(tsv*1000); tmr7.start(); raz=0;} }
   if(abs(sliderOLD - slider) > 5) {  raz=1;  servo.attach(21,400, 3100);   //servo.write(slider);servo.attach(21,400, 3100);
     serv(sliderOLD, slider );
     //if(sliderOLD < slider) { for ( pos = sliderOLD; pos <= slider; pos += 1) {servo.write(pos);delay(10);Serial.print(pos); } }   // Чем больше delay, тем медленнее поворотservo.write(slider); 
     //if(sliderOLD > slider) { for ( pos = sliderOLD; pos >= slider; pos -= 1) {servo.write(pos);delay(10);Serial.print(pos); } }  
   
   }
   // if (servo.tick()) ugol= servo.getCurrentDeg();  //EEPROM.put(300, slider);EEPROM.commit();servo.setPeriodHertz(50); servo.attach(21,600, 2400);
   //if(servo.tick() != 1) { tmr7.start();lampON();} 

   
} 
       else  {  manual=0;
 //===================================РАБОЧИЙ ЦИКЛ =========================================================================================
  if(osv > svet ) {  if(l1==0) {digitalWrite(16,HIGH);} else digitalWrite(16,LOW);
    if (tmr2.tick()) {lampOFF();  timerstop(); servo.detach(); raz=1; micled==0;mic=1; n = 1; i=1; sek3 = 60000*random(10, 30); tmr5.setTime(sek3);tmr5.start();sek4=sek3/1000;timer1 = millis();}
    if (tmr5.tick()) {  n = 0; timerstop(); stroka = " Begin ! ";   }

 switch (n) {
  case 0: lampON(); servo.attach(21,400, 3100); sek = random(60000, 180000); tmr2.setTime(sek);tmr2.start(); n=4; mic=0; raz=0; break;
  //=====================================================================================================================
  
  case 1: elapsedTime = millis() - timer1; remainingTime = (sek4 * 1000 - elapsedTime) / 1000; hour = remainingTime / 3600;remainingTime %= 3600;
          second = remainingTime % 60; minute = remainingTime / 60; stroka = String("Wait ! ") + hour +":"+minute+":"+ second ;  break;  
  case 2:   if (tmr9.tick()) {if (abs(sek1-servo.read()) < 5) { ugol= servo.read(); n = 4; }} break;
  case 3:  break;
  case 4:  stroka = "Search simulation"; 
          sek1 = random(0, 180); stroka = String("Time-")+(tmr2.timeLeft()/1000)+" Сек."+" Go--"+sek1+" grad";      //tmr9.setTime(200);tmr9.start();
          //servo.setTargetDeg(sek1); 
          //servo.write(sek1); 
         serv(sek1OLD, sek1);  sek1OLD=sek1; n = 2; lampON();   break; //  raz  разрешение на проверку освещенности
  
  
  //case 5:  servo.tickManual(); stroka = "ЕДЕМ"; n=4; break;   //if(servo.tick() ==  1) {}  else {  n=5;  } if(servo.tick() ==  1) {Serial.print("ждем");}  else  n=5;  
   
   
   }
//===========================Стоп моторы ==========================================================================================itoa (now1, RemoteXY.text_1, 10);
//if (tmr3.tick() or tmr4.tick()) {lampOFF();digitalWrite(D7,LOW); digitalWrite(D5,LOW);delay(100);i=1;n=4;}
//=====================================================================================================================
} 
else {stroka =String("Ver. ")+ota.version()+" Light!";lampOFF(); } //servo.stop();
//==========================Конец авто режима===========================================================================================
} 


if (digitalRead(22)==1) {led=1;  }
 else led=0;
//------------------------------ Обновление и переподключение -----------------------------------------------------------------
if(portal!=2){ if(tmr8.tick())  obnovl();}
if (tmr3.tick()  && portal !=2  ) rekonektt();
if (tmr11.tick() && portal !=2 ) { Serial.print("11");lampON(); WiFi.disconnect(); sportlotoMelody(); WiFi.mode(WIFI_AP); WiFi.softAP("Progektor_Virtuino", "12345678"); 
portal=2; str=1; }
if (tmr12.tick() && portal !=2)  { Serial.print("12");EEPROM.put(0,0);EEPROM.put(50,0); EEPROM.commit(); lampON(); sportlotoMelody();lampOFF(); ESP.restart();}
//==============================================================================================

}
//==========================      Фунции    ===========================================================================================
void serv(int x, int y ) { 
     if(x < y) { for ( pos = x; pos <= y; pos += 1) {servo.write(pos);delay(skorost);} }  
     if(x > y) { for ( pos = x; pos >= y; pos -= 1) {servo.write(pos);delay(skorost);} } 

}

//==============================================================================================================================
void rekonektt() { 
  if (WiFi.status() != WL_CONNECTED) { 
    if(tim1==0) {lampON();lampOFF();lampON();lampOFF();lampON();
     tmr12.setTime(TIME_reconekt_WIFI*60000);tmr12.start(); tim1=1;lampOFF();} Serial.println("WiFi.reconnect");WiFi.disconnect(); WiFi.reconnect(); }
     else { tim1=0; tmr12.stop();}

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
      if( strcmp(topic,vkl_topic) == 0) { if (incommingMessage.equals("0"))  vkl=0; if (incommingMessage.equals("333"))  vkl=1;}
      if( strcmp(topic,vklmic_topic) == 0) {if (incommingMessage.equals("0"))  {vklmic=0;} if (incommingMessage.equals("333"))  vklmic=1;} 

     //if (strcmp(topic,vklmic_topic) == 0) {vklmic= incommingMessage.toInt();  }
     //if (strcmp(topic,vkl_topic) == 0) {vkl= incommingMessage.toInt();  }
     if (strcmp(topic,slider_topic) == 0) { slider= incommingMessage.toInt(); }  ////tust = incommingMessage.toInt();
  //  Serial.println("___________________");   
  //  Serial.print("vkl =  "); Serial.println(vkl);
  //  Serial.print("vklmic =  "); Serial.println(vklmic);
  //  Serial.print("slider =  "); Serial.println(slider);
  }
//==============================================================================================================================
void publishMessage(const char* topic, String payload , boolean retained){
  if (client.publish(topic, payload.c_str(), true))
      Serial.println("Message publised ["+String(topic)+"]: "+payload);
}
//==============================================================================================================================
void reconnect(){ 
  if (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
Serial.print("tim=");Serial.println(tim); if(tim==0) {lampON(); tmr11.setTime(TIME_reconekt*60000);tmr11.start(); tim=1;lampOFF();}
    String clientId = "EClient-";   // Create a random client ID
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {Serial.println("connected"); tim=0; tmr11.stop();
    //client.subscribe(osv_topic); 
    client.subscribe(slider_topic);
    client.subscribe(vkl_topic);
    client.subscribe(vklmic_topic);
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
void lampON()  { digitalWrite(LEDPin,HIGH);l1=1;}
void lampOFF() { digitalWrite(LEDPin,LOW);l1=0;digitalWrite(16,LOW); }
//===========================================================================================
 void delayFunction(int t) {  now = millis(); while ((millis() - now) < (t+1)) { yield(); }  }       // функция delay yield();ui.updateInt("sw2", valSwitch2); ui.update(); 
 //===========================================================================================
 void wifisel(){
label0:
 if(portal==0){
   EEPROM.get(0, portalCfg.SSID); EEPROM.get(150, portalCfg.pass); WiFi.mode(WIFI_STA); WiFi.begin(portalCfg.SSID, portalCfg.pass);
      Serial.println(portalCfg.SSID); Serial.print(portalCfg.pass);  delay(3000); 
      Serial.println("Подключение.");

  timerwifi = millis(); 
  while (WiFi.status() != WL_CONNECTED) {digitalWrite(16,HIGH); Serial.print("."); delay(500); digitalWrite(16,LOW); delay(500);
    if((millis()-timerwifi) > 25000) { portal=1; WiFi.disconnect(); 
   Serial.print("  ESP-conf Start !  ");goto label0;} }
    
  }
if (portal==1) {
 
  portalRun(port); //180000
   
 

 switch (portalStatus()) { 
   
    

        case SP_SUBMIT: portal=0; EEPROM.put(0,portalCfg.SSID);EEPROM.put(150,portalCfg.pass); EEPROM.commit();
        Serial.println(portalCfg.SSID);Serial.print(portalCfg.pass); delay(3000); str=0; 
        char SSI[32]; 
  EEPROM.get(0, SSI);Serial.println(SSI);
  EEPROM.get(150, SSI);Serial.print(SSI); delay(3000);                                          goto label0;    break;
        case SP_SWITCH_AP: portal=2;WiFi.mode(WIFI_AP); WiFi.softAP("Progektor_Virtuino", "12345678");  str=1;  break;  
        case SP_SWITCH_LOCAL: portal=0;                                                                         break;
        case SP_EXIT:  portal=0; goto label0;                                                                   break;
        case SP_TIMEOUT: portal=2; WiFi.mode(WIFI_AP); WiFi.softAP("Progektor_Virtuino", "12345678");    str=1; break;
        case SP_ERROR:   portal=2; goto label0;                                                                 break;
 }
}
char SSI[32]; 
  EEPROM.get(0, SSI);Serial.println(SSI);
  EEPROM.get(150, SSI);Serial.println(SSI); 
  Serial.print("portalStatus() = ");Serial.println(portalStatus());  
  Serial.print("Portal = ");Serial.println(portal); 
    
}
//===========================================================================================
//==============================================================================================================================
 void obnovl() { String ver, notes;
if (ota.checkUpdate(&ver, &notes)) { stroka =String("Ver. ")+ver+" notes!"+notes;tone(tonePin, 783, 5000);ota.updateNow();}
}
 
//===========================================ФУНКЦИИ==============================================================================================   
void build() {
  GP.BUILD_BEGIN(GP_DARK,800); 
 //.................................................................................................................................
  GP.UPDATE("str,time,date,ch1,ch2,ch3,ch4,ch4,ch5,ch6,ch7,time1,time2,led,led1,led2,sw3,ef1,ef2,ef3,ef4,ef5,ef6,ef7,slider",500);
  //GP.UPDATE("test",5000);
  //GP.UPDATE("sw1,led",500);
  //GP.RELOAD_CLICK("sw1,sel1,sel2");
  //GP.RELOAD("test");
  //.................................................................................................................................
//  GP.SPOILER_BEGIN("LampaLito v1.00.",GP_BLUE_B);
//  GP.LABEL("Лито лампа ", "t11", GP_ORANGE_B, 15); GP.BREAK();
//  GP.LABEL("**Produced by Labada Studio** ", "t2", GP_ORANGE_B, 15,0,0); GP.BREAK();
//  GP.LABEL("EPS-Minsk.by", "t3", GP_ORANGE_B, 20);GP.SPOILER_END();
 //................................................................................................................................. 
GP.BLOCK_BEGIN(GP_THIN,"100%","Строка состояния", GP_GREEN );
       M_BOX(GP_CENTER,GP.AREA("str",2,stroka););
  // M_BOX(GP_CENTER, GP.TEXT("date","",stroka,"100%"); );  //GP.TEXT(имя, подсказка, текст, ширина); 
   GP.BLOCK_END();
 //................................................................................................................................. 
  
GP.GRID_RESPONSIVE(700);M_GRID( 
  GP.BLOCK_BEGIN(GP_TAB,"100%","Режим работы.", GP_YELLOW);
 M_BOX(GP.LABEL("Ручное упр."); GP.LED("led", manual, GP_ORANGE_B);  ); 
 M_BOX(GP.LABEL("Мик.");        GP.LED("led1", micled, GP_PINK_B);  );
 
 M_BOX(GP_CENTER,GP.LABEL("Прожектор"); GP.LED("led2", l1, GP_YELLOW_B); );
 GP.BLOCK_END(); 
 //................................................................................................................................. 
GP.BLOCK_BEGIN(GP_TAB,"100%","Выбрать режим работы.", GP_YELLOW);
M_BOX(GP.LABEL("Ручное упр.");GP.CHECK("ch1","",GP_ORANGE_B); GP.LABEL("Акт.мик.");GP.CHECK("ch2","",GP_ORANGE_B);
//GP.LABEL("Ср.");GP.CHECK("ch3","",GP_ORANGE_B);GP.LABEL("Чтв.");GP.CHECK("ch4","",GP_ORANGE_B);
);
//M_BOX(GP.LABEL("Птн.");GP.CHECK("ch5","",GP_ORANGE_B);GP.LABEL("Суб.");GP.CHECK("ch6","",GP_ORANGE_B);GP.LABEL("Вос.");GP.CHECK("ch7","",GP_ORANGE_B); );
GP.BLOCK_END();

// GP.BLOCK_BEGIN(GP_TAB,"100%","Выбрать эффекты.", GP_YELLOW);
// M_BOX(GP.LABEL("Sve");GP.CHECK("ef1","",GP_ORANGE_B); GP.LABEL("Fir12");GP.CHECK("ef2","",GP_ORANGE_B);GP.LABEL("FirEf");GP.CHECK("ef3","",GP_ORANGE_B);
 
// GP.LABEL("Rain");GP.CHECK("ef4","",GP_ORANGE_B););
//  M_BOX(GP.LABEL("Smena");GP.CHECK("ef5","",GP_ORANGE_B);GP.LABEL("Beg");GP.CHECK("ef6","",GP_ORANGE_B);GP.LABEL("Radu");GP.CHECK("ef7","",GP_ORANGE_B); );
// GP.BLOCK_END();
);
GP.BLOCK_BEGIN(GP_THIN,"100%","АЗИМУТ ", GP_ORANGE );
   GP.LABEL("Угол");GP.NUMBER_F("ch3", "С.", ugol, 0,"100px");
   GP.LABEL("Направление освещения."); GP.SLIDER_C("slider", slider, 0, 180, 1, 0, GP_GREEN,1);
   GP.BLOCK_END();

// GP.BLOCK_BEGIN(GP_THIN,"100%","Управление.", GP_GREEN );
//    GP.LABEL("Вкл./Выкл.");GP.SWITCH("sw3", on, GP_VIOL_B);
//    GP.BLOCK_END();
//................................................................................................................................. 
  M_BOX(GP.BUTTON("sw1", "Перезагрузка!", "", GP_BLUE_B, "", 0, 1); GP.BUTTON("sw2", "Сбр.наст. WI-FI!", "", GP_ORANGE_B, "", 0, 1); );


  
 



  GP.BUILD_END();
}
//========================================================================================================================================= 
void action() {
  //======================= Отправка данных ====================================================================
 if (ui.update()) { 
     ui.updateBool("ch1", vkl);
     ui.updateBool("ch2", vklmic);
     ui.updateInt("ch3", ugol);
    //  ui.updateBool("ch3", EEPROM.read(102));
    ui.updateInt("slider", slider);
    ui.updateString("str", stroka);
    ui.updateBool("led", manual);
    ui.updateBool("led1", micled);
    ui.updateBool("led2", l1);
   
}
  //============================= Прием данных ============================================================== 
   if (ui.click("sw1")) { delay(3000);ESP.restart();}
   if (ui.click("sw2")) { EEPROM.put(0,0);EEPROM.put(50,0); EEPROM.commit();Serial.println("Сброс WIFI ");delay(1000);ESP.restart(); }
   if (ui.click("ch1")) { vkl= ui.getBool("ch1");  } 
   if (ui.click("ch2")) { vklmic= ui.getBool("ch2");  } 
   //if (ui.click("ch3")) { ch3= ui.getBool("ch3"); EEPROM.put(102,ch3); } 
  //  if (ui.click("ch3")) { ch3= ui.getBool("ch3"); EEPROM.put(102,ch3); } raz=1;
   if (ui.click("slider")) { slider = ui.getInt("slider");  }
   //Serial.print(" slider-- "); Serial.println(sld);
  
} 
//========================================================================================================================================= 
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
//=================================================================================================
void sportlotoMelody() {
  tone(tonePin, 659, 200); delay(250);  // E5
  tone(tonePin, 659, 200); delay(250);  // E5
  tone(tonePin, 659, 200); delay(250);  // E5
  tone(tonePin, 523, 200); delay(200);  // C5
  tone(tonePin, 659, 200); delay(250);  // E5
  tone(tonePin, 784, 400); delay(450);  // G5
  delay(200);

  tone(tonePin, 392, 400); delay(500);  // G4
  delay(100);

  tone(tonePin, 523, 200); delay(250);  // C5
  tone(tonePin, 392, 200); delay(250);  // G4
  tone(tonePin, 330, 200); delay(250);  // E4

  tone(tonePin, 440, 200); delay(250);  // A4
  tone(tonePin, 494, 200); delay(250);  // B4
  tone(tonePin, 466, 200); delay(250);  // A#4
  tone(tonePin, 440, 200); delay(250);  // A4

  tone(tonePin, 392, 200); delay(250);  // G4
  tone(tonePin, 659, 200); delay(250);  // E5
  tone(tonePin, 784, 400); delay(500);  // G5
}

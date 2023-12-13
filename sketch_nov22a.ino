#include <Buzzer.h>
#include <RBD_LightSensor.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "AsyncTaskLib.h"


void light(void);
AsyncTask asyncTask(1000, true, light);

#define WIFI_SSID "TP-LINK_DE16" // Wifi network station credentials
#define WIFI_PASSWORD "67743667"
#define BOT_TOKEN "6772428240:AAEszAeE7JJgfWBbs5q3s6obHchbo34hIww"
#define ledPin 4
#define buzzerPin 13

  const unsigned long BOT_MTBS = 1000; // mean time between scan messages
  WiFiClientSecure secured_client;
  UniversalTelegramBot bot(BOT_TOKEN, secured_client);
  unsigned long bot_lasttime; // last time messages' scan has been done
  RBD::LightSensor light_sensor(34);

//const int ledPin = 4;
//const int buzzerPin = 13;

  int sensorValue = 50;
  int ledStatus = 0;
  int reps = 1;
  float percentage=0.0;

void handleNewMessages(int numNewMessages){

    Serial.print("handleNewMessages ");
    Serial.println(numNewMessages); 
  for (int i = 0; i < numNewMessages; i++){

    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";
    if (text == "/ledon"){
      
      digitalWrite(ledPin, HIGH); // turn the LED on (HIGH is the voltage level)
      ledStatus = 1;
      bot.sendMessage(chat_id, "el niño esta durmiendo", "");
      loop2();
    }
    if (text == "/ledoff"){

    digitalWrite(ledPin, LOW); // turn the LED on (HIGH is the voltage level)
    ledStatus = 0;
    bot.sendMessage(chat_id, "el niño esta despierto", "");
    delay(5000);
    }
    if (text == "/monitoreo"){
      
      percentage = light_sensor.getPercentValue();
      char percentageStr[10]; // Ajusta el tamaño según tus necesidades
      dtostrf(percentage, 4, 2, percentageStr); // 4 = ancho total, 2 = decimales   

      if (percentage < 200){
        bot.sendMessage(chat_id, "La luz nocturna esta apagada", "");
        bot.sendMessage(chat_id, "La habitación está iluminada", "");
        bot.sendMessage(chat_id, "El valor del sensor es: " + String(percentageStr),"");
      }else{
        bot.sendMessage(chat_id, "La luz nocturna esta encendida", "");
        bot.sendMessage(chat_id, "La habitación está oscura", "");
        bot.sendMessage(chat_id, "El valor del sensor es: " + String(percentageStr),"");
      }
           
    }
    if (text == "/start"){

      String welcome = "proyecto cuna inteligente, " + from_name + ".\n";
      welcome += "/ledon : encender luz nocturna\n";
      welcome += "/ledoff : apagar luz nocturna\n";
      welcome += "/monitoreo : monitorear nivel de luz\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup(){

  pinMode(buzzerPin, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  pinMode(ledPin, OUTPUT); // initialize digital ledPin as an output.
  delay(10);
  digitalWrite(ledPin, LOW); // initialize pin as off (active LOW)
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  while (WiFi.status() != WL_CONNECTED){

    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);

  while (now < 24 * 3600){

    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
  asyncTask.Start();
}

void loop(){

  asyncTask.Update();
  if (millis() - bot_lasttime > BOT_MTBS){

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages){

      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }
}

//BUZZER MELODIA:
// Definir las notas y sus frecuencias
int melody[] = {NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, 0,
                NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4, 0,
                NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, 0,
                NOTE_G4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, 0,
                NOTE_C4, NOTE_C4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, 0,
                NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4, 0};

// Definir la duración de cada nota
int noteDurations[] = {4, 4, 4, 4, 4, 4, 2, 4,
                       4, 4, 4, 4, 4, 4, 2, 4,
                       4, 4, 4, 4, 4, 4, 2, 4,
                       4, 4, 4, 4, 4, 4, 2, 4,
                       4, 4, 4, 4, 4, 4, 2, 4,
                       4, 4, 4, 4, 4, 4, 2, 4};

void play () {
  // Reproducir la melodía
  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {

      int duration = 1000 / noteDurations[i];
      tone(buzzerPin, melody[i], duration); 
      int pause = duration * 1.1; // Pausa entre notas
      delay(pause);
      noTone(buzzerPin); // Detener el sonido
    }
}

void loop2(){
  if (reps==1){
      for (int k = 0; k < 3; k++ ){
        play();
        delay(500);
      }
  }
  reps = 0;
}

void light(void){

    int lightPercentage = light_sensor.getPercentValue();
    Serial.print(", Light_Sensor: ");
    Serial.println(lightPercentage);
  
    if (lightPercentage > 200) {
        Serial.println("el niño esta durmiento");
        digitalWrite(ledPin, HIGH);
        play();

    } else {
        Serial.print("el niño esta despierto");
        digitalWrite(ledPin, LOW);
    }
        digitalWrite(ledPin, LOW);
}

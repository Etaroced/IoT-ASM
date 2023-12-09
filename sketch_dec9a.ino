#define BLYNK_TEMPLATE_ID "TMPL6hhoBumur"
#define BLYNK_TEMPLATE_NAME "Assignment"
#define BLYNK_AUTH_TOKEN "gUyOLLuvvtpVddcL_AmFfSlfSxj5Dfx-"

#include <SPI.h>
#include <MFRC522.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <TimeLib.h>
#include <WidgetRTC.h>  // Include the WidgetRTC library for RTC widget
#include <Servo.h> 

WiFiClient client;
BlynkTimer timer;

#define SS_PIN D4
#define RST_PIN D3

#define LR_LED D1
#define BR_LED D2

int LR = 0;
int BR = 0;

MFRC522 mfrc522(SS_PIN, RST_PIN);

char auth[] = BLYNK_AUTH_TOKEN;
const char* ssid = "Redmi K50"; 
const char* password = "03052002"; 

int fflag = 0;

Servo myServo;

WidgetRTC rtc;  // Create an instance of the WidgetRTC
WidgetTerminal terminal(V2);

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, password);

  pinMode(LR_LED, OUTPUT);
  pinMode(BR_LED, OUTPUT);

  SPI.begin();
  myServo.attach(D0);
  myServo.write(0);

  terminal.clear();
  Blynk.virtualWrite(V2, "System is ready.");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  mfrc522.PCD_Init();
  Serial.println("Put your card to the reader...");
  Serial.println();

  timer.setInterval(1000L, iot_rfid);

  // Initialize the RTC widget
  rtc.begin();
}

void loop() {
  Blynk.run();
  timer.run();
}

void iot_rfid() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID tag :");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();

  if ((content.substring(1) == "23 57 ED 0F") && (fflag == 0)) {
    Serial.println("Authorized access");
    Blynk.virtualWrite(V2, "USER, Authorized access");
    Blynk.virtualWrite(V2, "Current Time: ");
    Blynk.virtualWrite(V2, String(hour()) + ":" + minute() + ":" + second());
    Serial.println();
    delay(500);
    myServo.write(180);
    delay(4000);
    myServo.write(0);
  } else {
    Serial.println("Access denied");
    Blynk.virtualWrite(V2, "Id card is incorrect, access denied");
    Blynk.virtualWrite(V2, "Current Time: ");
    Blynk.virtualWrite(V2, String(hour()) + ":" + minute() + ":" + second());
  }
}

BLYNK_WRITE(V1) {
  fflag = param.asInt();
}

BLYNK_WRITE(V3) {
  int value = param.asInt();
  digitalWrite(LR_LED, value);
}

BLYNK_WRITE(V4) {
  int value = param.asInt();
  digitalWrite(BR_LED, value);
}

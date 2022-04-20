#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <WiFiClientSecure.h>
#include <PZEM004Tv30.h>

#define FIREBASE_HOST "https://phase-72507-default-rtdb.asia-southeast1.firebasedatabase.app/" //--> URL address of your Firebase Realtime Database.
#define FIREBASE_AUTH "bJiFvYccw6RKJDL5sS7gjpP8VvfZy6KvKUQjiMvP" //--> Your firebase database secret code.

#define WIFI_SSID "DIAMANDGIFT_2G"
#define WIFI_PASSWORD "0864599551"

const char* host = "script.google.com";
const int httpsPort = 443;

WiFiClientSecure client;
FirebaseData firebaseData;

float data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
String GAS_ID = "AKfycbwABnzS-6LqgZZhxVvvnWTauPlFtxpo2nBoWQX1Mf9T72BL4U1jLHbr8qoo2CV_YdPd";

bool val;
void setup() {
  Serial.begin(115200);
  Firebase_connection();
  client.setInsecure();
}

void Firebase_connection() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Firebase_connection();
  }
  if (val) {
    val = false;
    Firebase.setInt(firebaseData, "phase/Link", 1);
  } else {
    val = true;
    Firebase.setInt(firebaseData, "phase/Link", 0);
  }
  pzem004(5, 4);
  pzem004(0, 2);
  pzem004(14, 12);
}

void pzem004(int digital1, int digital2) {
  String L = "";
  PZEM004Tv30 pzem(digital1, digital2);
  String v = "Voltage";
  String c = "Current";
  String p = "Power";
  String eg = "Energy";
  String feq = "Frequency";
  String PF = "PF";
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();
  if (digital1 == 5 && digital2 == 4) {
    L = "phase/L1/";
    data[0] = voltage;
    data[3] = current;
    data[6] = power;
    data[9] = energy;
    data[12] = frequency;
    data[15] = pf;
  }
  else if (digital1 == 0 && digital2 == 2) {
    L = "phase/L2/";
    data[1] = voltage;
    data[4] = current;
    data[7] = power;
    data[10] = energy;
    data[13] = frequency;
    data[16] = pf;
  }
  else if (digital1 == 14 && digital2 == 12) {
    L = "phase/L3/";
    data[2] = voltage;
    data[5] = current;
    data[8] = power;
    data[11] = energy;
    data[14] = frequency;
    data[17] = pf;
  }
  if (!isnan(voltage)) {
    Firebase.setFloat(firebaseData, L + v, voltage);
    Firebase.setFloat(firebaseData, L + c, current);
    Firebase.setFloat(firebaseData, L + p, power);
    Firebase.setFloat(firebaseData, L + eg, energy);
    Firebase.setFloat(firebaseData, L + feq, frequency);
    Firebase.setFloat(firebaseData, L + PF, pf);
    sendData();
  } delay(1000);

  if (val) {
    val = false;
    Firebase.setInt(firebaseData, "phase/Link", 1);
  } else {
    val = true;
    Firebase.setInt(firebaseData, "phase/Link", 0);
  }

}

void sendData() {

  if (!client.connect(host, httpsPort)) {
    return;
  }
  String url = "/macros/s/" + GAS_ID + "/exec?V1=" + data[0] + "&V2=" + data[1] + "&V3=" + data[2] + "&A1=" + data[3] + "&A2=" + data[4] + "&A3=" + data[5] + "&W1=" + data[6] + "&W2=" + data[7] + "&W3=" + data[8] + "&kwh1=" + data[9] + "&kwh2=" + data[10] + "&kwh3=" + data[11] + "&PF1=" + data[12] + "&PF2=" + data[13] + "&PF3=" + data[14] + "&HZ1=" + data[15] + "&HZ2=" + data[16] + "&HZ3=" + data[17];
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");
}

// 3,590,000 -> seconds time to post google sheet

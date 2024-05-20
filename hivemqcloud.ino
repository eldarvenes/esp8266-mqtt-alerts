#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include <TZ.h>
#include <FS.h>
#include <LittleFS.h>
#include <CertStoreBearSSL.h>
#include "config.h"

const int rødtLysPin = 5;
const int grøntLysPin = 4;
const int gultLysPin = 0;
const int lydPin = 2;

BearSSL::CertStore certStore;

WiFiClientSecure espClient;
PubSubClient * client;
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setDateTime() {

  configTime(TZ_Europe_Berlin, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(100);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();

  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.printf("%s %s", tzname[0], asctime(&timeinfo));
}

void controlLys(const char* payload, unsigned int length) {
  // Slukk alle lys først
  digitalWrite(rødtLysPin, LOW);
  digitalWrite(grøntLysPin, LOW);
  digitalWrite(gultLysPin, LOW);

  for (unsigned int i = 0; i < length; i++) {
    if (payload[i] == 'R') {
      digitalWrite(rødtLysPin, HIGH);
    } else if (payload[i] == 'G') {
      digitalWrite(grøntLysPin, HIGH);
    } else if (payload[i] == 'Y') {
      digitalWrite(gultLysPin, HIGH);
    } else if (payload[i] == 'X') {
      // Slukk alle lys
      digitalWrite(rødtLysPin, LOW);
      digitalWrite(grøntLysPin, LOW);
      digitalWrite(gultLysPin, LOW);
      break;
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, "lys") == 0) {
    controlLys((const char*)payload, length);
  } else if (strcmp(topic, "lyd") == 0) {
    if ((char)payload[0] == '1') {
      digitalWrite(lydPin, HIGH);
      delay(500);
      digitalWrite(lydPin, LOW);
    } else {
      digitalWrite(lydPin, LOW);
    }
  }
}

void reconnect() {
  // Loop until we’re reconnected
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client - MyClient";
    // Attempt to connect
    if (client->connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client->subscribe("lys");
      client->subscribe("lyd");
    } else {
      Serial.print("failed, rc = ");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  delay(500);
  Serial.begin(9600);
  delay(500);

  LittleFS.begin();
  setup_wifi();
  setDateTime();

  pinMode(rødtLysPin, OUTPUT);
  pinMode(grøntLysPin, OUTPUT);
  pinMode(gultLysPin, OUTPUT);
  pinMode(lydPin, OUTPUT);

  int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.printf("Number of CA certs read: %d\n", numCerts);
  if (numCerts == 0) {
    Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the LittleFS directory before running?\n");
    return;
  }

  BearSSL::WiFiClientSecure *bear = new BearSSL::WiFiClientSecure();
  bear->setCertStore(&certStore);

  client = new PubSubClient(*bear);

  client->setServer(mqtt_server, 8883);
  client->setCallback(callback);
}

void loop() {
  if (!client->connected()) {
    reconnect();
  }
  client->loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    // client->publish("testTopic", msg); // Uncomment to publish test messages
  }
}

//DEFINES AND FUNCTION PROTOTYPES

#include <ESP8266WiFi.h>
#include "ESP8266HTTPClient.h"
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Stream.h>
#include <MillisTimer.h>

//AWS
#include "sha256.h"
#include "Utils.h"


//WEBSockets
#include <Hash.h>
#include <WebSocketsClient.h>

//Paho MQTT
#include <SPI.h>
#include <IPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>


//HTTP REQUESTS
HTTPClient http;

//MQTT PUBSUBCLIENT LIB
#include <PubSubClient.h>

//AWS MQTT Websocket
#include "Client.h"
#include <AWSWebSocketClient.h>
#include "CircularByteBuffer.h"

extern "C" {
#include "user_interface.h"
}

#define rele D1      // the number of the LED pin
#define Led_Aviso D0

const int buttonResetPin = 4; //Button reset wifi
int buttonResetState = 0;

const int  FLOW_SENSOR_PIN = D2;// Sensor Input

char aws_endpoint[]    = "agq6mvwjsctpy-ats.iot.us-east-1.amazonaws.com";
char aws_region[]      = "us-east-1";
const char* aws_topic_times  = strdup(((String)ESP.getChipId() + "/times").c_str());
const char* aws_topic_conf  = strdup(((String)ESP.getChipId() + "/configuration").c_str());
const char* aws_statistics_topic  = "statistics";
int port = 443;

//Check Connection Variable
MillisTimer checkConnectionTimer = MillisTimer(1000);

//Bath timers
MillisTimer bathDurationTimer = MillisTimer(1000);
MillisTimer bathStopTimer = MillisTimer(1000);
MillisTimer bathWaitingTimer = MillisTimer(1000);
MillisTimer bathScanTimmer = MillisTimer(1000);
MillisTimer bathFalseAlarmTimmer = MillisTimer(1000);
unsigned long flowLastValue;
int bathRemainingTime;
int stopRemainingTime;

//MQTT config
const int maxMQTTpackageSize = 512;
const int maxMQTTMessageHandlers = 1;
//
AWSWebSocketClient awsWSclient(1000);
//
PubSubClient client(awsWSclient);
//IPStack ipstack(awsWSclient);
//MQTT::Client<IPStack, Countdown, maxMQTTpackageSize, maxMQTTMessageHandlers> client(ipstack);

//# of connections
long connection = 0;

//count messages arrived
int arrivedcount = 0;

bool shouldSaveConfig = false;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 25;    // the debounce time; increase if the output flickers

//positions to save variables on the EEPROM
int address_time = 0;
int address_wait = 1;
int address_stopped = 2;
int address_password = 3;
int address_email = 4;
int test_timer = 2;
boolean showerFalseAlarmTesting = false;
boolean falseAlarmRunning = false;

byte armazenado;
byte bathTime = EEPROM.read(address_time); //tempo de banho
byte bathWaitTime = EEPROM.read(address_wait); //tempo de espera atÃ© o banho ser habilitado novamente
byte bathStoppedTime = EEPROM.read(address_stopped); // tempo que o banho pode ficar pausado
byte password = EEPROM.read(address_password);
byte email = EEPROM.read(address_email);

//int tempo = (int)minutos * 60;
//int tempo_espera = (int)minutos_espera * 60;
//int tempo_de_pausa = (int)minutos_pausa * 60;

#define DBG_OUTPUT_PORT Serial

ESP8266WebServer server(80);

// TODO - Verify if all these functions will be used
//API REST Mapping Functions
bool handleFileRead(String path);
void selectDurationTime();
void setActualShowerTimePlus();
void setActualShowerTimeLess();
void selectOffTime();
void setActualOffTimePlus();
void setActualOffTimeLess();
void selectPausedTime();
void setActualPausedTimePlus();
void setActualPausedTimeLess();

// Shower logic variables

volatile int flow_frequency; // Measures flow sensor pulses
unsigned long totalFlowFrequency;
unsigned int l_hour; // Calculated litres/hour
unsigned long currentTime;
unsigned long cloopTime;
boolean bathRunning;
boolean showerIsOn;
boolean waiting;

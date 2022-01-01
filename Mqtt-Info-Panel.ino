// 211121 First In first out added for Queue, String class removed!
// 101121 Keep Alive pixel added
// 061121 Print Anweisungen im Callback entfernt Absturzgefahr
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


///////////////////////////////////use NodeMCU 1.0, select in IDE /////////////////////
#define PIN D6
#define Led_delay 40
#define Led_bright 100
#define NUMstrip 768
#define Textlenght 350

/************************* WiFi Access Point *********************************/

// Update these with values suitable for your network.
const char* ssid = "Your ID";
const char* password = "your Password" ;
const char* mqtt_server = "192.168.1.48";

long loop_cnt, Clock;
long callback_cnt;
char reconnect_cnt, Pixel;
int Led;
long lastMsg = 0;
char msg[50];
int value = 0;
char topic_global[30][500];
// char topic_global = new char[10][40];
int Mqtt_in_zeiger, Mqtt_out_zeiger;
char Clear_done;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMstrip, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(96, 8, PIN,NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG, NEO_GRB + NEO_KHZ800);
WiFiClient espClient;
PubSubClient client(espClient);

const uint16_t colors[] = {matrix.Color(255, 60, 0), matrix.Color(255, 255, 0), matrix.Color(255, 0, 0), matrix.Color(0, 0, 255), matrix.Color(0, 255, 0), matrix.Color(255, 0, 255), matrix.Color(255, 0, 100)};
int x    = matrix.width();
int pass = 0;

uint32_t Wheel(byte WheelPos) {
  uint32_t returnvalue;
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    returnvalue = strip.Color(255 - WheelPos * 3, 0, WheelPos * 3, 0);
  }
  else {
    if (WheelPos < 170) {
      WheelPos -= 85;
      returnvalue =  strip.Color(0, WheelPos * 3, 255 - WheelPos * 3, 0);
    }
    else
    {
      WheelPos -= 170;
      returnvalue = strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0, 0);
    }
  }
  return (returnvalue);
}


void rainbowCycle(uint numbers) {
  uint16_t i, j;

  for (j = 0; j < 256 * numbers; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < NUMstrip; i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / NUMstrip) + j) & 255));
      // Serial.print(i);
      yield(); // Avoid WD error
    }
    strip.show();

  }
}
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

void reconnect() {
  // Loop until we're reconnected
  // 5 Versuche
  while (!client.connected() || (reconnect_cnt == 6)) {
    // setup_wifi();
    reconnect_cnt++;
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    char* clientId = "ESP8266_01";
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe("rasp4_2GB/+");
      client.subscribe("waveshare/+");
      client.subscribe("knx/sensor/+");
      client.subscribe("explorer700/+");
      client.subscribe("epaper/+");
      client.subscribe("news/+");
      client.subscribe("Aussen/+");
      // show all messages
      client.subscribe("+");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
} // end reconnect
// void scrollText(String textToDisplay) {
void scrollText(char* textToDisplay) {  
  // int x = matrix.width(); // Text läuft von ganz rechts los
  int x = 10; // fast ganz links start
  // Account for 6 pixel wide characters plus a space
  int pixelsInText = (strlen(textToDisplay) * 8) + 50; // + 120: Text weiter scrollen bis Display leer
  //int pixelsInText = (textToDisplay.length() * 8) + 50; // + 120: Text weiter scrollen bis Display leer
  while (x > (matrix.width() - pixelsInText)) {
    matrix.fillScreen(matrix.Color(0, 0, 0));
    matrix.setCursor(--x, 0);
    matrix.print(textToDisplay);
    matrix.show();
    delay(1);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  (length > 499) ? length = 499 : length;
  strcpy(topic_global[Mqtt_in_zeiger], topic);
  strcat(topic_global[Mqtt_in_zeiger], ": ");
  strncat(topic_global[Mqtt_in_zeiger], (char *)payload, length); // Payload auf globale Var kopieren
  if (Mqtt_in_zeiger < 29) Mqtt_in_zeiger++;
  // Serial.println(Mqtt_in_zeiger);
}

void setup() {
  Serial.begin(115200);
  //delay(10);
  setup_wifi();
  //delay(1000);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //timeClient.begin();

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(Led_bright);
  matrix.setTextColor(colors[0]);
#if 0
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &ISR_Task_10ms, true);
  timerAlarmWrite(timer, 77400, true); // alle 10 ms
  timerAlarmEnable(timer);
#endif
  strip.setBrightness(5);
  rainbowCycle(1);
  strip.setBrightness(Led_bright);
  Mqtt_in_zeiger = Mqtt_out_zeiger = 0;
  callback_cnt = 0;
}
void loop() {
  loop_cnt ++;
  Clock++;
  // Mqtt ///////////////////////////////////////////////////////
  if (!client.connected()) {reconnect();delay(100);}
  client.loop();
  //////////////////////////////////////////////////////////////
  
  if ((Mqtt_in_zeiger) && (loop_cnt > 100000)) // Wartezeit, damit alle Mqtt Botschaften empfangen werden können  -> ScrollText blockiert
  {
    loop_cnt = 0;
    // Temperatur = rot
    if ((strstr(topic_global[Mqtt_out_zeiger], "Temp")) || (strstr(topic_global[Mqtt_out_zeiger], "temp"))) matrix.setTextColor(colors[2]);
    else
    { // Feuchte = blau
      if ((strstr(topic_global[Mqtt_out_zeiger], "Feuch")) || (strstr(topic_global[Mqtt_out_zeiger], "hum"))) matrix.setTextColor(colors[3]);
      else
      { // Luftdruck = blau
        if ((strstr(topic_global[Mqtt_out_zeiger], "Luft")) || (strstr(topic_global[Mqtt_out_zeiger], "druck"))) matrix.setTextColor(colors[4]);
        else
        {
          if (strstr(topic_global[Mqtt_out_zeiger], "news")) matrix.setTextColor(matrix.Color(255, 255, 255)); //weiß
          else { // knx_lila
            if (strstr(topic_global[Mqtt_out_zeiger], "Knx")) matrix.setTextColor(colors[5]);
            else matrix.setTextColor(colors[0]); // sonst orange
          }
        }

      }
    }
//    Serial.println(topic_global[Mqtt_out_zeiger]);
    scrollText(topic_global[Mqtt_out_zeiger]);
    //    Serial.print("Zeigerwert: ");
    //    Serial.println(zeiger);
    Mqtt_out_zeiger++;
    if ((Mqtt_in_zeiger == Mqtt_out_zeiger)&& (Mqtt_in_zeiger != 0)) //queue ist leer und nicht schon resettet
    {
      Mqtt_in_zeiger = 0;
      Mqtt_out_zeiger = 0;
    }
    Clear_done = 0;
  }
  else
  {
    if (Clear_done == 0)
    {
      matrix.fillScreen(0);
      matrix.show();
      Clear_done = 1;
      callback_cnt = 0;
    }
  }
  if (!(Clock & 0xffff))
  { // Keep alive pixel schreiben
    matrix.setPixelColor(Led, matrix.Color(0, 4, 0));
    matrix.setPixelColor(Led - 1, 0);
    matrix.show();
    if (Led > 767) Led = 0;
    else Led++;
  }
}

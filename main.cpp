#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include "wifi-credentials.h"
#include "userdefs.h"

#define histlen 10
uint hist_array[histlen] = {0};
uint hist_idx = 0;
uint32_t avglen = 0;
int avgcnt = 0;


uint debounce = DEBOUNCE;


// Replace with your network credentials defined in userdefs.h
const char *ssid = USER_WIFI_SSID;
const char *password = USER_WIFI_PASS;

int ledstate      = LEDOFF;
bool Device_Armed = true;
bool sensor_triggered = false;
uint trigger_start_time = 0;

uint barrage_end  = 0; // end firing clock
uint shots_fired  = 0;
uint trigger_count = 0;
unsigned long int active_end_msec = 0;
char updateUrl[100] = {0};
#define DEBUGLEN 500
char debuginfo[DEBUGLEN] = {0};

ESP8266WebServer server(80); //instantiate server at port 80 (http port)
ESP8266HTTPUpdateServer httpUpdater;

char page[PAGE_LENGTH+100] = {0};

void setled(uint32_t state)
{
    digitalWrite(LED_PIN_OUT, state);
    digitalWrite(LED_ONBOARD, state);
}
void redirect(void)
{
    char buttonstr[40];
    unsigned long minutes_left = 0;
    if(active_end_msec > millis() ){
      minutes_left = ((active_end_msec - millis())/(SECONDS_IN_MINUTE * SECONDS_IN_MS));
    } 
    snprintf(buttonstr, 40, "%s", (Device_Armed ? "Disarm" : "Arm"));
    snprintf(page, PAGE_LENGTH, BASEPAGE, 0, (Device_Armed ? "<font size=\"3\" color=\"red\">Armed</font>" : 
    "<font size=\"3\" color=\"blue\">Disarmed</font>"), trigger_count, shots_fired, buttonstr, buttonstr, 
        (uint) minutes_left / 60, (uint) minutes_left % 60, avglen, updateUrl,debuginfo);

    server.send(200, "text/html", page);
}
void refresh(void)
{
    char buttonstr[40];
    unsigned long minutes_left = 0;
    if(active_end_msec > millis() ){
      minutes_left = ((active_end_msec - millis())/(SECONDS_IN_MINUTE * SECONDS_IN_MS));
    } 
    snprintf(buttonstr, 40, "%s", (Device_Armed ? "Disarm" : "Arm"));
    snprintf(page, PAGE_LENGTH, BASEPAGE, 30, (Device_Armed ? "<font size=\"3\" color=\"red\">Armed</font>" : 
    "<font size=\"3\" color=\"blue\">Disarmed</font>"), trigger_count, shots_fired, buttonstr, buttonstr, 
        (uint) minutes_left / 60, (uint) minutes_left % 60, avglen, updateUrl,debuginfo);
   server.send(200, "text/html", page);
}
uint init_wifi()
{
  uint32_t wrap = 1;
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.hostname(USER_WIFI_HOSTNAME);
    WiFi.begin(ssid, password);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    setled(wrap % 2);
    if (wrap++ % 80 == 0)
      Serial.println("\n");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  if (MDNS.begin(USER_WIFI_HOSTNAME))
  {
      MDNS.addService("http", "tcp", 80);
      Serial.printf("MDNS online, %s reachable\n", USER_WIFI_HOSTNAME);
  } else {
      Serial.println("Error setting up MDNS responder!\n");
  }
  return 1;
}
void init_httpd()
{

  server.on("/", []() {
    refresh();
  });
  server.on("/Arm", []() {
    setled(LEDON);
    Device_Armed = 255;
    Serial.println("Armed");
    redirect();
    delay(50);
  });
  server.on("/Disarm", []() {
    setled(LEDOFF);
    Device_Armed = 0;
    Serial.println("Disarmed");
    redirect();
    delay(50);
  });
  server.on("/reset-count", []() {
    trigger_count = 0;
    shots_fired = 0;
    redirect();
    delay(50);
  });
  server.on("/reset-clock", []() {
    active_end_msec = millis() + ACTIVE_MSEC;
    redirect();
    delay(50);
  });
  
  server.begin();
  Serial.printf("Web server started. Access via http://%s.local/\n", USER_WIFI_HOSTNAME);
  httpUpdater.setup(&server);
  Serial.printf("HTTPUpdateServer ready. http://%s.local/update\n", USER_WIFI_HOSTNAME);
  snprintf(updateUrl, 100,"<a href=\"http://%s.local/update\">http://%s.local/update</a>", USER_WIFI_HOSTNAME,USER_WIFI_HOSTNAME);
}
void update_histeresis(uint ctime){
    if((ctime - trigger_start_time) < DEBOUNCE){
      hist_array[hist_idx++ % histlen] = ctime - trigger_start_time;
      avgcnt = 0;
      avglen = 0;
      for(int i = 0;i<histlen;i++){
        //Average the populated entries
        if(hist_array[i]){
          avglen += hist_array[i];
          avgcnt++;
        }
      }
      avglen = avglen / avgcnt; 
      if (avgcnt > (histlen / 2)){
        debounce = avglen + 2000;
      }
    }
    snprintf(debuginfo,DEBUGLEN,"History: %d %d %d %d %d %d %d %d %d %d",
    hist_array[0],hist_array[1],hist_array[2],hist_array[3],hist_array[4],
    hist_array[5],hist_array[6],hist_array[7],hist_array[8],hist_array[9]);
}


void setup(void)
{
  char buttonstr[40];
  active_end_msec = millis() + ACTIVE_MSEC;
  unsigned long minutes_left =  ((active_end_msec - millis())/(SECONDS_IN_MINUTE * SECONDS_IN_MS));

  snprintf(buttonstr, 40, "%s", (Device_Armed ? "Disarm" : "Arm"));
   snprintf(page, PAGE_LENGTH, BASEPAGE, 0, (Device_Armed ? "<font size=\"3\" color=\"red\">Armed</font>" : 
   "<font size=\"3\" color=\"blue\">Disarmed</font>"), trigger_count, shots_fired, buttonstr, buttonstr, 
        (uint) minutes_left / 60, (uint) minutes_left % 60, avglen, updateUrl,debuginfo);
  //the HTML of the web page
  //make the LED pin output and initially turned off
  pinMode(LED_PIN_OUT, OUTPUT);
  pinMode(LED_ONBOARD, OUTPUT);

  setled(LEDOFF);
  pinMode(PUMP_PIN_OUT, OUTPUT);
  digitalWrite(PUMP_PIN_OUT, PUMPOFF);

  delay(1000);
  Serial.begin(9600);
  Serial.println("");
  if(init_wifi()) {
    init_httpd();
  }
  for(int i = 0;i<histlen;i++){
      hist_array[i] = 0;
    }
}

void loop(void)
{
  server.handleClient();
  uint ctime = millis();

  if(ctime > active_end_msec){
    return;
  }


  if ((sensor_triggered) ||
      ((ctime % ALIVE_PULSE_PERIOD) < ALIVE_PULSE_LENGTH)){
    setled(LEDON);
  } else {
    setled(LEDOFF);
  }
  if (ctime < QUIET_PERIOD)
  {
    delay(50);
    return;
  }

  if (ctime > barrage_end)
  {
    digitalWrite(PUMP_PIN_OUT, PUMPOFF);
  }
  else
  {
    if ((ctime % PULSE_PERIOD) < BURST_LENGTH)
    {
      digitalWrite(PUMP_PIN_OUT, PUMPON);
    }
    else
    {
      digitalWrite(PUMP_PIN_OUT, PUMPOFF);
    }
  }

  if (digitalRead(PIR_PIN_IN))
  {
    if (Device_Armed)
    {
      if (!sensor_triggered)
      {
        sensor_triggered = 1;
        trigger_count++;
        trigger_start_time = ctime;
        Serial.println("creature detected");
      }
      //Serial.println("Triggered!!!");
      if (((ctime - trigger_start_time) > debounce) &&
          (ctime > (barrage_end + PAUSE)))
      { // if its been hot for x secs, new barrage
        barrage_end = ctime + BARRAGE_LENGTH;
        Serial.println("Firing barrage");
        shots_fired++;
      }
      refresh();
    }
  }
  else
  {
    if (sensor_triggered)
    {// Sensor ceased signallins
      Serial.printf("Sensor time: %d\n", ctime - trigger_start_time);
      update_histeresis(ctime);
    }
    sensor_triggered = 0;
    //Histeresis

  }
  delay(50);
}

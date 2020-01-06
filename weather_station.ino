/*
   ESP8266, TFT ILI9341 240x320, sensor DHT22
   Weather Station
   by Fabio Kimura
   fkimura@gmail.com
*/

#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ArduinoJson.h"
#include "SPI.h"
#include "DHT.h"

// bibliotecas Adafruit
#include "Adafruit_GFX.h"
#include "Adafruit_ImageReader.h"
#include "Adafruit_ILI9341.h"

// Fontes
#include "fonts/FreeMono9pt7b.h"
#include "fonts/OpenSansBold9pt7b.h"
#include "fonts/OpenSansBold14pt7b.h"
#include "fonts/OpenSansBold30pt7b.h"
#include "fonts/FreeSerifBold9pt7b.h"
#include "fonts/FreeSerif9pt7b.h"
// Icones em bmp convertidos para bytes
#include "icons.h"
// configure a rede wifi e a localizacao aqui:
#include "configure.h"

boolean controleVentilador = false;

// Cores
#define ILI9341_GRAY 0x734D
#define ILI9341_BLUE 0x04D5
#define ILI9341_GREEN       0x07E0  ///<   0, 255,   0
#define ILI9341_RED         0xF800  ///< 255,   0,   0

#define DHT_PIN 10
#define TFT_DC D7 // DC/RS necessario
#define TFT_CS D1 // CS: Chip Select da tela. Se você não estiver usando mais nada no bus SPI (como o leitor SD) pode ser deixado permanentemente ligado a GND para operação normal do display.
#define TFT_LED D8
//builtin_led = D4, nao usar para nao ficar piscando
#define TFT_MISO D0 //SDO/MISO: Não é realmente necessário usar e pode ser deixado desconectado. O terminal MISO é usado para “ler” informações do dispositivo SPI escravo e em quase 100% dos casos você não precisa ler nada de uma tela;
#define TFT_CLK D5
#define TFT_MOSI D2
#define TFT_RST D6 // RESET: Pode ser deixado permanentemente ligado a +3,3V para operação normal do display.
#define BUTTON_PIN D3

DHT dht(DHT_PIN, DHT22); // define o pino e o tipo de DHT, se utilizar o DHT11 basta alterar para DHT11 aqui
float temperatura = 0.0; // variável para armazenar o valor de temperatura
int umidade = 0; // variável para armazenar o valor de umidade

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);
Adafruit_ImageReader reader;
icons _icons = icons(&tft);

String previsao_anterior = "x";
String previsao_atual = "x";

void ICACHE_RAM_ATTR verificaBotao();

void setup() {

  Serial.begin(115200);
  pinMode(TFT_LED, OUTPUT);
  digitalWrite(TFT_LED, HIGH);
  dht.begin(); // inicializa o sensor DHT

  pinMode(BUTTON_PIN, INPUT); 
 // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), verificaBotao, FALLING); 

  tft.begin();
  //  tft.setRotation(2);

  connectWifi();
  if (WiFi.status() == WL_CONNECTED) {
    tft.println("\nConectado!");
  }

  delay(1000);

  tft.fillScreen(ILI9341_BLACK);
  printWifi();
}

int botao = 0;
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }
  desenhaPrevisao();
  desenhaTempUmidade();
  getCurrentWeather();
  desenhaHoras();
  botao = 0;
  for (int i = 0; i < 60; i++) {
    desenhaTempUmidade();
    for (int j = 0; j < 60; j++) {
      verificarBotao();
      delay(100);
    }
  }
}

void verificarBotao() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    botao++;
    if (botao >= 4) {
      botao = 0;
      controleVentilador = !controleVentilador;
      desenhaVentilador(); 
    }
    trocaPrevisao(); 
  }
}
void connectWifi() {
  String ssid = ssid2;
  while (WiFi.status() != WL_CONNECTED) {
    if (ssid1.equals(ssid)) {
      ssid = ssid2;
    } else {
      ssid = ssid1;
    }
    tft.setFont(&FreeMono9pt7b);
    tft.setTextSize(1);
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(1);

    tft.setCursor(0, 38);
    Serial.println(WiFi.status());
    Serial.println("Conectando wifi");
    tft.println("Conectando wifi");
    tft.println(ssid);
    WiFi.begin(ssid, password);
    tft.print("Conectando");
    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED) {
      if (timeout > 10) {
        tft.setTextColor(ILI9341_RED);
        tft.setCursor(0, 0);
        tft.println("\nFalha ao conectar!");
        return;
      }
      tft.print(".");
      timeout++;
      delay(1000);
    }
  if (WiFi.status() == WL_CONNECTED) Serial.println("Connected");
  }
}

// Get weather data in JSON from OpenWeatherMap API
void getCurrentWeather() {
  // Setup request string;
  String temperature_unit;
  char* temperature_deg;
  temperature_unit = "metric";
  temperature_deg = "C";

  //  String current = "http://api.openweathermap.org/data/2.5/weather?q="+city+"&units="+temperature_unit+"&APPID="+key;
  String current = "http://api.openweathermap.org/data/2.5/weather?"+lugar+"&lang=pt&&units=" + temperature_unit + "&APPID=" + key;
  Serial.println(current);
  printWifi();
  Serial.print("Getting current weather");
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;

    http.begin(current);
    int httpCode = http.GET();

    String payload;
    if (httpCode > 0) { //Check for the returning code
      const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(12) + 600;
      DynamicJsonDocument doc(capacity);

      String payload = http.getString();
      //Serial.println(httpCode);
      //Serial.println(payload);

      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      }

      ImageReturnCode stat;

      int id = doc["weather"][0]["id"];
      Serial.println(id);

      float temperature = doc["main"]["temp"];
      int humidity = doc["main"]["humidity"];
      int pressure = doc["main"]["pressure"];
      float visibility = doc["visibility"];
      String description = doc["weather"][0]["description"];
      float wind_speed = doc["wind"]["speed"];
      int wind_deg = doc["wind"]["deg"];
      int rain_volume = doc["rain"]["1h"];
      int snow_volume = doc["snow"]["1h"];
      String horas = convertTimestamp(doc["dt"], time_format);
      Serial.println("horas=" + horas);
      //    desenhaHoras(horas);
      //int id = 700;
      bool is_night = true;

      // Weather image
      char icon[28];

      if (id < 233) char icon[] = "/weather/wi-thunderstorm.bmp";
      if (id > 299 && id < 322) strncpy(icon, "/weather/wi-sleet.bmp", 28);
      if (id > 499 && id < 502) strncpy(icon, "/weather/wi-showers.bmp", 28);
      if (id > 501 && id < 532) strncpy(icon, "/weather/wi-rain.bmp", 28);
      if (id > 599 && id < 623) strncpy(icon, "/weather/wi-snow.bmp", 28);
      if (id > 699 && id < 782) strncpy(icon, "/weather/wi-fog.bmp", 28);




      // Different icon for night and day
      bool isDay = ((intTime(doc["dt"]) >= intTime(doc["sys"]["sunrise"])) && (intTime(doc["dt"]) <= intTime(doc["sys"]["sunset"])));
      if (id == 800 && isDay) strncpy(icon, "/weather/wi-day-sunny.bmp", 28);
      if (id == 800 && !isDay) strncpy(icon, "/weather/wi-stars.bmp", 28);

      if (id > 800 && id < 803) strncpy(icon, "/weather/wi-day-cloudy.bmp", 28);
      if (id > 802) strncpy(icon, "/weather/wi-cloud.bmp", 28);

      tft.fillRect(0, 20, 120, 105, ILI9341_BLACK);

      drawBitmap(icon, 10, 26);

      tft.setFont(&OpenSansBold9pt7b);
      tft.setCursor(10, 137);
      tft.fillRect(0, 124, 232, 24, ILI9341_BLACK);
      description.replace("é", "e");
      description.replace("ç", "c");
      description.replace("ã", "a");
      tft.print(description);
      tft.setFont(&OpenSansBold14pt7b);
      tft.setCursor(18, 172);
      tft.print(temperature_deg);
      tft.setFont(&OpenSansBold9pt7b);
      tft.setCursor(8, 162);
      tft.print("o");

      // Main temperature
      tft.setFont(&OpenSansBold30pt7b);
      tft.fillRect(37, 150, 180 - 37, 54, ILI9341_BLACK);
      tft.setCursor(40, 195);

      //    temperature = temperature*10;
      //    temperature = round(temperature);
      //   tft.print(temperature/10,1);
      int t = (int) temperature;
      int g = (int)((temperature - t) * 10);
      tft.print(t);
      tft.print(",");
      tft.print(g);


      tft.setFont(&OpenSansBold9pt7b);

      tft.fillRect(50, 253 , 268, 39, ILI9341_BLACK);

      drawBitmap("/wi-strong-wind.bmp", 5, 250);

      String direction = "Norte";
      if (wind_deg > 22.5) direction = "Nordeste";
      if (wind_deg > 67.5) direction =  "Leste";
      if (wind_deg > 122.5) direction =  "Sudeste";
      if (wind_deg > 157.5) direction =  "Sul";
      if (wind_deg > 202.5) direction =  "Sudoeste";
      if (wind_deg > 247.5) direction =  "Oeste";
      if (wind_deg > 292.5) direction =  "Noroeste";
      if (wind_deg > 337.5) direction =  "Norte";

      tft.setCursor(50, 278);
      tft.print(direction + " ");
      tft.print(wind_speed, 1);
      tft.print(" km/h");

      int y = 20;


      printRow(120, 25, visibility / 1000, "/wi-dust.bmp", "km", 1);
      printRow(120, 57, humidity, "/wi-humidity.bmp", "%", 0);
      printRow(120, 90, pressure, "/wi-barometer.bmp", "hPa", 0);

      tft.setTextColor(ILI9341_BLUE);
      printRow(0, 198, rain_volume, "/wi-raindrops.bmp", "mm", 0);
      printRow(120, 198, snow_volume, "/wi-snowflake-cold.bmp", "mm", 0);
      tft.setTextColor(ILI9341_WHITE);

      printRow(10, 283, -1, "/wi-sunrise.bmp", doc["sys"]["sunrise"], 0);
      printRow(130, 283, -1, "/wi-sunset.bmp", doc["sys"]["sunset"], 0);

    } else {
      Serial.println("HTTP request error");
    }
    http.end();
  }
}

void desenhaVentilador() {
  tft.setFont(&OpenSansBold9pt7b);
  tft.setCursor(8, 192);
  if (controleVentilador) {
    tft.print("+");
  } else {
    tft.fillRect(8, 192 - 11 , 12, 12, ILI9341_BLACK);
  }
}

void printRow(int x, int y, float volume, char *icon, String unit, int decimal) {

  tft.fillRect(x, y + 5, 130, 25, ILI9341_BLACK);

  if (volume == 0) return;

  //  ImageReturnCode stat;
  //  stat = reader.drawBMP(icon, tft, x, y);
  drawBitmap(icon, x, y);
  tft.setFont(&OpenSansBold9pt7b);

  if (icon == "/wi-sunrise.bmp" || icon == "/wi-sunset.bmp") {
    unit = convertTimestamp(unit, time_format);
    tft.setCursor(x + 42, y + 25);
    tft.print(unit);
    return;
  }


  tft.setCursor(x + 37, y + 25);
  tft.print(volume, decimal);
  tft.print(" ");
  tft.println(unit);
}

void drawBitmap(char *icon, int x, int y) {
  _icons.drawBitmap(icon, x, y);
}

void printWifi() {
  tft.setFont(&OpenSansBold9pt7b);
  tft.setTextColor(ILI9341_GREEN);
  tft.fillRect(0, 0, 240, 22, ILI9341_BLACK);

  tft.fillRect(221, 16, 4, 3, ILI9341_GRAY);
  tft.fillRect(226, 13, 4, 6, ILI9341_GRAY);
  tft.fillRect(231, 8, 4, 11, ILI9341_GRAY);
  tft.fillRect(236, 3, 4, 16, ILI9341_GRAY);

  ImageReturnCode stat;

  if (WiFi.status() != WL_CONNECTED) {
    drawBitmap("/wifi1.bmp", 90, 0);
    tft.setCursor(120, 16);
    tft.println("sem wifi");
    return;
  }

  //  drawBitmap("/wifi2.bmp", 80, 0);
  tft.setCursor(92, 16);
  String nome = WiFi.SSID();
  if (nome.length() <= 12) {
    tft.print(" ");
  }
  if (nome.length() <= 15) {
    tft.print(" ");
  }
  tft.print(nome);
  //  tft.print("rede-wifi");
  //  tft.setCursor(160, 16);
  //  tft.print(WiFi.RSSI());
  tft.setTextColor(ILI9341_WHITE);

  int ss = WiFi.RSSI();

  if (ss > -90) tft.fillRect(221, 16, 4, 3, ILI9341_WHITE);
  if (ss > -80) tft.fillRect(226, 13, 4, 6, ILI9341_WHITE);
  if (ss > -70) tft.fillRect(231, 8, 4, 11, ILI9341_WHITE);
  if (ss > -65) tft.fillRect(236, 3, 4, 16, ILI9341_WHITE);
}


String convertTimestamp(String timestamp, byte time_format) {
  long int time_int = timestamp.toInt();
  time_int = time_int + timezone * 3600;
  struct tm *info;

  time_t t = time_int;
  info = gmtime(&t);
  String result;
  byte hours = ((info->tm_hour) > 12 && time_format == 1) ? info->tm_hour - 12 : info->tm_hour;
  String noon;
  if (time_format == 1) {
    noon = ((info->tm_hour) > 12) ? " PM" : " AM";
  } else {
    noon = "";
  }


  String o = (info->tm_min < 10) ? "0" : "";
  result = String(hours) + ":" + o + String(info->tm_min) + noon;

  return result;
}

int intTime(String timestamp) {
  long int time_int = timestamp.toInt();
  struct tm *info;
  time_t t = time_int;
  info = gmtime(&t);
  int result = info->tm_hour * 100 + info->tm_min;

  return result;
}


int chamarURL(String url) {
  HTTPClient http;
  http.begin(url);
  http.setTimeout(30000);
  int httpCode = http.GET();
  http.end();
  Serial.println(httpCode); 
  return httpCode;
}

boolean ventiladorLigado = false;

void desenhaTempUmidade() {
  tft.fillRect(182, 162 - 14, 240 - 180, 39, ILI9341_BLACK);
  temperatura = dht.readTemperature() ; // lê a temperatura em Celsius
  umidade = dht.readHumidity(); // lê a umidade
  if (isnan(umidade) || isnan(temperatura)) {
    tft.setCursor(182, 162);
    tft.println("Erro");
    Serial.println("Erro!");
    return;
  } else { // Se não
    Serial.println("temperatura=" + String(temperatura) + ", umidade=" + String(umidade));
  }
  tft.setCursor(182, 162);
  if (temperatura >= 27) {
    tft.setTextColor(ILI9341_RED);
  } else {
    tft.setTextColor(ILI9341_WHITE); 
  }
  int t = (int) temperatura;
  int g = (int)((temperatura - t) * 10);
  tft.print(t);
  tft.print(",");
  tft.print(g);
  tft.println(" C");
  if (umidade <= 50) {
    tft.setTextColor(ILI9341_RED);
  } else {
    tft.setTextColor(ILI9341_WHITE); 
  }
  tft.setCursor(182, 182);
  tft.print(umidade);
  tft.println(" %");
  int resultado = 0;
  tft.setCursor(182, 202);
  if (controleVentilador && temperatura >= 27 && !ventiladorLigado) {
    resultado = chamarURL("http://192.168.1.149/mqtt/ligarVentilador.php");
    if (resultado > 0 ) {
      ventiladorLigado = true;
      drawBitmap("/weather/fan-on.bmp", 190, 189);
    }
  }
  if (controleVentilador && temperatura <= 25 && ventiladorLigado) {
    resultado = chamarURL("http://192.168.1.149/mqtt/desligarVentilador.php");
    if (resultado > 0 ) {
      ventiladorLigado = false;
      tft.fillRect(190, 189, 40, 40, ILI9341_BLACK);
    }
  }
  Serial.print("resultado=");
  Serial.println(resultado);
}

String semana[7] = { "dom", "seg", "ter", "qua", "qui", "sex", "sab" };
void desenhaHoras() {
  HTTPClient http;  //Declare an object of class HTTPClient
  http.begin("http://worldtimeapi.org/api/timezone/America/Sao_Paulo");  //Specify request destination
  int httpCode = http.GET(); //Send the request
  if (httpCode > 0) { //Check the returning code
    String payload = http.getString();   //Get the request response payload
    Serial.println(payload);
    int i = payload.indexOf("\"datetime\"");
    String datetime = payload.substring(i + 12, payload.indexOf("\"", i + 12));
    Serial.println(datetime);
    i = datetime.indexOf("T");
    String date = datetime.substring(0, i);
    String time = datetime.substring(i + 1, i + 6);
    int horas = time.substring(0, time.indexOf(":")).toInt();
    Serial.println("horas = " + String(horas));
    i = payload.indexOf("\"day_of_week\"");
    int dow = payload.substring(i + 14, i + 15).toInt();
    Serial.println("day of week:" + String(dow));
    tft.setFont(&OpenSansBold9pt7b);
    tft.setCursor(0, 16);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.print(semana[dow] + " ");
    tft.println(time);

    if (horas >= 2 && horas <= 6) {
      analogWrite(TFT_LED, 32);
    } else if (horas >= 0 && horas <= 7) {
      analogWrite(TFT_LED, 64);
    } else if (horas >= 20) {
      analogWrite(TFT_LED, 128);
    } else if (horas >= 22) {
      analogWrite(TFT_LED, 96);
    } else {
      digitalWrite(TFT_LED, HIGH);
    }
    if (horas == 10) {
      controleVentilador = false;
    }
  }
  http.end();   //Close connection
}



void desenhaPrevisao() {
  String current = "http://api.openweathermap.org/data/2.5/forecast?"+lugar+"&lang=pt&units=metric&APPID=" + key;
  Serial.println(current);
  printWifi();
  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;
    http.begin(current); //Specify the URL
    int httpCode = http.GET();  //Make the request
    String payload;
    if (httpCode > 0) { //Check for the returning code
      String payload = http.getString();
      int i = payload.indexOf("\"description\"");
      int j = payload.indexOf("\"dt_txt\"");
      String reduzido = payload.substring(i + 15, j + 30);
      Serial.println(reduzido);
      i = reduzido.indexOf("\"");
      String description = reduzido.substring(0, i);
      description = description.substring(0, i);
      Serial.println("previsao description=" + description);

      i = reduzido.indexOf("\"dt_txt\"");
      String horas = reduzido.substring(i + 10 + 11);
      i = horas.indexOf("\"");
      horas = horas.substring(0, i - 3);
      Serial.println("horas=" + horas);

      tft.setFont(&OpenSansBold9pt7b);
      tft.setCursor(10, 245);
      tft.fillRect(0, 245 - 13, 260, 18, ILI9341_BLACK);
      description.replace("é", "e");
      description.replace("ç", "c");
      description.replace("ã", "a");
      String previsao = horas + " " + description;
      if (previsao_atual == "x") {
        previsao_atual = previsao;
        previsao_anterior = previsao + ".";
      }

      if (previsao != previsao_atual) { // mudou
        previsao_anterior = previsao_atual;
        previsao_atual = previsao;
      }
      tft.print(previsao_anterior);
    } else {
      Serial.println("HTTP request error");
    }
    http.end();
  }
}

void trocaPrevisao() {
    tft.setFont(&OpenSansBold9pt7b);
    tft.setCursor(10, 245);
    tft.fillRect(0, 245 - 13, 240, 18, ILI9341_BLACK);
    String p = previsao_atual;
    previsao_atual = previsao_anterior;
    previsao_anterior = p;
    tft.print(previsao_anterior);   
}

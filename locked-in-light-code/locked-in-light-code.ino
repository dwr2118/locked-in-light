/**************************************************************************
HTTP Request Example for Lab 3b: Web APIs
Fetch a random breaking bad quote from the Breaking Bad quotes API and 
allow users to click a button to reveal the author of the quote. 

To reveal the author, press the "left" button on the LILYGO (GPIO 0). 
To fetch a new quote, press the "right" button on the LILYGO (GPIO 35).
**************************************************************************/
#include "TFT_eSPI.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <Simple_HCSR04.h>

// TODO: replace with your own SSID & Password
const char* ssid = "Barnard Guest";
const char* password = "";

#define BUTTON_LEFT 0
#define BUTTON_RIGHT 35
#define ECHO_PIN 26
#define TRIG_PIN 27

volatile bool leftButtonPressed = false;
volatile bool rightButtonPressed = false;

String quoteString = " ";
String authorString = " ";

TFT_eSPI tft = TFT_eSPI();
Simple_HCSR04 *sensor;

void setup() {
  Serial.begin(115200);

  sensor = new Simple_HCSR04(ECHO_PIN, TRIG_PIN);
  
  // setup our display
  tft.init();
  tft.setRotation(1);  // 1 = landscape, 2 = portrait
  tft.fillScreen(TFT_BLACK);
  tft.setTextWrap(true);
  tft.setTextSize(1);

  Serial.print("display dimensions: ");
  Serial.print(tft.width());
  Serial.print(" x ");
  Serial.println(tft.height());

  // connect to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // Send the HTTP GET request
  if (WiFi.status() == WL_CONNECTED) {
    fetchQuotes();
  } else {
    Serial.println("WiFi not connected");
  }

  // setup our buttons
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), pressedLeftButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), pressedRightButton, FALLING);
}

void fetchQuotes() {

    // prep for the next HTTP request to be displayed 
    tft.fillScreen(TFT_BLACK);

    // create your HTTP request
    HTTPClient http;

    // store the URL that will be used for the request 
    String url = "https://api.breakingbadquotes.xyz/v1/quotes";
    http.begin(url);

    // Send the GET request
    int httpResponseCode = http.GET();

    // Check the response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
      JSONVar responseJSON = JSON.parse(response);

      if (JSON.typeof(responseJSON) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }

      JSONVar responseObject = responseJSON[0]; 
      
      // grab the quotes & author strings to be displayed at different times 
      quoteString = JSON.stringify(responseObject["quote"]);
      authorString = JSON.stringify(responseObject["author"]);
      Serial.print("Quote: ");
      Serial.println(quoteString);

      Serial.print("Author: ");
      Serial.println(authorString);

      // remove the quotes from around the author and add a hyphen at the beginning 
      authorString = "- " + authorString.substring(1, authorString.length() - 1);

      // display the quote without the author 
      tft.setTextColor(TFT_WHITE);
      tft.setTextWrap(true, true);
      tft.setCursor(0, 0);
      tft.print(quoteString);
      
    } else {
      Serial.println("Error on sending POST request");
    }

    // Free resources
    http.end();
}

void pressedLeftButton() {
  leftButtonPressed = true;
}

void pressedRightButton() {
  rightButtonPressed = true;
}

void loop() {

  unsigned long distance = sensor->measure()->cm();
  
  // reveal who said this quote by pressing the left button 
  if (leftButtonPressed) {
    tft.setTextColor(TFT_WHITE);
    tft.setTextWrap(true, true);
    
    // position the revealed author at the bottom right of the landscape ESP32 
    tft.setCursor(0, tft.height() - tft.fontHeight()*2);
    tft.print(authorString);
    leftButtonPressed = false;
  }

  // generate another breaking bad code from the API and display it 
  if (distance < 25 && distance > 0) {
    Serial.print("distance: ");
    Serial.print(distance);
    Serial.print("cm\n");
    fetchQuotes();
    rightButtonPressed = false;
  }
}

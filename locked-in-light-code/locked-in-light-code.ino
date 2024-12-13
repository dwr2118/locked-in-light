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
const char* ssid = "Columbia University";
const char* password = "";

#define BUTTON_LEFT 0
#define BUTTON_RIGHT 35
#define BUTTON 37
#define ECHO_PIN 27
#define TRIG_PIN 26

volatile bool leftButtonPressed = false;
volatile bool rightButtonPressed = false;
volatile bool buttonPressed = false;

// String quoteString = " ";
// String authorString = " ";
String colorString = " ";
String taskString = " ";
String prevColor = " ";
String prevTask = " ";

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
  tft.setTextSize(2);

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
    fetchStatus();
  } else {
    Serial.println("WiFi not connected");
  }

  // setup our buttons
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_LEFT), pressedLeftButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BUTTON_RIGHT), pressedRightButton, FALLING);
  
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), pressedButton, FALLING);
}

void fetchStatus() {

    // create your HTTP request
    HTTPClient http;

    // store the URL that will be used for the request 
    // String url = "https://api.breakingbadquotes.xyz/v1/quotes";
    String url = "https://52f3-209-2-231-37.ngrok-free.app/color";
    http.begin(url);

    // Send the GET request
    int httpResponseCode = http.GET();

    prevColor = colorString;
    prevTask = taskString; 

    // Check the response
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
      JSONVar responseJSON = JSON.parse(response);

      if (JSON.typeof(responseJSON) == "undefined") {
        Serial.println("Parsing input failed!");
        return;
      }
      
      // grab the quotes & author strings to be displayed at different times 
      // quoteString = JSON.stringify(responseObject["quote"]);
      // authorString = JSON.stringify(responseObject["author"]);
      // Serial.print("Quote: ");
      // Serial.println(quoteString);

      // Serial.print("Author: ");
      // Serial.println(authorString);

      // grab the color from the "color" key in the JSON object
      // JSONVar responseObject = responseJSON[0]; 
      colorString = JSON.stringify(responseJSON["color"]);
      taskString = JSON.stringify(responseJSON["task"]);
      colorString.replace("\"","");
      Serial.println(colorString);
      taskString.replace("\"","");
      Serial.println(taskString);
      // statusString = JSON.stringify(responseObject["task"]);

      // remove the quotes from around the author and add a hyphen at the beginning 
      // authorString = "- " + authorString.substring(1, authorString.length() - 1);

      // prep for the next HTTP request to be displayed 
      tft.setTextColor(TFT_WHITE);

      // Only update the screen if there's been a change in color
      if ( prevColor != colorString || prevTask != taskString){
        if (colorString == "RED"){
          tft.fillScreen(TFT_RED);
        } else if (colorString == "YELLOW"){
          tft.setTextColor(TFT_BLACK);
          tft.fillScreen(TFT_YELLOW);
        } else if (colorString == "GREEN"){
          tft.fillScreen(TFT_GREEN);
        } else {
          tft.fillScreen(TFT_BLACK);
        }
        
        tft.setTextWrap(true, true);
        // tft.setCursor(0, 0);
        //tft.print(colorString);
        
        // print the task on a new line 
        tft.setCursor(0, 20);
        tft.print(taskString);
      } else {

        // Need some sort of feedback if the button is pressed 
        Serial.println("No color change");
      }
      
    } else {
      Serial.println("Error on sending GET request");
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

void pressedButton() {
  buttonPressed = true;
}

void loop() {

  unsigned long distance = sensor->measure()->cm();

  // generate another breaking bad code from the API and display it 
  if (distance < 25 && distance > 0) {
    Serial.print("distance: ");
    Serial.print(distance);
    Serial.print("cm\n");
    fetchStatus();
  }

  if (buttonPressed){
    Serial.println("Button has been pressed.");
    fetchStatus();
    buttonPressed = false; 
  }
}

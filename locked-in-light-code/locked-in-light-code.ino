/**************************************************************************
HTTP Request to Locked-in-light API for my final project. 
Fetch the current working status and task that the user has told the
API they are doing. 

To fetch the GET request, stand in front of the sensor or simply press the button
to override this sensor. 
**************************************************************************/
#include "TFT_eSPI.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <Simple_HCSR04.h>

// TODO: replace with your own SSID & Password
const char* ssid = "Columbia University";
const char* password = "";

#define BUTTON 37
#define ECHO_PIN 27
#define TRIG_PIN 26

volatile bool buttonPressed = false;

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

  // setup the button
  pinMode(BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON), pressedButton, FALLING);
}

void fetchStatus() {

    // create your HTTP request
    HTTPClient http;

    // store the URL that will be used for the request 
    // the URL on ngrok changes on every launch! 
    String url = "https://17f0-209-2-231-37.ngrok-free.app/color";
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

      // grab the color from the "color" key in the JSON object
      colorString = JSON.stringify(responseJSON["color"]);
      taskString = JSON.stringify(responseJSON["task"]);
      colorString.replace("\"","");
      Serial.println(colorString);
      taskString.replace("\"","");
      Serial.println(taskString);
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
        
        tft.setTextWrap(false, false);
        drawWrappedText(tft, taskString, 1, 20, 220);
        // print the task on a new line 
        // tft.setCursor(0, 20);
        // tft.print(taskString);
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

void pressedButton() {
  buttonPressed = true;
}

void drawWrappedText(TFT_eSPI &tft, const String &text, int x, int y, int maxWidth) {
  tft.setCursor(x, y);
  int cursorX = x;
  int cursorY = y;

  String word = "";  // Buffer to store the current word
  for (int i = 0; i <= text.length(); i++) {
    char currentChar = text[i];
    int wordWidth = tft.textWidth(word);

    // Check if the word fits in the current line or if it's a space
    if (currentChar == ' ' || currentChar == '\0') {
      // If adding the word exceeds the line width, wrap to the next line
      if (cursorX + wordWidth > maxWidth) {
        cursorX = x;  // Reset X to the starting X-coordinate
        cursorY += tft.fontHeight();  // Move down by the height of a line
      }

      // Print the word
      tft.setCursor(cursorX, cursorY);
      tft.print(word + " ");  // Add the space back after the word
      cursorX += wordWidth + tft.textWidth(" ");  // Advance cursor X position
      word = "";  // Clear the word buffer
    } else {
      // Add the current character to the word buffer
      word += currentChar;
    }

    // If the word itself exceeds the max width, force-wrap it
    if (tft.textWidth(word) > maxWidth) {
      tft.setCursor(cursorX, cursorY);
      tft.print(word);
      cursorX = x;
      cursorY += tft.fontHeight();
      word = "";  // Reset the word buffer
    }
  }
}


void loop() {

  unsigned long distance = sensor->measure()->cm();

  // generate a GET request if there's someone within 25cm of the sensor 
  if (distance < 25 && distance > 0) {
    Serial.print("distance: ");
    Serial.print(distance);
    Serial.print("cm\n");
    fetchStatus();
  }


  // generate a GET request if the button was pressed (override for errors in ultrasonic sensor)
  if (buttonPressed){
    Serial.println("Button has been pressed.");
    fetchStatus();
    buttonPressed = false; 
  }
}

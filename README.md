# locked-in-light
COMS BC3930 Creative Embedded Systems Final Project. 

## What you will need:
1. A LilyGo T-Display board, ultrasonic distance sensor hc sr04 and a push button. 
2. Install the correct [driver](https://github.com/Xinyuan-LilyGO/TTGO-T-Display) for the LilyGo T-Display.
3. USB-C cable with working data connections.
4. Install the [Arduino IDE](https://www.arduino.cc/en/software)

## Arduino IDE Setup:
1. Download the TFT_eSPI library by Bodmer:
    - Open Arduino IDE
    - Click Tools > Manage Libraries
    - Search for "tft_espi" by Bodmer and click install.
    - Navigate to the library on your computer's file manager (e.g., Documents/Arduino/libraries/tft_espi
    - Open the file "User_Setup_Select.h"
    - Comment out "#include <User_setup.h>" near the top of the page
    - Uncomment "#include <User_Setups/Setup25_TTGO_T_Display.h>" further down the page (the lines are in numeric order).

2. Install the ESP 32 Board Library:
    - We will install it through Arduino IDE.
    - First, go to [this website](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) and under "Installing using Arduino IDE" copy the "Stable release link".
    - Open Arduino IDE.
    - Click File > Preferences
    - At the bottom of the "Settings" tab, in "Additional boards manager URLs", paste the stable release link in the text box, click OK.
    - Click Tools > Board > Boards Manager
    - Search for 'esp32' by Espressif and click Install
    - Download the Simple HCSR04 library too. 
      
## How to recreate this project:
1. After setting up, connect your board to your computer with the USB-C cable.
2. Open Arduino IDE. Click the upper-left dropdown menu to select your Board and Port. It will usually pop-up as "LilyGo T-Display" on port "COM xx".
4. Open 'locked-in-light-code.ino' and upload it to your ESP32 device
5. To get the Locked-in-Light API to work on your local computer:
    - go to the root of the web-app directory 
    - Download the corresponding dependencies using `npm install`
    - use `netlify dev` to run the app locally and use `ngrok http http://localhost:8888` to expose this development environment using ngrok
    - grab the forwarding URL from ngrok on your terminal and input this link into the index.html, arduino file 
    - travel to the ngrok URL/color route to access the website's UI
    - try to change the color of your current status by pressing one of the buttons or inputting a new task by typing into the text box and hitting enter
6. Try out the locked in lights yourself! 

## Design Documentation: 
Read more about my creative process [here](https://denim-duckling-124.notion.site/Module-4-Locked-in-Lights-1487d975e60580e4acc5dc75ee4d8802?pvs=4). 
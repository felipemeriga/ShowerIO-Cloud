# ShowerIO-Cloud
Smart-Shower This is an application build on a ESP8622 where you can set how much time you can use your shower. With the help of a background electrical circuito you can set up these variables. Build in C++ in the backend, and AngularJS in the front.  The core chip of this application is the ESP8622 how to install this chip in the Arduino IDE and how to sync it with your pc you can enter on the repository: https://github.com/esp8266/Arduino  All the dependencies of the project like the javascript codes, HTML and CSS files, will have to be uploaded into ESP8266 with the help of the FS library for arduino: http://esp8266.github.io/Arduino/versions/2.0.0/doc/filesystem.html.  The core of the HTTP requisitions are handled by the ESP8266Webserver library: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/src/ESP8266WebServer.h

# ESP8266 Requeriments

- Arduino IDE has to be in 1.8.5 version
- ESP8266 version in Boards Manager has to be 2.4.0
- Dependencies are located in /ESP8266/Project/Dependencies

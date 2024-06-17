

// Load Wi-Fi library
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <arduinoFFT.h>

#include <driver/i2s.h>
#define I2S_WS 15
#define I2S_SD 32
#define I2S_SCK 14
#define I2S_PORT I2S_NUM_0
#define bufferLen 128

#define PIN 16
#define PIXELCOUNT 105
#define PIXELCOUNT2 210
#define MAX_INPUT 400
// Replace with your network credentials
const char* ssid = "";
const char* password = "";

int r=255;
int g=255;
int b=255;
uint8_t reds[PIXELCOUNT] = {0, 20, 25, 30, 35, 40, 50, 60, 70, 70, 80, 80, 90, 110, 130, 150, 170, 190, 200, 220, 240, 250, 255, 255, 255, 255 };
int16_t sBuffer[bufferLen];
//frequencyanalyzer
int f=0;
const int BLOCK_SIZE = 512;
int hoehe=0;
int bands[3] = {0, 0, 0};

const double signalFrequency = 1000;
const double samplingFrequency = 10000;
const uint8_t amplitude = 150;

double vReal[BLOCK_SIZE];
double vImag[BLOCK_SIZE];
int32_t samples[BLOCK_SIZE];

String labels[] = {"125", "250", "500", "1K", "2K", "4K", "8K", "16K"};

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;


Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELCOUNT, PIN, NEO_RGB + NEO_KHZ800);


// Auxiliar variables to store the current output state
String LEDState = "off";

// Assign output variables to GPIO pins

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  strip.begin();
  strip.setBrightness(50);  
              for (int i=0; i<PIXELCOUNT2; i++){
            strip.setPixelColor(i, g, r, b); 
            }
            strip.show();
  // Connect to Wi-Fi network with SSID and password
  Serial.begin(115200);
  Serial.println("Setup I2S ...");
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(500);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void update(){
  if(LEDState=="stable"){
  for (int i=0; i<PIXELCOUNT2; i++){
            strip.setPixelColor(i, g,r,b); 
            }
            strip.show();
    if(g==0&&b==0&&r==0){
        for (int i=0; i<PIXELCOUNT2; i++){
            strip.setPixelColor(i, 30,30,30); 
            }
            strip.show();
    }
  }else{
      LEDState="off";
              for (int i=0; i<PIXELCOUNT2; i++){
            strip.setPixelColor(i, 20,20,20); 
            }
      strip.show();
  }
}

void disco(){
  for (int j=0; j<=10; j++){
  for (int i=0; i<PIXELCOUNT2; i++){
    strip.setPixelColor(i, random(0, 255),random(0, 255),random(0, 255)); 
  }
  strip.show();
  delay(50);
  }
}
void discoinfinite(){
  while(1){
    disco();
  }
}

void chase(){
  for (int j=0; j<=2; j++){
      for (int i = 0; i < PIXELCOUNT; i++) {
      strip.setPixelColor(i, g, r, b);
        if(i>=5){
          strip.setPixelColor(i-5, 0, 0, 0);
        }else {
          strip.setPixelColor(PIXELCOUNT-i, 0, 0, 0);
        }
      strip.setPixelColor(210-i, g, r, b);
        if(i<=205){
          strip.setPixelColor(i+5, 0, 0, 0);
        }else {
          strip.setPixelColor(PIXELCOUNT-i, 0, 0, 0);
        } 
      strip.show();
      delay(30);
      }
  }
  for (int i=0; i<PIXELCOUNT2; i++){
            strip.setPixelColor(i, 20,20,20); 
            }
      strip.show();
}

void chaseinfinite(){
  while(1){
    chase();
  }
}

void funkeln(){
  for (int i=0; i<950; i++){
    strip.setPixelColor(random(0,210), g, r, b);
    strip.show();
    delay(5);
  }
}

void mic(){
  while(1){
    size_t bytesIn = 0;
  esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
  if (result == ESP_OK) {
    int samples_read = bytesIn / 8;
    if (samples_read > 0) {
      float mean = 0;
      for (int i = 0; i < samples_read; ++i) {
        mean += (sBuffer[i]);
      }
      mean /= samples_read;
      int maxLed = map((long)abs(mean), 10, MAX_INPUT, 0, PIXELCOUNT);
      //  Serial.println(maxLed);
      for (int i = 0; i < maxLed; i++) {
        uint8_t red = constrain(15*i, 0, 255);
        uint8_t green = constrain(255 - 12*i, 0, 255);
        uint32_t color = (red << 16) | (green << 8);
        strip.setPixelColor(i, color);
        strip.show();
      }
    }
  }

  strip.fill(0x000000, 0, PIXELCOUNT2);
  strip.show();
  }
}
//show
void show(int a){
  for (int i=0; i<(a/2000000); i++){
    strip.setPixelColor(i, g, r, b);
  }
  strip.show();
  delay(10);
}
/*
void mic1(){
  int num_bytes_read = i2s_read_bytes(I2S_PORT, 
                                      (char *)samples, 
                                      BLOCK_SIZE,     // the doc says bytes, but its elements.
                                      portMAX_DELAY); // no timeout

  for (uint16_t i = 0; i < BLOCK_SIZE; i++) {
    vReal[i] = samples[i] << 8;
    vImag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
  }

  FFT.Windowing(vReal, BLOCK_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, BLOCK_SIZE, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, BLOCK_SIZE);
  
  for (int i = 2; i < (BLOCK_SIZE/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
    if (vReal[i] > 2000) { // Add a crude noise filter, 10 x amplitude or more
      if (i<=2 )             Serial.println(125); // 125Hz
      if (i >3   && i<=5 )   Serial.println(250); // 250Hz
      if (i >5   && i<=7 )   Serial.println(500); // 500Hz
      if (i >7   && i<=15 )  Serial.println(1000); // 1000Hz
      if (i >15  && i<=30 )  Serial.println(2000); // 2000Hz
      if (i >30  && i<=53 )  Serial.println(4000); // 4000Hz
      if (i >53  && i<=200 ) Serial.println(8000); // 8000Hz
      if (i >200           ) Serial.println(16000); // 16000Hz
    }

    //for (byte band = 0; band <= 6; band++) display.drawHorizontalLine(18*band,64-peak[band],14);
  }
}
*/

void filter(){
  size_t bytesIn = 0;
  esp_err_t result = i2s_read(I2S_PORT, &sBuffer, bufferLen, &bytesIn, portMAX_DELAY);
  for (uint16_t i = 0; i < BLOCK_SIZE; i++) {
        vReal[i] = samples[i] << 8;
        vImag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
      }
  if (result == ESP_OK) {
    int samples_read = bytesIn / 8;
    if (samples_read > 0) {
    FFT.Windowing(vReal, BLOCK_SIZE, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, BLOCK_SIZE, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, BLOCK_SIZE);
    for (int i = 0; i < 3; i++) {
    bands[i] = 0;
  }
    for (int i = 2; i < (BLOCK_SIZE/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
      if (vReal[i] > 2000) { // Add a crude noise filter, 10 x amplitude or more
        if (i<=2 && i<=7 && hoehe==1){
          bands[0] = max(bands[0], (int)(vReal[i]/amplitude));
          show(bands[0]); // 125-500Hz
        }     
        if (i >7   && i<=30 && hoehe==2)  {
          bands[1] = max(bands[1], (int)(vReal[i]/amplitude));
          show(bands[1]); // 1000-2000Hz
        }
        if (i >30  && i<=53 && hoehe==3){
          bands[2] = max(bands[2], (int)(vReal[i]/amplitude));
          show(bands[2]); // 4000-16000Hz
        }
     }
    }
    }
  }
}


void i2s_install() {
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 5120,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,  // default interrupt priority
    .dma_buf_count = 8,
    .dma_buf_len = bufferLen,
    .use_apll = false
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

void i2s_setpin() {
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);
}



void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            //Gruen
            if (header.indexOf("GET /Gruen/on") >= 0) {
              Serial.println("Gruen an");
              g=255;
              if (LEDState=="stable"||LEDState=="off"){
              update();
              }else if(LEDState=="Chase on"){
                chase();
              }else if(LEDState=="Funkeln on"){
                funkeln();
              }
            } else if (header.indexOf("GET /Gruen/off") >= 0) {
              Serial.println("Gruen aus");
              g=0;
              if (LEDState=="stable"||LEDState=="off"){
              update();
              }else if(LEDState=="Chase on"){
                chase();
              }else if(LEDState=="Funkeln on"){
                funkeln();
              }
            }
            //rot
            if (header.indexOf("GET /Rot/on") >= 0) {
              Serial.println("Rot an");
              r=255;
              if (LEDState=="stable"||LEDState=="off"){
              update();
              }else if(LEDState=="Chase on"){
                chase();
              }else if(LEDState=="Funkeln on"){
                funkeln();
              }
            } else if (header.indexOf("GET /Rot/off") >= 0) {
              Serial.println("Rot aus");
              r=0;
              if (LEDState=="stable"||LEDState=="off"){
              update();
              }else if(LEDState=="Chase on"){
                chase();
              }else if(LEDState=="Funkeln on"){
                funkeln();
              }
            }
            //blau
            if (header.indexOf("GET /Blau/on") >= 0) {
              Serial.println("Blau an");
              b=255;
              if (LEDState=="stable"||LEDState=="off"){
              update();
              }else if(LEDState=="Chase on"){
                chase();
              }else if(LEDState=="Funkeln on"){
                funkeln();
              }
            } else if (header.indexOf("GET /Blau/off") >= 0) {
              Serial.println("Blau aus");
              b=0;
              if (LEDState=="stable"||LEDState=="off"){
              update();
              }else if(LEDState=="Chase on"){
                chase();
              }else if(LEDState=="Funkeln on"){
                funkeln();
              }
            }
            // turns the GPIOs on and off
            if (header.indexOf("GET /LED/on") >= 0) {
              Serial.println("LED stable");
              LEDState="stable";
            for (int i=0; i<PIXELCOUNT2; i++){
            strip.setPixelColor(i, g,r,b); 
            }
            strip.show();
            } else if (header.indexOf("GET /LED/off") >= 0) {
              Serial.println("LED off");
              LEDState="off";
              for (int i=0; i<PIXELCOUNT2; i++){
            strip.setPixelColor(i, 20,20,20); 
            }
            strip.show();
            }
            //chase
            if (header.indexOf("GET /Chase/on") >= 0) {
              Serial.println("Chase on");
              LEDState="Chase on";
              chase();
            strip.show();
            } else if (header.indexOf("GET /Chase/off") >= 0) {
              Serial.println("Chase off");
              update();
            }
            //chaseinfinite
            if (header.indexOf("GET /ChaseInfinite/on") >= 0) {
              Serial.println("Chase Infinite on");
              LEDState="Chase Infinite on";
              chaseinfinite();
            strip.show();
            }
            //disco
            if (header.indexOf("GET /Disco/on") >= 0) {
              Serial.println("Disco on");
              LEDState="Disco on";
              disco();
            strip.show();
            } else if (header.indexOf("GET /Disco/off") >= 0) {
              Serial.println("Disco off");
              update();
            }
            //mic
            if (header.indexOf("GET /Mic/on") >= 0) {
              Serial.println("Mic on");
              LEDState="Mic on";
              mic();
            } else if (header.indexOf("GET /Mic/off") >= 0) {
              Serial.println("Mic off");
              update();
            }
            //filter
            if (header.indexOf("GET /Filter/on") >= 0) {
              Serial.println("Filter on");
              LEDState="Filter on";
              filter();
            } else if (header.indexOf("GET /Filter/off") >= 0) {
              Serial.println("Filter off");
              update();
            }
            //Hoehen
            //h1
            if (header.indexOf("GET /h1/on") >= 0) {
              Serial.println("Tieffrequenz on");
              hoehe=1;
              filter();
            } else if (header.indexOf("GET /h1/off") >= 0) {
              Serial.println("Filter off");
              hoehe=0;
              update();
            }
            //h2
            if (header.indexOf("GET /h2/on") >= 0) {
              Serial.println("Mittelfrequenz on");
              hoehe=2;
              filter();
            } else if (header.indexOf("GET /h2/off") >= 0) {
              Serial.println("Mittelfrequenz off");
              hoehe=0;
              update();
            }
            //h3
            if (header.indexOf("GET /h3/on") >= 0) {
              Serial.println("Hochfrequenz on");
              hoehe=3;
              filter();
            } else if (header.indexOf("GET /h3/off") >= 0) {
              Serial.println("Hochfrequenz off");
              hoehe=0;
              update();
            }
            //DiscoInfinite
            if (header.indexOf("GET /DiscoInfinite/on") >= 0) {
              Serial.println("Disco Infinite on");
              LEDState="Disco Infinite on";
              discoinfinite();
            strip.show();
            }
            //funkeln
            if (header.indexOf("GET /Funkeln/on") >= 0) {
              Serial.println("Funkeln on");
              LEDState="Funkeln on";
              funkeln();
            strip.show();
            } else if (header.indexOf("GET /Funkeln/off") >= 0) {
              Serial.println("Funkeln off");
              update();
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #FFD700; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttong1 {background-color: #008B00; border: none; color: white; padding: 16px 40px}</style></head>");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonr1 {background-color: #EE0000; border: none; color: white; padding: 16px 40px;}</style></head>");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonb1 {background-color: #0000FF; border: none; color: white; padding: 16px 40px;}</style></head>"),
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonchase {background-color: #FF8247; border: none; color: white; padding: 16px 40px;}</style></head>"),
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttondisco {background-color: #FF34B3; border: none; color: white; padding: 16px 40px;}</style></head>"),
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonmic {background-color: #66CDAA; border: none; color: white; padding: 16px 40px;}</style></head>"),
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonf {background-color: #66CDAA; border: none; color: white; padding: 16px 40px;}</style></head>"),
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonh1 {background-color: #66CDAA; border: none; color: white; padding: 16px 40px;}</style></head>"),
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonh2 {background-color: #66CDAA; border: none; color: white; padding: 16px 40px;}</style></head>"),
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonh3 {background-color: #66CDAA; border: none; color: white; padding: 16px 40px;}</style></head>"),
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonfunkeln {background-color: #828282; border: none; color: white; padding: 16px 40px;}</style></head>"),
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttonchasei {background-color: #DB7093; border: none; color: white; padding: 16px 40px;}</style></head>"),
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".buttondiscoi {background-color: #8B1C62; border: none; color: white; padding: 16px 40px;}</style></head>"),
            
            
            // Web Page Heading
            client.println("<body><h1>LED Control Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>LED - State " + LEDState + "</p>");
            // If the output26State is off, it displays the ON button       
            if (LEDState=="off") {
              client.println("<p><a href=\"/LED/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/LED/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
            client.println("</body></html>");
            //Gruen
            client.println("<p>Gruen</p>");
            if (g==0) {
              client.println("<p><a href=\"/Gruen/on\"><button class=\"buttong1\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Gruen/off\"><button class=\"buttong1 button2\">OFF</button></a></p>");
            } 
            client.println("</body></html>");
            //rot
            client.println("<p>Rot</p>");
            if (r==0) {
              client.println("<p><a href=\"/Rot/on\"><button class=\"buttonr1\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Rot/off\"><button class=\"buttonr1 button2\">OFF</button></a></p>");
            } 
            client.println("</body></html>");
            //blau
            client.println("<p>Blau</p>");
            if (b==0) {
              client.println("<p><a href=\"/Blau/on\"><button class=\"buttonb1\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/Blau/off\"><button class=\"buttonb1 button2\">OFF</button></a></p>");
            } 
            client.println("</body></html>");   
            //Effekte:
            client.println("<p>Effekte:</p>");
            //chase
            client.println("<p>Chase</p>");
            if (LEDState=="Chase on") {
              client.println("<p><a href=\"/Chase/off\"><button class=\"buttonchase button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/Chase/on\"><button class=\"buttonchase\">ON</button></a></p>");
            } 
            client.println("</body></html>"); 
            //disco
            client.println("<p>Disco</p>");
            if (LEDState=="Disco on") {
              client.println("<p><a href=\"/Disco/off\"><button class=\"buttondisco button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/Disco/on\"><button class=\"buttondisco\">ON</button></a></p>");
            } 
            client.println("</body></html>");   
            //mic
            client.println("<p>Mikrophon</p>");
            if (LEDState=="Mic on") {
              client.println("<p><a href=\"/Mic/off\"><button class=\"buttonmic button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/Mic/on\"><button class=\"buttonmic\">ON</button></a></p>");
            } 
            client.println("</body></html>");
            //filter
            client.println("<p>Filter</p>");
            if (LEDState=="Filter on") {
              client.println("<p><a href=\"/Filter/off\"><button class=\"buttonf button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/Filter/on\"><button class=\"buttonf\">ON</button></a></p>");
            } 
            client.println("</body></html>");
            //hoehen
            //h2
            client.println("<p>Tieffrequenz</p>");
            if (LEDState=="Tieffrequenz on") {
              client.println("<p><a href=\"/h1/off\"><button class=\"buttonh1 button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/h1/on\"><button class=\"buttonh1\">ON</button></a></p>");
            } 
            client.println("</body></html>");
            //h2
            client.println("<p>Mittelfrequenz</p>");
            if (LEDState=="Mittelfrequenz on") {
              client.println("<p><a href=\"/h2/off\"><button class=\"buttonh2 button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/h2/on\"><button class=\"buttonh2\">ON</button></a></p>");
            } 
            client.println("</body></html>");
            //h3
            client.println("<p>Hochfrequenz</p>");
            if (LEDState=="Hochfrequenz on") {
              client.println("<p><a href=\"/h3/off\"><button class=\"buttonh3 button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/h3/on\"><button class=\"buttonh3\">ON</button></a></p>");
            } 
            client.println("</body></html>");
            //funkeln
            client.println("<p>Funkeln</p>");
            if (LEDState=="Funkeln on") {
              client.println("<p><a href=\"/Funkeln/off\"><button class=\"buttonfunkeln button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/Funkeln/on\"><button class=\"buttonfunkeln\">ON</button></a></p>");
            } 
            client.println("</body></html>");     
            //chaseinfinite
            client.println("<p>Chase Infinite</p>");
            if (LEDState=="Chase Infinite") {
              client.println("<p><a href=\"/ChaseInfinite/off\"><button class=\"buttonchasei button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/ChaseInfinite/on\"><button class=\"buttonchasei\">ON</button></a></p>");
            } 
            client.println("</body></html>"); 
            //discoinfinite
            client.println("<p>Disco Infinite</p>");
            if (LEDState=="Disco Infinite") {
              client.println("<p><a href=\"/DiscoInfinite/off\"><button class=\"buttondiscoi button2\">OFF</button></a></p>");
            } else {
              client.println("<p><a href=\"/DiscoInfinite/on\"><button class=\"buttondiscoi\">ON</button></a></p>");
            } 
            client.println("</body></html>");  
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

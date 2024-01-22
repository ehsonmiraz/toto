#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "network_param.h"
#include "CloudSpeechClient.h"
#include "Audio.h"

#define uart_en 15
#define RXp2 16
#define TXp2 17
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

//Variables
// Replace with your network credentials
const char* WIFI_SSID     = "Ehson Lappy";
const char* WIFI_PASSWORD = "12345678";

// Replace with your OpenAI API key

//int i = 0;
//int statusCode;
//const char* ssid = "Default SSID";
//const char* passphrase = "Default passord";
//String st;
//String content;
//String esid;
//String epass = "";
HTTPClient http;
//HTTPClient http;


void setup(){
  // Initialize Serial
  Serial.begin(9600);

  // Connect to Wi-Fi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
  }
  Serial.println(WiFi.localIP());

  // Send request to OpenAI API
  
  
  http.begin(apiUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + String(AI_API));

  // audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  // audio.setVolume(100);
  // audio.connecttospeech("Starting ", "en"); // Google TTS

 
}
String retrieveInput(){
    // while (Serial.available() == 0);
    // String  inputText = Serial.readString(); //Reading the Input string from Serial port.
    // Serial.println("Input recieved: "+inputText);
    // return inputText;
    String inputText="";
    Serial.println("\r\nRecord start!\r\n");
  //Serial2.println("\r\nRecord start!\r\n");
    Audio* audio = new Audio(ADMP441);
 //Audio* audio = new Audio(M5STACKFIRE);
  audio->Record();
  Serial.println("Recoding Complited Processing");
  // digitalWrite(led_1,0);
  // digitalWrite(led_3,0);
  // digitalWrite(led_2,1);
  CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient(USE_APIKEY);
  cloudSpeechClient->Transcribe(audio);
  delete cloudSpeechClient;
  delete audio;
  return inputText;
}
String generateErrorMesssage(int errorCode){
  Serial.printf("Error %i \n", errorCode);
  return "unable to understand";
}
String decode(String response){
          DynamicJsonDocument jsonDoc(1024);
          deserializeJson(jsonDoc, response);
          String outputText = jsonDoc["choices"][0]["text"];
          return outputText;
}
String sendQuery(String userTranscript){
    String prompt=promptMessage  +userTranscript;
    String payload = "{\"prompt\":\"" + prompt + "\",\"max_tokens\":100, \"model\": \"text-davinci-003\"}";
    int httpResponseCode = http.POST(payload);
    String outputText="Unable to understand";
  
    if (httpResponseCode == 200) {
          String response = http.getString();
          outputText=decode(response);
          
  } else {
          outputText=generateErrorMesssage(httpResponseCode);
  }
  return outputText;

}

void provideOutput(String response){
    Serial.println(response);
}
void run(){
    String userTranscript=retrieveInput();
    
    String outputText=sendQuery(userTranscript);
    provideOutput(outputText);
}
void loop() {
  // do nothing
  run();   
}

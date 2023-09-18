#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <Servo.h>

const char* ssid = "Micromax-HS2";
const char* password = "Micromax";

WiFiClient client;
WiFiServer server(80);
String data = "";

Servo fl;
Servo fr;
Servo bl;
Servo br;

#define fl_pin D3
#define fr_pin D4
#define bl_pin D7
#define br_pin D8

int fl_pos = 0;
int fr_pos = 0;
int bl_pos = 0;
int br_pos = 0;
int fl_ppos = 0;
int fr_ppos = 0;
int bl_ppos = 0;
int br_ppos = 0;

#define pwm_1 D1
#define dir_1 D2
#define pwm_2 D5
#define dir_2 D6
int rpm = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(dir_1, OUTPUT);
  pinMode(dir_2, OUTPUT);
  pinMode(pwm_1, OUTPUT);
  pinMode(pwm_2, OUTPUT);

  fl.attach(fl_pin);
  fr.attach(fr_pin);
  bl.attach(bl_pin);
  br.attach(br_pin);

  fl.write(fl_pos);
  fr.write(fr_pos);
  bl.write(bl_pos);
  br.write(br_pos);

  Serial.begin(115200);
  Serial.println("Connecting to WIFI");
  WiFi.begin(ssid, password);
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(500);
    Serial.print("...");
  }
  Serial.println("WiFi connected");
  Serial.println("ESP32 Local IP is : ");
  Serial.print((WiFi.localIP()));
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  client = server.available();
    if (!client){
      return; 
      }
    data = checkClient();
    String dataVal = data.substring(1, data.length());
    String rt = data.substring(0, 1); 
    Serial.println(data); 

    if (rt == "f"){
      Serial.println("Forward");
      rpm = dataVal.toInt();
      
      forward(rpm);
    }
    else if (rt == "b"){
      Serial.println("Reverse");
      rpm = dataVal.toInt();
      reverse(rpm);
    }
    else if (rt == "l"){
      Serial.println("Left");
      rpm = dataVal.toInt();
      left(rpm);
    }    
    else if (rt == "r"){
      Serial.println("Right");
      rpm = dataVal.toInt();
      right(rpm);
    }
    else if (rt == "s"){
      Serial.println("Stop");
      brake();
    }

    else if(rt == "a"){
      Serial.println("Front Left Servo");
      fl_pos = dataVal.toInt();

      if (fl_pos > fl_ppos){
        for (int i = fl_ppos; i <= fl_pos; i++){
          fl.write(i);
          delay(20);
        }
      }
      else if(fl_pos < fl_ppos){
        for (int j = fl_ppos; j >= fl_pos; j--){
          fl.write(j);
          delay(20);
        }
      }
      fl_ppos = fl_pos;
    }

    else if(rt == "q"){
      Serial.println("Front Right Servo");
      fr_pos = dataVal.toInt();

      if (fr_pos > fr_ppos){
        for (int k = fr_ppos; k <= fr_pos; k++){
          fr.write(k);
          delay(20);
        }
      }
      else if(fr_pos < fr_ppos){
        for (int l = fr_ppos; l >= fr_pos; l--){
          fr.write(l);
          delay(20);
        }
      }
      fr_ppos = fr_pos;
    }

    else if(rt == "e"){
      Serial.println("Front Left Servo");
      bl_pos = dataVal.toInt();

      if (bl_pos > bl_ppos){
        for (int m = bl_ppos; m <= bl_pos; m++){
          bl.write(m);
          delay(20);
        }
      }
      else if(bl_pos < bl_ppos){
        for (int n = bl_ppos; n >= bl_pos; n--){
          bl.write(n);
          delay(20);
        }
      }
      bl_ppos = bl_pos;
    }

    else if(rt == "d"){
      Serial.println("Front Right Servo");
      br_pos = dataVal.toInt();

      if (br_pos > br_ppos){
        for (int o = br_ppos; o <= br_pos; o++){
          br.write(o);
          delay(20);
        }
      }
      else if(br_pos < br_ppos){
        for (int p = br_ppos; p >= br_pos; p--){
          br.write(p);
          delay(20);
        }
      }
      br_ppos = br_pos;
    }

}

String checkClient (void){
  while(!client.available()){
  delay(1); 
  Serial.println(".");}
  Serial.println("Client Found");
  String request = client.readStringUntil('\r');
  request.remove(0, 5);
  request.remove(request.length()-9,9);
  return request;
}

void forward(int rpm){
      analogWrite(pwm_1, rpm);
      analogWrite(pwm_2, rpm);
      digitalWrite(dir_1, HIGH);
      digitalWrite(dir_2, HIGH);
}

void reverse(int rpm){
      analogWrite(pwm_1, rpm);
      analogWrite(pwm_2, rpm);
      digitalWrite(dir_1, LOW);
      digitalWrite(dir_2, LOW);
}

void left(int rpm){
      analogWrite(pwm_1, rpm);
      analogWrite(pwm_2, rpm);
      digitalWrite(dir_1, HIGH);
      digitalWrite(dir_2, LOW);
}

void right(int rpm){
      analogWrite(pwm_1, rpm);
      analogWrite(pwm_2, rpm);
      digitalWrite(dir_1, LOW);
      digitalWrite(dir_2, HIGH);
}

void brake(){
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
}
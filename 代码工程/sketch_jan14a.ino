 #include <Arduino.h>
#include <WiFi.h>
#include "PubSubClient.h"

const char *ssid = "danke";
const char *pwd  = "12345678";
const char *mqtt_server = "5K43NA1J04.iotcloud.tencentdevices.com";
const char *mqtt_username = "5K43NA1J04ESP32_tem;12010126;X0L2C;1709985823";
const char *mqtt_userpwd  = "be50859e488c790e2f1ed16df63ff4f53dbfb44590b4ed291c428b8959ded110;hmacsha256";
const char *mqtt_clientid = "5K43NA1J04ESP32_tem";
const char *mqtt_pub_topic = "$thing/up/property/5K43NA1J04/ESP32_tem";
const char *mqtt_sub_topic = "$thing/down/property/5K43NA1J04/ESP32_tem";

#define REPORT_DATA_TEMPLATE "{\"method\":\"report\",\"clientToken\":\"123\",\"params\":{\"current_humidity\":%d,\"current_temp\":%d}}"

WiFiClient espClient;
PubSubClient mqttclient(espClient);

long lastMsg = 0;
int tem = 17;
int hum = 60;
char report_buf[1024];

void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("--->Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println();
  Serial.print("payload [");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup_wifi()
{
    Serial.printf("Connect to %s ", ssid);
    WiFi.begin(ssid, pwd);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.printf(".");
        delay(500);
    }
    Serial.println("Connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);
    setup_wifi();

    // connect mqtt server
    mqttclient.setServer(mqtt_server, 1883);
    mqttclient.setCallback(callback);
    mqttclient.setKeepAlive(65535);
    while (!mqttclient.connect(mqtt_clientid, mqtt_username, mqtt_userpwd)) {
        Serial.println("mqtt connect fail, reconnect");
        delay(2000);
    }
    Serial.println("mqtt connected!");
    
    // sub topic
    boolean ret = mqttclient.subscribe(mqtt_sub_topic);
    if (ret != true) {
        Serial.printf("mqtt subscribe topic [%s] fail\n", mqtt_sub_topic);
    }
    Serial.printf("mqtt subscribe topic [%s] ok\n", mqtt_sub_topic);
}

void loop() {
    // client loop
    mqttclient.loop();
        
    // pub topic
    long now = millis();
    if (now - lastMsg > 3000) {
      
        if(Serial2.available()){
          // 读取接收到的数据
          Serial.printf("get！！！！！！！！！！！！！！！！！！！！\n");  //收到消息 
          String data1 = Serial2.readStringUntil('\n');  // 读取串口接收到的数据，直到遇到换行符
          String data2 = Serial2.readStringUntil('\n');  // 读取串口接收到的数据，直到遇到换行符
          data1.trim();                      // 去除字符串两端的空格
          data2.trim();                      // 去除字符串两端的空格
          int numData1 = data1.toInt();
          int numData2 = data2.toInt();
          Serial.printf("[%d][%d]\n", numData1,numData2);
          tem = numData1;
          hum = numData2;
          
        }
        
        lastMsg = now;
        memset(report_buf, 0, 1024);
        sprintf(report_buf, REPORT_DATA_TEMPLATE, hum, tem);
        Serial.println(report_buf);

        if (mqttclient.publish(mqtt_pub_topic, report_buf)) {
            Serial.printf("mqtt publish topic [%s] ok\n", mqtt_pub_topic);
        } else {
            Serial.printf("mqtt publish topic [%s] fail\n", mqtt_pub_topic);
        }

    }
}


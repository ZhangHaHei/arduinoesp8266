#include <DHT.h> //温度传感器库文件
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial softSerial(6, 7);
#define irSensorPin A0    // 连接红外传感器引脚
#define flamePin A2       //定义火焰传感器接口
#define lightSensorPin A4 //光敏传感器引脚
#define atuoLedPin 2      // led引脚，测试专用
#define DHTPin 3          //温度传感器引脚
#define warnPin 4         //蜂鸣器引脚
#define fanPin 12         //风扇引脚
#define ledPin 13         //卧室灯
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPin, DHTTYPE); //初始化DHT类

bool fanState = false;
bool warnState = false;
bool ledState = false;
bool atuoLedState = false;
bool curState = false;
float Humidity = 0;
float Temperature = 0;
unsigned long lastmin = 10000;
unsigned long lasttm = 0;
void setup()
{
    Serial.begin(9600); //启动串口通讯
    //软件串口初始化
    softSerial.begin(9600);
    //监听软串口通信
    softSerial.listen();
    pinMode(irSensorPin, INPUT);
    pinMode(atuoLedPin, OUTPUT);
    pinMode(fanPin, OUTPUT); // fanPin 接口为输出接口
    pinMode(warnPin, OUTPUT);
    delay(3000);
    softSerial.println("ATE0");
    delay(500);
    if (softSerial.find("OK"))
    {
        Serial.println("ATE0设置");
    }
    softSerial.println("AT+TCDEVINFOSET=1,\"****",\"dev_1\",\"******"");
    delay(500);
    softSerial.println("AT+TCMQTTCONN=1,5000,240,0,1");
    delay(500);
    softSerial.println("AT+TCMQTTSUB=\"$thing/down/property/******/dev_1\",0");
    delay(500);
    if (softSerial.find("OK"))
    {
        Serial.println("AT+TCMQTTSUB:ok");
    }
    Serial.println("testing.....");
}
void loop()
{

    autoLight();   //自动感应灯  玄关位置
    Tem_Hum();     //温湿度传感器
    flameSensor(); //火焰传感器
    if (millis() - lastmin >= 4000)
    {
        lastmin = millis();
        String upTopic = "AT+TCMQTTPUB=\"$thing/up/property/******/dev_1\",0,";
        String property = upProperty();
        Serial.println(upTopic + property);
        softSerial.println(upTopic + property);
        delay(500);
    }
    if (softSerial.available() > 0)
    {
        if (softSerial.find("params\":"))
        {
            Serial.println("找到parm");
            String serialData = softSerial.readString();
            int position = serialData.indexOf('}');
            serialData.remove(position + 1, serialData.length() - position);
            StaticJsonDocument<64> doc;
            DeserializationError error = deserializeJson(doc, serialData);
            if (error)
            {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(error.c_str());
            }
            ledState = doc["redled"];
            fanState = doc["fan"];
            curState = doc["cur"];
            sensorState(ledState, ledPin);
            sensorState(fanState, fanPin);
            delay(50);
        }
        else
        {
        }
    }
}

String upProperty()
{

    String content = "\"{\\\"method\\\":\\\"report\\\"\\,\\\"clientToken\\\":\\\"\\\"\\,\\\"params\\\":{\\\"redled\\\":";
    content += ledState;
    content += "\\,\\\"tem\\\":";
    content += Temperature;
    content += "\\,\\\"hum\\\":";
    content += Humidity;
    content += "\\,\\\"warn\\\":";
    content += warnState;
    content += "\\,\\\"atuoLed\\\":";
    content += atuoLedState;
    content += "\\,\\\"cur\\\":";
    content += curState;
    content += "\\,\\\"fan\\\":";
    content += fanState;
    content += "}}\"";
    return content;
}

//自动感应灯
void autoLight()
{
    // Serial.print("LDR Reading: ");              //通过串口监视器
    int lightNess = analogRead(lightSensorPin); //输出LDR读数
    // Serial.println(lightNess);
    int irSensorOutput = analogRead(irSensorPin); // 读取红外传感器输出
    // Serial.print("irSensor: ");
    // Serial.println(irSensorOutput);
    if (lightNess < 300 && irSensorOutput > 600) // 如果红外传感器输出高电平
    {
        atuoLedState = true;
        sensorState(atuoLedState, atuoLedPin);
    }
    else
    {
        atuoLedState = false;
        sensorState(atuoLedState, atuoLedPin);
    }
}

//温湿度传感器
void Tem_Hum()
{
    dht.begin();
    Humidity = dht.readHumidity();
    Temperature = dht.readTemperature();
    delay(50);
    if (isnan(Humidity) || isnan(Temperature))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    if (millis() - lasttm >= 4000)
    {
        lasttm = millis();
        Serial.print(F("Humidity(%):"));
        Serial.println(Humidity);
        Serial.print(F("Temperature(°C):"));
        Serial.println(Temperature);
    }
}

//传感器状态模块
void sensorState(bool sensorState, int pin)
{
    if (sensorState)
    {
        digitalWrite(pin, HIGH);
    }
    else
    {
        digitalWrite(pin, LOW);
    }
}

// Warn模块

void warning(bool warnState)
{
    if (warnState)
    {

        for (int i = 0; i < 80; i++) //输出一个频率的声音
        {
            digitalWrite(warnPin, HIGH);
            delay(1);
            digitalWrite(warnPin, LOW);
            delay(1);
        }
        for (int i = 0; i < 100; i++) //输出另一个频率的声音
        {
            digitalWrite(warnPin, HIGH);
            delay(2);
            digitalWrite(warnPin, LOW);
            delay(2);
        }
        // Serial.println("warning!!");
    }
    else
    {
        digitalWrite(warnPin, LOW);
    }
}

//火焰传感器
void flameSensor()
{
    int flameNess = analogRead(flamePin);
    // Serial.print("flameNess:");
    // Serial.println(flameNess);
    if (flameNess < 512)
    {
        warnState = true;
        warning(warnState);
    }
    else
    {
        warnState = false;
        warning(warnState);
    }
}

#include <SoftwareSerial.h>
SoftwareSerial softSerial(6, 7);
String arduino = "";
String esp8266 = "";
unsigned long lastmin = 10000;
bool fanState = false;
bool warnState = false;
bool ledState = false;
bool atuoLedState = false;
bool curState = false;
float Humidity = 0;
float Temperature = 0;
void setup()
{
    Serial.begin(9600);
    Serial.println("heloo");
    softSerial.begin(9600);
    //监听软串口通信
    softSerial.listen();
    delay(3000);
    softSerial.println("ATE0");
    delay(500);
    if (softSerial.find("OK"))
    {
        Serial.println("ATE0设置");
    }
    softSerial.println("AT+TCDEVINFOSET=1,\"2IUBPL6QC1\",\"dev_1\",\"d/F9wK2th/V8tXiyZmGfSA==\"");
    delay(500);
    softSerial.println("AT+TCMQTTCONN=1,5000,240,0,1");
    delay(500);
    softSerial.println("AT+TCMQTTSUB=\"$thing/down/property/2IUBPL6QC1/dev_1\",0");
    delay(500);
    if (softSerial.find("OK"))
    {
        Serial.println("AT+TCMQTTSUB:ok");
    }
}
void loop()
{
    // if (Serial.available() > 0)
    // {
    //     if (Serial.peek() != '\n')
    //     {
    //         arduino += (char)Serial.read();
    //     }
    //     else
    //     {
    //         Serial.read();
    //         Serial.print("arduino said:");
    //         Serial.println(arduino);
    //         softSerial.println(arduino);
    //         delay(2000);
    //         arduino = "";
    //     }
    // }
    if (millis() - lastmin >= 4000)
    {
        lastmin=millis();
        String property = upProperty();
        Serial.println(property);
        softSerial.println(property);
        delay(500);
    }
    if (softSerial.available() > 0)
    {
        //         if (softSerial.peek() != '\n')
        //         {
        //             esp8266 += (char)softSerial.peek();
        //         }
        //         else
        //         {
        //             softSerial.read();
        //             Serial.print("esp8266 said:");
        //             Serial.println(esp8266);
        //             esp8266 = "";
        //         }
        if (softSerial.find("params\":"))
        {
            Serial.println("找到parm");
            String serialData = softSerial.readString();
            Serial.print(serialData.length());
            int position = serialData.indexOf('}');
            // serialData.replace("}\"","");
            serialData.remove(position + 1, serialData.length() - position);
            Serial.println(serialData);
            delay(500);
        }
        else{
          //while(softSerial.read()>0);
          }
    }
}

String upProperty()
{
    String upTopic = "AT+TCMQTTPUB=\"$thing/up/property/2IUBPL6QC1/dev_1\",0,";
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
    return upTopic + content;
}

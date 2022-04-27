bool fanState = false;
bool warnState = false;
bool ledState = false;
bool atuoLedState = false;
bool curState = false;
float Humidity = 0;
float Temperature = 0;

String upProperty()
{
    String upTopic = "\"$thing/up/property/2IUBPL6QC1/dev_1\",0,";
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

void setup()
{
    String property = upProperty();
    Serial.begin(9600);
    Serial.println("test....");
    Serial.println(property);
}

void loop()
{
}
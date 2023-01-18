#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

const char *ssidAP = "SIS_IoT";
const char *passwordAP = "123456789";

const char *user_name = "user";
const char *online_ssid = "ssid";
const char *online_passsword = "pass";

String user_name_data,
    online_ssid_data,
    online_password_data;

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

String updateHTML(const String &var)
{
  File dataFile = SPIFFS.open("/data.txt", "r");
  String dataGetten;
  if (var == "owner")
  {
    dataGetten = dataFile.readStringUntil('\n');
    dataFile.close();
    return dataGetten;
  }
  else if (var == "ssid")
  {
    dataFile.readStringUntil('\n');
    dataGetten = dataFile.readStringUntil('\n');
    dataFile.close();
    return dataGetten;
  }
  else if (var == "password")
  {
    dataFile.readStringUntil('\n');
    dataFile.readStringUntil('\n');
    dataGetten = dataFile.readStringUntil('\n');
    dataFile.close();
    return dataGetten;
  }
  return String();
}

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>SIS IoT Solutions</title>
    </head>
<body>
    <form name="setting" action="/sendData" method="post" class="form-container">
        <fieldset>
            <legend>OWNER</legend>
            <label>Name of owner of the device </label><br><br>
            <input type="text" name="user" class="data" maxlength="50" size="52" value="%owner%" id="ownerid">
        </fieldset>
        <br>
        <fieldset>
            <legend>WIFI Setting</legend>
            <p>Please enter SSID and password for a certain network to make the device can access to it.</p>
            <label>SSID</label> 
            <input type="text" name="ssid" class="data" maxlength="50" size="52" value="%ssid%" id="sside">
            <br><br>
            <label>Password</label>
            <input type="password" name="pass" class="data" maxlength="50" size="52" id="pass" value="%password%">
        </fieldset>
        <br>
        <input type="submit" value="Save" class="buttonSave">
    </form>
</body>
</html>
)rawliteral";

void setup()
{
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssidAP, passwordAP);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("IP: ");
  Serial.println(IP);
  Serial.println("-----------------------------------");

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(SPIFFS, "/index.html", "text/html", false, updateHTML);
            });

  server.on("/sendData", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              user_name_data        = request->getParam(0)->value().c_str();
              online_ssid_data      = request->getParam(1)->value().c_str();
              online_password_data  = request->getParam(2)->value().c_str();

              if (SPIFFS.exists("data.txt")) SPIFFS.remove("data.txt");
              File data_file = SPIFFS.open("/data.txt", "w");
              data_file.println(user_name_data);
              data_file.println(online_ssid_data);
              data_file.println(online_password_data);
              data_file.close();

              request->send_P(200, "text/html", "Data Saved Successfully");

              // request->send(SPIFFS, "/sun.png", "image/png");
            });

  server.onNotFound(notFound);
  server.begin();
}

void loop() {}
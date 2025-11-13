#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "useless2";
const char* password = "#arystoteles54";

WebServer server(80);

int lampka1=22;
int lampka2=23;

bool stan1=true;
bool stan2=false;


void handleRoot() {
    String html = "<!DOCTYPE html><html lang='pl'><head>";
    html += "<meta charset='UTF-8'>";
    html += "<title>Sterowanie lampkami</title>";
    html += "<style>";
    html += "body{background:#181818;color:#fff;font-family:Arial,sans-serif;text-align:center;margin-top:50px;}";
    html += "h1{color:#00ffff;}";
    html += ".lampka{display:inline-block;background:#242424;border-radius:10px;margin:15px;padding:20px;width:200px;";
    html += "box-shadow:0 0 10px rgba(0,255,255,0.3);}";
    html += "a{display:inline-block;margin:10px 5px;padding:10px 20px;border-radius:5px;text-decoration:none;";
    html += "color:white;font-weight:bold;}";
    html += "a.on{background-color:#00cc66;}";
    html += "a.off{background-color:#cc3300;}";
    html += "strong{color:#ffff00;}";
    html += "</style></head><body>";

    html += "<h1>Sterowanie lampkami</h1>";

    html += "<div class='lampka'>";
    html += "<h2>Lampka 1</h2>";
    // html += "<p>Stan: <strong>" + String(stan1 ? "ON" : "OFF") + "</strong></p>";
    String stanTekst;
    if(stan1){
        stanTekst="ON";
    }
    else{
        stanTekst="OFF";
    }
    html += "<p>Stan: <strong>" + stanTekst + "</strong></p>"; 
    html += "<a class='on' href='/lampka1/on'>ON</a>";
    html += "<a class='off' href='/lampka1/off'>OFF</a>";
    html += "</div>";


    
    
    html += "<div class='lampka'>";
    html += "<h2>Lampka 2</h2>";
    html += "<p>Stan: <strong>" + String(stan2 ? "ON" : "OFF") + "</strong></p>";
    html += "<a class='on' href='/lampka2/on'>ON</a>";
    html += "<a class='off' href='/lampka2/off'>OFF</a>";
    html += "</div>";

    html += "</body></html>";

    server.send(200, "text/html", html);
}


void handleLampka1ON(){ 
    stan1=true; 
    digitalWrite(lampka1, HIGH); 
    handleRoot(); 
}
void handleLampka1OFF(){ 
    stan1=false; 
    digitalWrite(lampka1, LOW); 
    handleRoot(); 
}

void handleLampka2ON(){ 
    stan2=true; 
    digitalWrite(lampka2, HIGH); 
    handleRoot(); 
}
void handleLampka2OFF(){ 
    stan2=false; 
    digitalWrite(lampka2, LOW); 
    handleRoot(); 
}

void handleNotFound() {
    server.send(404, "text/plain", "Nie znaleziono");
}

void setup(){
    Serial.begin(9600);

    pinMode(lampka1, OUTPUT);
    pinMode(lampka2, OUTPUT);

     digitalWrite(lampka1, stan1 ? HIGH : LOW);
    digitalWrite(lampka2, stan2 ? HIGH : LOW);

    WiFi.begin(ssid, password);
    Serial.print("Laczenie z WiFi");
    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Polaczono!");
    Serial.print("Adres IP ESP32: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/lampka1/on", handleLampka1ON);
    server.on("/lampka1/off", handleLampka1OFF);
    server.on("/lampka2/on", handleLampka2ON);
    server.on("/lampka2/off", handleLampka2OFF);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("Serwer uruchomiony");
}

void loop() {
    server.handleClient();
}

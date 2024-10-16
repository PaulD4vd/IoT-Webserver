#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Alona2"; // Inisialisasi SSID dan password WiFi
const char* password = "manggolo";

WiFiServer Server(80); // Web server menggunakan port HTTP 80

String header; // Untuk menyimpan header HTTP request

String kondisilampu1 = "mati"; // Menyimpan kondisi lampu 1
String kondisilampu2 = "mati"; // Menyimpan kondisi lampu 2
String kondisikedualampu = "mati"; // Menyimpan kondisi lampu 2


// Inisialisasi GPIO yang dipakai
const int lampu1 = 26;
const int lampu2 = 27;

// Waktu sekarang dan sebelumnya untuk timeout client
unsigned long waktusaatini = millis();
unsigned long waktusebelumnya = 0;

// Timeout untuk menjaga client terhubung
const long timeout = 2000;

void setup() {
  Serial.begin(115200);

  // Inisialisasi pin GPIO sebagai output
  pinMode(lampu1, OUTPUT);
  pinMode(lampu2, OUTPUT);

  // Menyalakan lampu di awal
  digitalWrite(lampu1, HIGH); // Lampu nyala di awal
  digitalWrite(lampu2, HIGH); // Lampu nyala di awal
  
  // Koneksi ke WiFi
  Serial.print("Connecting to: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // Menampilkan IP lokal

  Server.begin(); // Memulai server
}

void loop() { 
  WiFiClient client = Server.available(); // Memeriksa apakah ada client baru yang terhubung

  if (client) { // Jika ada client yang terhubung
    waktusaatini = millis();
    waktusebelumnya = waktusaatini;
    Serial.println("Client baru terhubung.");
    String currentLine = ""; // Untuk menyimpan setiap baris HTTP request

    while (client.connected() && (waktusaatini - waktusebelumnya <= timeout)) {
      waktusaatini = millis();
      if (client.available()) { // Jika data tersedia untuk dibaca dari client
        char c = client.read(); // Membaca karakter dari client
        Serial.write(c); // Menampilkan karakter yang diterima di serial monitor
        header += c; // Menyimpan karakter ke string header

        if (c == '\n') { // Jika akhir dari request (baris kosong)
          if (currentLine.length() == 0) {

            // Kirimkan response HTTP ke client
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Menyalakan dan mematikan GPIO berdasarkan URL
            if (header.indexOf("GET /lampu1/nyala") >= 0) {
            Serial.println("Lampu 1 nyala");
            kondisilampu1 = "nyala";
            digitalWrite(lampu1, HIGH); // Menyalakan lampu 1
            } 
            else if (header.indexOf("GET /lampu1/mati") >= 0) {
            Serial.println("Lampu 1 mati");
            kondisilampu1 = "mati";
            digitalWrite(lampu1, LOW); // Mematikan lampu 1
            } 
            else if (header.indexOf("GET /lampu2/nyala") >= 0) {
            Serial.println("Lampu 2 nyala");
            kondisilampu2 = "nyala";
            digitalWrite(lampu2, HIGH); // Menyalakan lampu 2
            }
            else if (header.indexOf("GET /lampu2/mati") >= 0) {
            Serial.println("Lampu 2 mati");
            kondisilampu2 = "mati";
            digitalWrite(lampu2, LOW); // Mematikan lampu 2
            }   
            else if (header.indexOf("GET /lampu12/nyala") >= 0) {
            Serial.println("Lampu 1 dan Lampu 2 nyala");
            kondisikedualampu = "nyala";
            digitalWrite(lampu1, HIGH);  // Menyalakan lampu 1
            digitalWrite(lampu2, HIGH);  // Menyalakan lampu 2
            } 
            else if (header.indexOf("GET /lampu12/mati") >= 0) {
            Serial.println("Lampu 1 dan Lampu 2 mati");
            kondisikedualampu = "mati";
            digitalWrite(lampu1, LOW);  // Mematikan lampu 1
            digitalWrite(lampu2, LOW);  // Mematikan lampu 2
}


            // HTML response
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<title>ESP32 Web Server</title>");
            client.println("<style>");
            client.println("body { text-align: center; font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 0; }");
            client.println("h1 { color: #333; margin-top: 50px; }");
            client.println(".container { max-width: 400px; margin: auto; padding: 20px; background-color: #fff; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }");
            client.println(".status { font-size: 18px; margin-bottom: 20px; }");
            client.println(".button { display: inline-block; padding: 10px 20px; font-size: 18px; margin: 10px; cursor: pointer; border-radius: 5px; border: none; text-decoration: none; color: white; }");
            client.println(".on { background-color: #4CAF50; }");  // Hijau untuk tombol ON
            client.println(".off { background-color: #f44336; }");  // Merah untuk tombol OFF
            client.println("</style>");
            client.println("</head>");

            client.println("<body>");
            client.println("<div class=\"container\">");
            client.println("<h1>Kontrol Lampu ESP32</h1>");

            client.println("<div class=\"status\">");
            client.println("<p>GPIO 26 - Lampu 1: <strong>" + kondisilampu1 + "</strong></p>");
            client.println("</div>");
            client.println("<p><a href=\"/lampu1/nyala\" class=\"button on\">NYALAKAN Lampu 1</a></p>");
            client.println("<p><a href=\"/lampu1/mati\" class=\"button off\">MATIKAN Lampu 1</a></p>");

            client.println("<div class=\"status\">");
            client.println("<p>GPIO 27 - Lampu 2: <strong>" + kondisilampu2 + "</strong></p>");
            client.println("</div>");
            client.println("<p><a href=\"/lampu2/nyala\" class=\"button on\">NYALAKAN Lampu 2</a></p>");
            client.println("<p><a href=\"/lampu2/mati\" class=\"button off\">MATIKAN Lampu 2</a></p>");

            client.println("<div class=\"status\">");
            client.println("<p>Kedua Lampu - Lampu 1 dan 2: <strong>" + kondisikedualampu + "</strong></p>");
            client.println("</div>");
            client.println("<p><a href=\"/lampu12/nyala\" class=\"button on\">NYALAKAN Kedua Lampu</a></p>");
            client.println("<p><a href=\"/lampu12/mati\" class=\"button off\">MATIKAN Kedua Lampu</a></p>");

            client.println("</div>");
            client.println("</body></html>");



            // Akhiri response HTTP
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c; // Menyimpan karakter dalam currentLine
        }
      }
    }

    // Bersihkan header
    header = "";
    // Tutup koneksi
    client.stop();
    Serial.println("Client terputus.");
  }
}

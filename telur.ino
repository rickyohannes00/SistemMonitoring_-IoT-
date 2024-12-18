#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ESP8266WiFi.h>


// Definisikan pin DHT11 dan Relay
#define DHTPIN 12      // Pin data DHT11 (D6 pada ESP8266)
#define DHTTYPE DHT11  // Gunakan sensor DHT11
#define RELAY_PIN 13   // Pin untuk Relay (D7 pada ESP8266)

// Blynk Template
#define BLYNK_TEMPLATE_ID "TMPL6_LGPNlf4"
#define BLYNK_TEMPLATE_NAME "Alat Penetas Telur Kelompok NONE"
#define BLYNK_AUTH_TOKEN "4Hr2FGTu3A80AVrCHt3AqihNBT9Rj7ST"

#include <BlynkSimpleEsp8266.h>

// Inisialisasi sensor DHT11
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

int relayState;

// Inisialisasi LCD I2C (alamat default adalah 0x27, bisa berubah tergantung jenis LCD)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16 kolom, 2 baris

// Definisikan kredensial WiFi
char ssid[] = "Ricky10";    // Ganti dengan SSID WiFi Anda
char pass[] = "yohanes11";  // Ganti dengan password WiFi Anda
char auth[] = "4Hr2FGTu3A80AVrCHt3AqihNBT9Rj7ST";

// Fungsi untuk membaca data suhu dan kelembapan
void readDHTData() {
  float suhu = dht.readTemperature();     // Baca suhu dalam Celcius
  float kelembapan = dht.readHumidity();  // Baca kelembapan

  // Periksa apakah pembacaan berhasil
  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca sensor DHT!");
    return;
  }

  // Tampilkan suhu dan kelembapan pada LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Suhu: ");
  lcd.print(suhu);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Kelembapan: ");
  lcd.print(kelembapan);
  lcd.print(" %");

  Serial.print("Suhu: ");
  Serial.print(suhu);
  Serial.print("Kelembapan: ");
  Serial.print(kelembapan);

    // Kirim data suhu dan kelembapan ke Blynk
    Blynk.virtualWrite(V1, suhu);      // Virtual pin V1: Suhu
  Blynk.virtualWrite(V2, kelembapan);  // Virtual pin V2: Kelembapan

  // Kontrol relay berdasarkan suhu 
  if (suhu >= 38.0) {
    digitalWrite(RELAY_PIN, LOW);  // Matikan relay (lampu pijar mati)
    lcd.setCursor(0, 1);
    lcd.print(" Relay: OFF     ");
    relayState = 0;  // Status relay mati
    Blynk.virtualWrite(V3, relayState);  // Kirim status relay OFF ke Blynk
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // Nyalakan relay (lampu pijar hidup)
    lcd.setCursor(0, 1);
    lcd.print(" Relay: ON      ");
    relayState = 1;  // Status relay hidup
    Blynk.virtualWrite(V3, relayState);  // Kirim status relay ON ke Blynk
  }
}

void setup() {
  // Mulai komunikasi serial
  Serial.begin(115200);

  // Inisialisasi DHT11
  dht.begin();

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();

  // Tampilkan pesan selamat datang di LCD
  lcd.setCursor(0, 0);
  lcd.print("Inkubator Telur");
  lcd.setCursor(0, 1);
  lcd.print("Menghubungkan...");
  delay(2000);
  lcd.clear();

  // Atur pin relay sebagai output
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Pastikan relay mati di awal

  // Hubungkan ke Blynk
  // Blynk.begin(BLYNK_TEMPLATE_ID, BLYNK_TEMPLATE_NAME, BLYNK_AUTH_TOKEN, ssid, pass);
  // Blynk.begin();
  Blynk.begin(auth, ssid, pass);
  lcd.setCursor(0, 0);
  lcd.print("Terhubung ke WiFi");

  // Atur timer untuk membaca data DHT setiap 2 detik
  timer.setInterval(2000L, readDHTData);
} 

void loop() {
  Blynk.run();  // Jalankan koneksi Blynk
  timer.run();  // Jalankan timer
}

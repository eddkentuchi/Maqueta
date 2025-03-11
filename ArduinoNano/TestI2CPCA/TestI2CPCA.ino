#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Crear objetos para cada PCA9685 con sus respectivas direcciones
Adafruit_PWMServoDriver pca1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pca2 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pca3 = Adafruit_PWMServoDriver(0x42);
char comandoRecibido = 0;
void setup() {
    // Configurar el Nano como esclavo I2C con dirección 0x08
    Wire.begin(0x08);
    Wire.onReceive(receiveEvent); // Configurar función de recepción de datos
    Serial.begin(9600);
  
    // Iniciar los PCA9685
    pca1.begin();
    pca2.begin();
    pca3.begin();

    // Configurar la frecuencia PWM (ejemplo: 1000 Hz)
    pca1.setPWMFreq(1000);
    pca2.setPWMFreq(1000);
    pca3.setPWMFreq(1000);

    // Encender todos los puertos al inicio
    setAllPWM(4096);
}

void loop() {
    setAllPWM(4096);
    digitalWrite(13,HIGH);
    delay(1000); 
    setAllPWM(0);
    digitalWrite(13,LOW);
    delay(1000);
}

// Función para encender o apagar todos los PWM
void setAllPWM(int value) {
    for (int i = 0; i < 16; i++) {
        pca1.setPWM(i, value, 0);
        pca2.setPWM(i, value, 0);
        pca3.setPWM(i, value, 0);
    }
}
//Evento para recibir informacion por I2C
void receiveEvent(int bytes) {
    Serial.print("Recibiendo ");
    Serial.print(bytes);
    Serial.println(" bytes...");
    while (Wire.available()) {
      comandoRecibido = Wire.read();
        //char c = Wire.read();
        Serial.print(comandoRecibido);
    }
    Serial.println(" - Recibido");
}

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Inicializar los controladores PCA9685
Adafruit_PWMServoDriver pca1(0x40);
Adafruit_PWMServoDriver pca2(0x41);
Adafruit_PWMServoDriver pca3(0x42);

#define DIRECCION_I2C 0x08
// Definir pines de los botones
//const int botones[5] = {13, 12, 14, 27, 26};

// Mapeo de los pisos para cada sección
struct PisoInfo {
    Adafruit_PWMServoDriver* pca;
    int puerto;
};

PisoInfo seccion1[] = {
    {&pca1, 0},{&pca1, 0}, {&pca1, 1}, {&pca1, 2}, {&pca1, 3}, {&pca1, 4}, {&pca1, 5}, {&pca1, 6}, {&pca1, 7},
    {&pca1, 8}, {&pca1, 9}, {&pca1, 10}, {&pca1, 11}, {&pca1, 12}, {&pca1, 13}, {&pca1, 14}, {&pca1, 15},
    {&pca2, 0}, {&pca2, 1}, {&pca2, 2}, {&pca2, 3}
};

PisoInfo seccion2[] = {
    {&pca2, 4}, {&pca2, 5}, {&pca2, 6}, {&pca2, 7}, {&pca2, 8}, {&pca2, 9}, {&pca2, 10}, {&pca2, 11},
    {&pca2, 12}, {&pca2, 13}, {&pca2, 14}, {&pca2, 15}, {&pca3, 0}, {&pca3, 1}, {&pca3, 2}, {&pca3, 3},
    {&pca3, 4}, {&pca3, 5}, {&pca3, 6}, {&pca3, 7}, {&pca3, 8}, {&pca3, 9}, {&pca3, 10}, {&pca3, 11}
};

const int numPisosSeccion1 = sizeof(seccion1) / sizeof(seccion1[0]);
const int numPisosSeccion2 = sizeof(seccion2) / sizeof(seccion2[0]);
const int maxPisos = max(numPisosSeccion1, numPisosSeccion2);
const int pwmMax =500;
const long menuInterval = 15000;
const long interval = 600;
unsigned long currentMillis=0;
unsigned long previousMillis = 0;
unsigned long menuPreviosMillis = menuInterval;
byte  currentMode = 0;
byte piso = 0;
byte edificio =0;
byte enable =0; 
bool subiendo = true;
int iVar=0;
bool isEverOn=false;
void setup() {
    Serial.begin(9600);
    //Wire.begin();
    Serial.print(" - Iniciando, PisosMaximos:");
    Serial.println(maxPisos);    
    Serial.print("Pisos Seccion1:");
    Serial.println(numPisosSeccion1);
    Serial.print("Pisos Seccion2:");
    Serial.println(numPisosSeccion2);
    //limpiarEstados();

    Wire.begin(0x08);
    Wire.setClock(10000);
    Wire.onReceive(receiveEvent); // Define la función para recibir datos
    //Wire.onRequest(requestEvent); 
    pca1.begin();
    pca2.begin();
    pca3.begin();
    pca1.setPWMFreq(1000);
    pca2.setPWMFreq(1000);
    pca3.setPWMFreq(1000);
    Serial.println(" - Fin setUp");
    //for (iVar = 0; iVar < 5; iVar++) {
    //    pinMode(botones[iVar], INPUT_PULLUP);
    //}
}

void loop() {
    /*for (iVar = 0; i < 5; i++) {
        if (digitalRead(botones[i]) == LOW) {
            currentMode = i + 1;
            limpiarEstados(); // Limpiar estados al cambiar de modo
            piso = 0; // Reiniciar el piso actual
            subiendo = true;
            delay(50);
        }
    }*/
    //Función para avance automatico
    //autoAdvance();
    switch (currentMode) {
        case 1:
            encendidoSecuencial();
            break;
        case 2:
            efectoSubidaYBajada();
            break;
        case 3:
            iluminacionEstatica();
            break;
        case 4:
            efectoRespiracion();
            break;
        case 5:
            modoAleatorio();
            break;
        case 6:
            modoAleatorio();
            break;
    }
    
       
}
void autoAdvance(){  
  currentMillis = millis();
  Serial.println(" - AutoAvance");
  if(menuPreviosMillis<currentMillis){
    Serial.println(" -Cambiando de modo");
    currentMode++;
    limpiarEstados();
    menuPreviosMillis=currentMillis+menuInterval;
    if(currentMode>6){
      currentMode=1;
      }
    } 
}
void setLuz(PisoInfo* seccion, int piso, int valor) {
  Serial.println(" - setLuz");
  seccion[piso].pca->setPWM(seccion[piso].puerto, 0, valor);
}

void limpiarEstados() {
  Serial.println(" - Limpiar estados");
  for (iVar = 0; iVar < maxPisos; iVar++) {
      //Serial.print(i);
        if (iVar < numPisosSeccion1) setLuz(seccion1, iVar, 0);
        if (iVar < numPisosSeccion2) setLuz(seccion2, iVar, 0);
        delay(1);
  }
   isEverOn=false;
   piso = 0;
   subiendo = true;
}
void encendidoSecuencial() {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println(" - Secuencial");
      Serial.print(currentMillis);
      previousMillis = currentMillis;
       if (piso < numPisosSeccion1){
        setLuz(seccion1, piso, pwmMax);
        if(piso>0)setLuz(seccion1, piso-1, 0);
        else setLuz(seccion1, numPisosSeccion1-1, 0);
       }
       if (piso < numPisosSeccion2) {
        setLuz(seccion2, piso, pwmMax);
        if(piso>0)setLuz(seccion2, piso-1, 0);
        else setLuz(seccion2, numPisosSeccion2-1, 0);
       }
      piso++;
      if (piso >= maxPisos) {
          piso = 0;
      }
    }
}

void efectoSubidaYBajada() {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println(" - Subida Bajada");
      previousMillis = currentMillis;
      if (piso < numPisosSeccion1){
      setLuz(seccion1, piso, subiendo ? pwmMax : 0);
      }
      if (piso < numPisosSeccion2) {
      setLuz(seccion2, piso, subiendo ? pwmMax : 0);
      }
      piso += (subiendo ? 1 : -1);
      if (piso >= maxPisos) {
          subiendo = false;
          piso = numPisosSeccion1 - 1;
      } else if (piso < 0) {
          subiendo = true;
          piso = 0;
      }
    }
}

void iluminacionEstatica() {
  Serial.println(" - Todos prendidos");
  if(isEverOn==false){
    for (iVar = 0; iVar < maxPisos; iVar++) {
      //Serial.print(i);
        if (iVar < numPisosSeccion1) setLuz(seccion1, iVar, 300);
        if (iVar < numPisosSeccion2) setLuz(seccion2, iVar, 300);
        delay(1);
    }
   isEverOn= true;
  }
}

void efectoRespiracion() {
    static int intensidad = 0;
    static bool aumentando = true;
    currentMillis = millis();
    if (currentMillis - previousMillis >= 30) {
      Serial.println(" - Respiracion");
      previousMillis = currentMillis;

      for (iVar = 0; iVar < maxPisos; iVar++) {
        if (iVar < numPisosSeccion1) setLuz(seccion1, iVar, intensidad);
        if (iVar < numPisosSeccion2) setLuz(seccion2, iVar, intensidad);
      }
      if (aumentando) {
        intensidad += 6;
        if (intensidad >= 350) aumentando = false;
      } else {
        intensidad -= 33;
        if (intensidad <= 0) aumentando = true;
      }
    }
}

void modoAleatorio() {
     currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {    
      Serial.println(" - Aleatorio");
      previousMillis = currentMillis;
      for (iVar = 0; iVar < numPisosSeccion1; iVar++) {
        int intensidad = random(0, pwmMax);
        setLuz(seccion1, iVar, intensidad);
      }
      for (iVar = 0; iVar < numPisosSeccion2; iVar++) {
        int intensidad = random(0, pwmMax);
        setLuz(seccion2, iVar, intensidad);
      }
    }
}
void receiveEvent(int bytes) {
    Serial.print("Recibiendo ");
    //Serial.print(bytes);
    //Serial.println(" bytes...");
     // Verifica que se hayan recibido 4 bytes
    if (bytes == 4) {
      currentMode = Wire.read();
      edificio = Wire.read();
      piso = Wire.read();
      enable = Wire.read();
      Serial.println("Datos recibidos: Estado="); Serial.print(currentMode);
      Serial.println(", Edificio="); Serial.print(edificio);
      Serial.println(", Pisos="); Serial.print(piso);
      Serial.println(", Enable="); Serial.println(enable);
    } else {
      Serial.println("Error: Se esperaban 4 bytes");
    }
    Serial.println(" - Recibido");
    //limpiarEstados();
}

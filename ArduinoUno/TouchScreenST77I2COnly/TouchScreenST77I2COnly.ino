
#include <TouchScreen.h> //touch library
#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library
#include <string.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Crear objetos para cada PCA9685 con sus respectivas direcciones
Adafruit_PWMServoDriver pca1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pca2 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver pca3 = Adafruit_PWMServoDriver(0x42);
//if the IC model is known or the modules is unreadable,you can use this constructed function
//LCDWIKI_KBV my_lcd(ST7789V,A3,A2,A1,A0,A4); //model,cs,cd,wr,rd,reset
//if the IC model is not known and the modules is readable,you can use this constructed function
LCDWIKI_KBV my_lcd(240,320,A3,A2,A1,A0,A4);//width,height,cs,cd,wr,rd,reset

                             /*  r     g    b */
#define BLACK        0x0000  /*   0,   0,   0 */
#define BLUE         0x001F  /*   0,   0, 255 */
#define RED          0xF800  /* 255,   0,   0 */
#define GREEN        0x07E0  /*   0, 255,   0 */
#define CYAN         0x07FF  /*   0, 255, 255 */
#define MAGENTA      0xF81F  /* 255,   0, 255 */
#define YELLOW       0xFFE0  /* 255, 255,   0 */
#define WHITE        0xFFFF  /* 255, 255, 255 */
#define NAVY         0x000F  /*   0,   0, 128 */
#define DARKGREEN    0x03E0  /*   0, 128,   0 */
#define DARKCYAN     0x03EF  /*   0, 128, 128 */
#define MAROON       0x7800  /* 128,   0,   0 */
#define PURPLE       0x780F  /* 128,   0, 128 */
#define OLIVE        0x7BE0  /* 128, 128,   0 */
#define LIGHTGREY    0xC618  /* 192, 192, 192 */
#define DARKGREY     0x7BEF  /* 128, 128, 128 */
#define ORANGE       0xFD20  /* 255, 165,   0 */
#define GREENYELLOW  0xAFE5  /* 173, 255,  47 */
#define PINK         0xF81F  /* 255,   0, 255 */

/******************* UI details */
#define BUTTON_R 25 //the radius of button 
#define BUTTON_SPACING_X 25 //the horizontal distance between button
#define BUTTON_SPACING_Y 5  //the vertical distance between button
#define EDG_Y 5 //lower edge distance 
#define EDG_X 20 //left and right distance
#define MARGIN_MF_X 20
#define MARGIN_MF_PADD 11
#define MARGIN_MF_TL 2
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
#define Height 40
//touch sensitivity for X
#define TS_MINX 124
#define TS_MAXX 906

//touch sensitivity for Y
#define TS_MINY 83
#define TS_MAXY 893

// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65

//touch sensitivity for press
#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
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
byte enable =0; 
bool subiendo = true;
int iVar=0;
bool isEverOn=false;

int reFreshLevel=10;
typedef struct _button_info
{
     char button_name[13];
     uint8_t button_name_size;
     uint16_t button_name_colour;
     uint16_t button_colour;
     uint16_t button_x;
     uint16_t button_y;     
 }button_info;

//the definition of buttons
uint16_t calculo_x1= EDG_X+BUTTON_R-1;
uint16_t calculo_y1= my_lcd.Get_Display_Height()-EDG_Y-4*BUTTON_SPACING_Y-9*BUTTON_R-1;
uint16_t calculo_y2= my_lcd.Get_Display_Height()-EDG_Y-3*BUTTON_SPACING_Y-7*BUTTON_R-1;
uint16_t calculo_y3= my_lcd.Get_Display_Height()-EDG_Y-2*BUTTON_SPACING_Y-5*BUTTON_R-1;
uint16_t calculo_y4= my_lcd.Get_Display_Height()-EDG_Y-BUTTON_SPACING_Y-3*BUTTON_R-1;
uint16_t calculo_y5= my_lcd.Get_Display_Height()-EDG_Y-BUTTON_R-1;

button_info phone_button[15] = 
{
  "1",3,BLACK,CYAN,calculo_x1,calculo_y1,
  "2",3,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,calculo_y1,
  "3",3,BLACK,CYAN,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,calculo_y1,
  "4",3,BLACK,CYAN,calculo_x1,calculo_y2, 
  "5",3,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,calculo_y2,
  "6",3,BLACK,CYAN,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,calculo_y2,
  "7",3,BLACK,CYAN,calculo_x1,calculo_y3,
  "8",3,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,calculo_y3,
  "9",3,BLACK,CYAN,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,calculo_y3,
  "on",2,BLACK,GREEN,calculo_x1,calculo_y4,
  "0",3,BLACK,CYAN,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,calculo_y4,
  "off",2,BLACK,RED,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,calculo_y4,
  "e1",2,BLACK,YELLOW,calculo_x1,calculo_y5,
  "back",2,BLACK,LIGHTGREY,EDG_X+3*BUTTON_R+BUTTON_SPACING_X-1,calculo_y5,
  "dele",2,BLACK,LIGHTGREY,EDG_X+5*BUTTON_R+2*BUTTON_SPACING_X-1,calculo_y5,
};
button_info menu_functions[6] = 
{
  "Secuencial",MARGIN_MF_TL,BLACK,0x6EB5,MARGIN_MF_X,MARGIN_MF_PADD,
  "Intermitente",MARGIN_MF_TL,BLACK,0x6EB5,MARGIN_MF_X,(Height)+MARGIN_MF_PADD*2,
  "Estatica",MARGIN_MF_TL,BLACK,0x6EB5,MARGIN_MF_X,(Height*2)+MARGIN_MF_PADD*3,
  "Respiracion",MARGIN_MF_TL,BLACK,0x6EB5,MARGIN_MF_X,(Height*3)+MARGIN_MF_PADD*4,
  "Aleatorio",MARGIN_MF_TL,BLACK,0x6EB5,MARGIN_MF_X,(Height*4)+MARGIN_MF_PADD*5,
  "Pisos",MARGIN_MF_TL,BLACK,0x6EB5,MARGIN_MF_X,(Height*5)+MARGIN_MF_PADD*6,
};
//display string
void show_string(const char* str,int16_t x,int16_t y,uint8_t csize,uint16_t fc, uint16_t bc,boolean mode)
{
    my_lcd.Set_Text_Mode(mode);
    my_lcd.Set_Text_Size(csize);
    my_lcd.Set_Text_colour(fc);
    my_lcd.Set_Text_Back_colour(bc);
    my_lcd.Print_String(str,x,y);
}

//Check whether to press or not
boolean is_pressed(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t px,int16_t py)
{
    if((px > x1 && px < x2) && (py > y1 && py < y2))
    {
        return true;  
    } 
    else
    {
        return false;  
    }
 }

//display the main menu
void levels_menu(void)
{
    uint16_t i;
   for(i = 0;i < sizeof(phone_button)/sizeof(button_info);i++)
   {
      my_lcd.Set_Draw_color(phone_button[i].button_colour);
      my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
      show_string(phone_button[i].button_name,phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+1,phone_button[i].button_name_size,phone_button[i].button_name_colour,BLACK,1);
   }
   my_lcd.Set_Draw_color(BLACK);
   my_lcd.Fill_Rectangle(1, 1, my_lcd.Get_Display_Width()-2, 3);
   my_lcd.Fill_Rectangle(1, 29, my_lcd.Get_Display_Width()-2, 31);
   my_lcd.Fill_Rectangle(1, 1, 3, 31);
   my_lcd.Fill_Rectangle(my_lcd.Get_Display_Width()-4, 1, my_lcd.Get_Display_Width()-2, 31);
}
void show_menu(int reFreshLevel)
{
    uint16_t i;
    
     // my_lcd.Set_Draw_color(RED);
      //my_lcd.Fill_Rectangle(20,10, my_lcd.Get_Display_Width()-20,50);
      
   for(i = 0; i < sizeof(menu_functions) / sizeof(button_info); i++)
    {
    
    // Dibujar botón con bordes redondeados
    if(reFreshLevel !=i){
      my_lcd.Set_Draw_color(DARKGREY);
      my_lcd.Fill_Round_Rectangle(menu_functions[i].button_x, menu_functions[i].button_y, menu_functions[i].button_x + my_lcd.Get_Display_Width() - (menu_functions[i].button_x+5), menu_functions[i].button_y + Height, 8); // Bordes redondeados
      
      my_lcd.Set_Draw_color(menu_functions[i].button_colour);
      my_lcd.Fill_Round_Rectangle(menu_functions[i].button_x + 2, menu_functions[i].button_y , menu_functions[i].button_x + my_lcd.Get_Display_Width() - (menu_functions[i].button_x+5), menu_functions[i].button_y + Height - 2, 8); // Botón interno
  
      // Mostrar texto centrado
  
      show_string(
          menu_functions[i].button_name,menu_functions[i].button_x + 10,menu_functions[i].button_y + (Height / 2) - 7,menu_functions[i].button_name_size,menu_functions[i].button_name_colour,menu_functions[i].button_colour,1);
      }
    }
}
void refreshBotton(int i)
{
    my_lcd.Set_Draw_color(GREEN);
    my_lcd.Fill_Round_Rectangle(menu_functions[i].button_x -8, menu_functions[i].button_y, menu_functions[i].button_x + my_lcd.Get_Display_Width() - (menu_functions[i].button_x+5) - 2, menu_functions[i].button_y + Height - 2, 8); // Botón interno
    show_string(menu_functions[i].button_name,menu_functions[i].button_x + 10,menu_functions[i].button_y + (Height / 2) - 7,menu_functions[i].button_name_size,menu_functions[i].button_name_colour,menu_functions[i].button_colour,1);
}
                            
void setup(void) 
{
  Serial.begin(9600);
  Wire.begin();
  my_lcd.Init_LCD();
  Serial.println(my_lcd.Read_ID(), HEX);
  pca1.begin();
  pca2.begin();
  pca3.begin();
  pca1.setPWMFreq(1000);
  pca2.setPWMFreq(1000);
  pca3.setPWMFreq(1000);
  my_lcd.Fill_Screen(WHITE); 
  show_menu(reFreshLevel);
}

uint16_t text_x=10,text_y=6,text_x_add = 6*phone_button[0].button_name_size,text_y_add = 8*phone_button[0].button_name_size;
uint16_t digitsN=0;
bool textChange = true;
char pisos[3]= "";
uint16_t i;
bool stateOfLevels= false;
int staticValue=0;
byte edificio = 0;
void loop(void)
{
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
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
    }
  //show_menu();
  //Serial.println("Ejecutando");
  //delay(100);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
  {
    //p.x = my_lcd.Get_Display_Width()-map(p.x, TS_MINX, TS_MAXX, my_lcd.Get_Display_Width(), 0);
    //p.y = my_lcd.Get_Display_Height()-map(p.y, TS_MINY, TS_MAXY, my_lcd.Get_Display_Height(), 0);
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, my_lcd.Get_Display_Width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, my_lcd.Get_Display_Height());
  
  for(i=0;i<sizeof(menu_functions)/sizeof(button_info);i++){
      //press the button
      Serial.println("again");
      Serial.println(i);
      Serial.println("-------");
      if(is_pressed(menu_functions[i].button_x-BUTTON_R,menu_functions[i].button_y,menu_functions[i].button_x + my_lcd.Get_Display_Width() - (menu_functions[i].button_x+5),menu_functions[i].button_y + Height,p.x,p.y))
         {
          my_lcd.Set_Draw_color(WHITE);
          my_lcd.Fill_Round_Rectangle(menu_functions[staticValue].button_x-10, menu_functions[staticValue].button_y, menu_functions[staticValue].button_x + my_lcd.Get_Display_Width() - (menu_functions[staticValue].button_x+5), menu_functions[staticValue].button_y + Height, 8); // Bordes redondeados 
          my_lcd.Set_Draw_color(DARKGREY);
          my_lcd.Fill_Round_Rectangle(menu_functions[staticValue].button_x, menu_functions[staticValue].button_y + 2, menu_functions[staticValue].button_x + my_lcd.Get_Display_Width() - (menu_functions[staticValue].button_x+5) - 2, menu_functions[staticValue].button_y + Height , 8); // Botón interno
          my_lcd.Set_Draw_color(menu_functions[staticValue].button_colour);
          my_lcd.Fill_Round_Rectangle(menu_functions[staticValue].button_x +2, menu_functions[staticValue].button_y + 2, menu_functions[staticValue].button_x + my_lcd.Get_Display_Width() - (menu_functions[staticValue].button_x+5) - 2, menu_functions[staticValue].button_y + Height - 2, 8); // Botón interno
          show_string(menu_functions[staticValue].button_name,menu_functions[staticValue].button_x + 10,menu_functions[staticValue].button_y + (Height / 2) - 7,menu_functions[staticValue].button_name_size,menu_functions[staticValue].button_name_colour,menu_functions[staticValue].button_colour,1);
          
          my_lcd.Set_Draw_color(DARKGREY);
          my_lcd.Fill_Round_Rectangle(menu_functions[i].button_x-10, menu_functions[i].button_y, menu_functions[i].button_x + my_lcd.Get_Display_Width() - (menu_functions[i].button_x+5)-2, menu_functions[i].button_y + Height, 8); // Bordes redondeados 
          my_lcd.Set_Draw_color(ORANGE);
          my_lcd.Fill_Round_Rectangle(menu_functions[i].button_x -8, menu_functions[i].button_y + 2, menu_functions[i].button_x + my_lcd.Get_Display_Width() - (menu_functions[i].button_x+5) - 2, menu_functions[i].button_y + Height - 2, 8); // Botón interno
          delay(100);
          refreshBotton(i);
          reFreshLevel=i;
          staticValue=i;
          Serial.println(i);
          switch(i){
            case 0:
              Serial.println("Case0");
              currentMode=1;
              show_menu(reFreshLevel);
              refreshBotton(i);
              break;
            case 1:            
              Serial.println("Case1");
              currentMode=2;             
              show_menu(reFreshLevel);
              refreshBotton(i);
              break;
            case 2:            
              Serial.println("Case2");
              currentMode=3;
              show_menu(reFreshLevel);
              refreshBotton(i);
              break;
            case 3:
              Serial.println("Case3");
              currentMode=4;
              show_menu(reFreshLevel);
              refreshBotton(i);
              break;
            case 4:
              Serial.println("Case4");
              currentMode=5;
              show_menu(reFreshLevel);
              refreshBotton(i);
              break;
            case 5:
              Serial.println("Case5");
              stateOfLevels= true;
              my_lcd.Fill_Screen(WHITE);
              levels_menu();
              while(stateOfLevels==true){
                //Serial.println("InWhile");
                TSPoint p = ts.getPoint();
                pinMode(XM, OUTPUT);
                pinMode(YP, OUTPUT);
                if (p.z > MINPRESSURE && p.z < MAXPRESSURE)
                {
                  p.x = map(p.x, TS_MINX, TS_MAXX, 0, my_lcd.Get_Display_Width());
                  p.y = map(p.y, TS_MINY, TS_MAXY, 0, my_lcd.Get_Display_Height());
                funtionToLevels(p);
                
                Serial.println("finalfuntion");
                if(stateOfLevels==false) {
                  Serial.println("stateoOfLevels false");
                  i=6;
                  Serial.println(i);
                  my_lcd.Fill_Screen(WHITE); 
                  show_menu(10);
                  delay(100);
                  break; 
                }
                }
              }
              break;
            default:
              Serial.println("Dato incorrecto");
              break;   
          }
          
        }
   }
    //funtionToLevels(p);
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

void funtionToLevels(TSPoint p){
  
  for(i=0;i<sizeof(phone_button)/sizeof(button_info);i++)
    {
         //press the button
         if(is_pressed(phone_button[i].button_x-BUTTON_R,phone_button[i].button_y-BUTTON_R,phone_button[i].button_x+BUTTON_R,phone_button[i].button_y+BUTTON_R,p.x,p.y))
         {
              my_lcd.Set_Draw_color(ORANGE);
              my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
              show_string(phone_button[i].button_name,phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+1,phone_button[i].button_name_size,WHITE,BLACK,1);
              delay(100);
              my_lcd.Set_Draw_color(phone_button[i].button_colour);
              my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
              show_string(phone_button[i].button_name,phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+1,phone_button[i].button_name_size,phone_button[i].button_name_colour,BLACK,1);  
              if(i < 12)
              {
                  if(digitsN < 2)
                  {
                    if(i != 9 && i != 11)
                    {
                    show_string(phone_button[i].button_name,text_x,text_y,phone_button[i].button_name_size,RED, BLACK,1);
                    text_x += text_x_add-1;
                    strncat(pisos, phone_button[i].button_name, 1);
                    Serial.print("Numero parcial: ");
                    Serial.println(pisos);
                    digitsN++;
                    }
                  }
                   if(i == 9 || i == 11){
                      Serial.println("Inicia rutina para mandar datos");
                      byte piso = 0;
                      if (pisos[0] != '\0') { // Comprobar si no está vacía
                        Serial.println("La cadena no es vacia");
                        int numeroEntero = atoi(pisos);
                        if (numeroEntero >= 0 && numeroEntero <= 255) { // Verificar rango de byte                          
                          Serial.println("tenemos un numero entre el rango");
                          piso = (byte)numeroEntero;
                          if(9 == i){
                            Serial.println("On");
                            //sendMessage(6,edificio, piso,1);
                          }else{
                            Serial.println("Off");
                            //sendMessage(6,edificio, piso,0);                          
                          }
                        } else {
                          Serial.println("Error: El número está fuera del rango de byte.");
                        }
                      } else {
                        Serial.println("Error: La cadena está vacía.");
                      }
                      }
              }else if(12 == i) //Change Levels
              {
                  Serial.println("Es doce");
                  Serial.println(textChange);
                  if(textChange == true){
                    Serial.println("Entro aqui");
                    edificio= 1;
                    my_lcd.Set_Draw_color(ORANGE);                  
                    my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
                    show_string("e2",phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+1,phone_button[i].button_name_size,phone_button[i].button_name_colour,BLACK,1);  
                  }else{
                    Serial.println("Entro else");
                    edificio= 0;
                    my_lcd.Set_Draw_color(phone_button[i].button_colour);                  
                    my_lcd.Fill_Circle(phone_button[i].button_x, phone_button[i].button_y, BUTTON_R);
                    show_string(phone_button[i].button_name,phone_button[i].button_x-strlen(phone_button[i].button_name)*phone_button[i].button_name_size*6/2+1,phone_button[i].button_y-phone_button[i].button_name_size*8/2+1,phone_button[i].button_name_size,phone_button[i].button_name_colour,BLACK,1);  
                  }                  
                  textChange = !textChange;
              } 
              else if(13 == i) //Return to the menu
              {
                  Serial.println("Entro al 13");
                  int longitud = strlen(pisos);
                  for(int j=0;j<longitud;j++){
                  eliminarUltimoDigito(pisos);
                  text_x -= (text_x_add-1);
                  digitsN--;
                  }
                  stateOfLevels=false;
                  break;
              }
              else if(14 == i) //delete button
              {
                  if(digitsN > 0)
                  {
                      my_lcd.Set_Draw_color(WHITE);
                      text_x -= (text_x_add-1);  
                      my_lcd.Fill_Rectangle(text_x, text_y, text_x+text_x_add-1, text_y+text_y_add-2);
                      digitsN--;
                      eliminarUltimoDigito(pisos);
                      Serial.print("Numero parcial despues de borrar: ");
                      Serial.println(pisos);
                  }
              }
              Serial.println(pisos);
         }      
      }
  }


void eliminarUltimoDigito(char *numero) {
    int longitud = strlen(numero);
    if (longitud > 0) {
        numero[longitud - 1] = '\0';
    }
}

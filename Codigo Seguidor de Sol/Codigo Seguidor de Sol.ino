//Codigo Seguidor Solar

///// Declaracion de bibliotecas arduino ////

//Para controlar el servo
#include <Servo.h>

//Para la pantalla OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//// Declaracion de pines del arduino /////

//Resistencias dependientes de luz y led
#define LDR1 A0
#define LDR2 A1

//Sensor de voltaje en el panel
#define SEN_PAN A2

// Boton
#define BTN 5

//Objeto para controlar el servo
Servo myservo;

//Pantalla OLED y sus parametros

#define SCREEN_WIDTH 128 //ancho de la pantalla OLED en pixeles
#define SCREEN_HEIGHT 64 //altura de la pantalla OLED en pixeles

#define OLED_RESET     -1   //Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C //Tipo de direccion de la pantalla en su controlador
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

///// Declaracion de variables y constantes ////
  
 int   estado_boton = 0; //Para saber si se ha pulsado el boton
 int   valor_ldr1   = 0; //Para guardar el valor de la LDR 1
 int   valor_ldr2   = 0; //Para guardar el valor de la LDR 2
 int   angulo_servo = 90; //Para guardar el valor del angulo del servo
 float voltaje_pan  = 0; //Para guardar el valor del voltaje del panel

void setup() {

///// Declaracion de funcion de pines como salida o entrada

  //Entradas
  pinMode(LDR1, INPUT);
  pinMode(LDR2, INPUT);
  pinMode(SEN_PAN, INPUT);
  pinMode(BTN, INPUT_PULLUP); //Modo especial de resistencia PULLUP para botones

  //Funcion para saber si la pantalla funciona (dada por fabricante)
  if(!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      digitalWrite(LED, HIGH);
      for(;;); // Don't proceed, loop forever
    }  
  oled.setTextColor(SSD1306_WHITE);
      
  //Definimos el pin del servo
  myservo.attach(9);

}

void loop() {

  estado_boton = digitalRead(BTN); //Lee si el boton ha sido pulsado
  seguimiento();
  if (!estado_boton) { //Si el boton se pulsa enciende la pantalla

    // Se mide el voltaje del panel y calcula su eficiencia
    float eficiencia_calculada = eficiencia();
    
    // Se coloca el texto en la pantalla con las siguientes funciones
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setCursor(0,5);
    oled.print("Voltaje Panel: ");
    oled.setTextSize(2);
    oled.setCursor(85,0);
    oled.print(voltaje_pan, 1);
    oled.setTextSize(1);
    oled.setCursor(0,40);
    oled.print("%");
    oled.setTextSize(1);
    oled.setCursor(7,40);
    oled.print("Eficiencia: ");
    oled.setTextSize(2);
    oled.setCursor(80,35);
    oled.print(eficiencia_calculada, 1);
    
    oled.display();
    
  }
  else { //Si no se ha pulsado, la pantalla esta apagada
    
    //Se borra todo lo que haya en pantalla
    oled.clearDisplay();
    oled.print("");
    oled.display();
  }
  
  
}

//// Funcion para el seguimiento del sol ////
void seguimiento(){


  ///Primero lee los valores de las LDRs y los promedia
  for(int i=0; i<10; i++){
    valor_ldr1 = valor_ldr1 + analogRead(LDR1);
    valor_ldr2 = valor_ldr2 + analogRead(LDR2);
  }
  valor_ldr1 = valor_ldr1 / 10;
  valor_ldr2 = valor_ldr2 / 10;

  

  // Restamos los valores para saber como se ha movido el sol
  int resta = valor_ldr1-valor_ldr2;
  
  // Se agregan valores de ajustes, estos dependeran de como se ubique el seguidor solar(necesario hacer pruebas)
  if(resta > 60){
    angulo_servo += 1;
  }
  if(resta < 50){
    angulo_servo -= 1;
  }
  else{
    angulo_servo = angulo_servo;
  }

  // Limita en angulo del servo entre 20 y 170 grados
  angulo_servo = constrain(angulo_servo, 20, 170);

  //Se resetean las variables
  valor_ldr1 = 0;
  valor_ldr2 = 0;

}

//// Funcion para calcular la eficiencia del panel ////
float eficiencia(){
  
  //Leemos el valor del panel solar
  voltaje_pan = analogRead(SEN_PAN);

  //Convertimos a voltaje
  voltaje_pan =  voltaje_pan/(1023 * 5.0);

  //Se calcuala la eficiencia del panel
  float eficiencia = (voltaje_pan/4) * 100;

  //Se retorna el valor calculado 
  return eficiencia;  

}


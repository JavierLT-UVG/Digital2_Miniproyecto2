#include "config.h"

char temp_uart;
int temp_map;
int estado_envio = 1;
int estado_led_a;
int estado_led_r;
int estado_led_v;
int estado_semaforo;

// set up the 'counter' feed
AdafruitIO_Feed *led_A_f        =   io.feed("led_A");
AdafruitIO_Feed *led_R_f        =   io.feed("led_R");
AdafruitIO_Feed *led_V_f        =   io.feed("led_V");
AdafruitIO_Feed *push_A_f       =   io.feed("push_A");
AdafruitIO_Feed *push_R_f       =   io.feed("push_R");
AdafruitIO_Feed *push_V_f       =   io.feed("push_V");
AdafruitIO_Feed *temperatura_f  =   io.feed("Temperatura");

void setup() {
  pinMode(2, OUTPUT);
  // start the serial connection
  Serial.begin(115200);           // Serial de la terminal
  Serial2.begin(115200);          // Serial del ESP32

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // set up a message handler for the count feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  push_A_f->onMessage(handleMessage1);
  push_R_f->onMessage(handleMessage1);
  push_V_f->onMessage(handleMessage1);
  
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Because Adafruit IO doesn't support the MQTT retain flag, we can use the
  // get() function to ask IO to resend the last value for this feed to just
  // this MQTT client after the io client is connected.
  push_A_f->get();
  push_R_f->get();
  push_V_f->get();

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
  
  // Asginación de variable que recibe la temperatura del UART
  while(Serial2.available()>0)
  {
    temp_uart = Serial2.read();
  }
  
  // Switch para mapear variable de temperatura 
  temp_uart = temp_uart - 64;
  switch(temp_uart)
  {
  case '0':   //48
    temp_map = 15;
    break;
  case '1':   //49
    temp_map = 16;
    break;
  case '2':   //50
    temp_map = 17;
    break;
  case '3':   //51
    temp_map = 18;
    break;
  case '4':   //52
    temp_map = 19;
    break;
  case '5':   //53
    temp_map = 20;
    break;
  case '6':   //54
    temp_map = 21;
    break;
  case '7':   //55
    temp_map = 22;
    break;
  case '8':   //56
    temp_map = 23;
    break;
  case '9':   //57
    temp_map = 24;
    break;
  case ':':   //58
    temp_map = 25;
    break;
  case ';':   //59
    temp_map = 26;
    break;
  case '<':   //60
    temp_map = 27;
    break;
  case '=':   //61
    temp_map = 28;
    break;
  case '>':   //62
    temp_map = 29;
    break;
  case '?':   //63
    temp_map = 30;
    break;
  case '@':   //64
    temp_map = 31;
    break;
  case 'A':   //65
    temp_map = 32;
    break;
  case 'B':   //66
    temp_map = 33;
    break;
  case 'C':   //67
    temp_map = 34;
    break;
  default:
    temp_map = 50;
    break;
  }
  
  // Switch para envío de variables
  switch(estado_envio){
    case 1:       // Verde
      Serial.print("sending -> ");
      Serial.println(estado_led_v);
      led_V_f->save(estado_led_v);
      estado_envio++;
      break;
      
    case 2:       // Amarillo
      Serial.print("sending -> ");
      Serial.println(estado_led_a);
      led_A_f->save(estado_led_a);
      estado_envio++;
      break;
      
    case 3:       // Rojo
      Serial.print("sending -> ");
      Serial.println(estado_led_r);
      led_R_f->save(estado_led_r);
      estado_envio++;
      break; 
      
    case 4:       // Temperatura
      Serial.print("sending -> ");
      Serial.println(temp_map);
      temperatura_f->save(temp_map);
      estado_envio=1;
      break; 
      
    default:
      estado_envio = 1;
      break;
  }

  delay(3000);
}

// RECEPCION
// this function is called whenever a 'counter' message
// is received from Adafruit IO. it was attached to
// the counter feed in the setup() function above.
void handleMessage1(AdafruitIO_Data *data) {
  Serial.print("received <- ");
  //Serial.println(data->value());
  estado_semaforo = *data->value();
  
  Serial2.write(estado_semaforo);
  Serial2.print(estado_semaforo);
  
  // Verde
  if(estado_semaforo == '1'){
    estado_led_v = 1; 
    estado_led_a = 0;
    estado_led_r = 0;
    digitalWrite(2, HIGH);
    Serial.print("Semaforo: Verde");
    Serial.print(estado_semaforo);
  }
  
  // Amarillo
 if(estado_semaforo == '2'){
    
    estado_led_v = 0; 
    estado_led_a = 1;
    estado_led_r = 0;
    Serial.print("Semaforo: Amarillo");
    Serial.print(estado_semaforo);
  }
  
  // Rojo
  if(estado_semaforo == '3'){
    
    estado_led_v = 0; 
    estado_led_a = 0;
    estado_led_r = 1;
    Serial.print("Semaforo: Rojo");
    Serial.print(estado_semaforo);
  }
}

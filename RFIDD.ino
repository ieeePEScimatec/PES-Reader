/*
* Autor: Miguel Sena
* blog.eletrogate.com
*/

#include <SPI.h>                                   //Inclusão das bibliotecas
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define ss 10                                     //Define os pinos ss e rst
#define rst 9 

LiquidCrystal_I2C lcd(0x27, 16, 2);               //Cria o objeto do display
MFRC522 rfid(ss, rst);                            //Cria o objeto do rfid
Servo s;                                          //Cria o objeto do servo-motor

String usuarios[][2] {                            //Matriz dos usuários
  {"Miguel", "02:DC:A7:34"},
  {"Joao", ""},
  {"Maria", ""}
};

byte count = 1;                                   //Contador de usuários

void setup() {
  Serial.begin(9600);                             //Inicia o Monitor Serial
  SPI.begin();                                    //Inicia o SPI
  rfid.PCD_Init();                                //Inicia o leitor

  lcd.init();                                     //Inicia e liga a luz do display
  lcd.backlight();

  pinMode(lb, OUTPUT);                            //Define os pinos dos leds como saída e o do botão como saída
  pinMode(lv, OUTPUT);
  pinMode(b, INPUT_PULLUP);

  s.attach(4);                                    //Anexa o servo-motor ao pino 4
  s.write(0);                                     //Inicia o motor na posição 0
}

void loop() {
  String id = "";                                  //Cria a variável "id"
  
  //Se não houver nenhum cartão, não faz nada
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
     lcd.setCursor(3, 0);                          //Imprime a mensagem centralizada no display
     lcd.print("Aproxime o");
     lcd.setCursor(5, 1);
     lcd.print("cartao");

     while (!digitalRead(b)) {                    //Verifica se o botão foi pressionado
      adiciona_cartao(count, le_cartao());
     }
     return;
  } 
  
  id = le_cartao();                               //Chama a função para ler a uid   
  verifica_usuario(id);                           //Chama a função para verificar o usuário
}

String le_cartao() {  
  String _id = "";                                //Cria a variável "_id"
  for (byte i = 0; i < 4; i++) {                  //Loop for para percorrer os caracteres lidos, seguindo o formato proposto
    _id += (rfid.uid.uidByte[i] < 0x10 ? "0" : "") + String(rfid.uid.uidByte[i], HEX) + (i!=3 ? ":" : "");
  }
    
  _id.toUpperCase();                              //Transforma as letras em caixa alta
  rfid.PICC_HaltA();                              //Para a criptografia
  rfid.PCD_StopCrypto1(); 

  return _id;                                     //Retorna a uid lida
}

void pisca(byte pino) {                           
  for (byte i = 0; i < 5; i++) {                  //Lopp para repetir a função
    digitalWrite(pino, HIGH);                     //Liga o led
    delay(100);                                   //Espera 100 milissegundos
    digitalWrite(pino, LOW);                      //Desliga o led
    delay(100);                                   //Espera 100 milissegundos
  }
}

void verifica_usuario(String id) {
  //Define o número de elementos da matriz
  byte elementos = (sizeof(usuarios) / sizeof(usuarios[0]));
  lcd.clear();                                    //Limpa o display
  
  for (byte i = 0; i < elementos; i++) {      
    if (usuarios[i][1] == id) {                   //Verifica a UID
      lcd.setCursor(3, 0);
      lcd.print("Ola,");                          //Mensagem centralizada exibindo o nome do ususário
      lcd.print(usuarios[i][0]);
      lcd.setCursor(1, 1);
      lcd.print("Seja bem-vindo");
      
      s.write(90);                                //Ativa o servo
      pisca(lb);                                  //Pisca o led branco
      
      delay(3000);
      lcd.clear();
      s.write(0);
      return true;
    }
  }

  lcd.setCursor(3, 0);                            //Mensagem de cartão não reconhecido
  lcd.print("Cartao nao");
  lcd.setCursor(2, 1);
  lcd.print("reconhecido!");
  
  pisca(lv);                                      //Pisca o led vermelho
  delay(3000);
  
  lcd.clear();
  return false;
}

void adiciona_cartao(int local, String id) {
  //Define o número de elementos da matriz
  byte elementos = (sizeof(usuarios) / sizeof(usuarios[0]));
  
  for (byte i = 0; i < elementos; i++) {          //Loop for para verificar se a UID já foi utilizada
    if (usuarios[i][1] == id) {
      return;
    }
  }
  
  lcd.clear();                                    //Limpa o display
  lcd.setCursor(0, 0);
  lcd.print("Cartao de: ");
  lcd.print(usuarios[local][0]);
  Serial.println(id);
  delay(1000);

  if (id != "00:00:00:00") {
    usuarios[local][1] = id;                      //Adiciona a UID à matriz
    count += 1;                                   //Adiciona uma unidade ao contador
    delay(3000);    
 
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Cartao");
    lcd.setCursor(3, 1);
    lcd.print("adicionado");
    pisca(lb);                                    //Pisca os leds brancos
    delay(3000);
    lcd.clear();
  } else {
    lcd.clear();                                  //Limpa o display
    lcd.setCursor(6, 0);                          //Mensagem de erro centralizada
    lcd.print("ERRO");
    pisca(lv);                                    //Pisca o led vermelho
    delay(3000);
    lcd.clear();
  }
}
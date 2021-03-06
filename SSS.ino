#include <LiquidCrystal.h>
//LCD pin to Arduino
#define UP 1;
#define DOWN 2;
#define SHOOT 3;

const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10;

LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);
int score = 0;
int buzzerPin = 45;
int shootingSound = 1000;
int explosionSound = 500;
int soundCount = 0;
bool manLowLevel = true;
bool lost = false;
byte gameStateUpper[16] = {0, 0, 0, 0,   //0 represents empty space, except for the first byte which is the charachter
                      0, 0, 0, 0,   //1 represents a bullet 
                      0, 0, 0, 0,   //2 represent a spaceShip
                      0, 0, 0, 0};  //3 represents an explosion
byte gameStateLower[16] = {0, 0, 0, 0,
                          0, 0, 0, 0,
                          0, 0, 0, 0,
                          0, 0, 0, 2};
              
byte man [8] = {
  0b01110,
  0b01110,
  0b00100,
  0b01111,
  0b00100,
  0b01010,
  0b01010,
  0b01010
};

byte alienShip[8] = {
  0b00010,
  0b00110,
  0b01110,
  0b11111,
  0b11111,
  0b01110,
  0b00110,
  0b00010
};

byte bullet[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b01100,
  0b01100,
  0b00000,
  0b00000,
  0b00000
};

byte explosion[8] = {
  0b00000,
  0b00100,
  0b10101,
  0b01110,
  0b11111,
  0b01110,
  0b10101,
  0b00100
};


void setup() {
 pinMode(buzzerPin, OUTPUT);
 lcd.begin(16, 2);
 lcd.createChar(7, man);
 lcd.createChar(8, alienShip);
 lcd.createChar(9, bullet);
 lcd.createChar(10, explosion);
 lcd.setCursor(0,0);
 lcd.write(7);
 lcd.setCursor(0,1);
 lcd.print("DOWN to start");
}
void loop() {

 int action;
 action = analogRead (0);
 lcd.setCursor(10,1);
 if ((action > 200) && (action < 400)){
  startGame();
 }
} 

int getAction() {
  int action = 0;
  int readInput = analogRead(0);
  if (readInput < 60) {
    action = 3;
    tone(buzzerPin, 1000); // Send 1KHz sound signal...
    soundCount = 2;
  }
  else if(readInput < 200) {
    action = 1;
  }
  else if(readInput < 400) {
    action = 2;
  }
  return action;
}

void startGame() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.write(7);
  int count = 15;
  while(!lost){
    if (count == 0) {
      int spawnPlace = random(0, 2); //spawn a space ship
      if (spawnPlace == 0) {
        gameStateUpper[15] = 2;  
      } else {
        gameStateLower[15] = 2;
      }

      if (score > 40) { //based on the current score, decide when should the next space ship spawn
        count = random(9, 6);
      } else if (score > 20) {
        count = random(12, 6);
      } else {
        count = random(16, 9);
      }
      
    }
   processAction(getAction()); //process current action
   renderScreen(); //render the screen
   processNextState(); //process the state of the game for the next frame
   count --;
   //handleSound
   if(soundCount != 0){
      soundCount --;
   } else {
    noTone(buzzerPin);
   }
   delay(70);
  }
  showEndGame();
}
void resetState() {
  for(int i = 0; i < 16; i++) {
    gameStateUpper[i] = 0;
    gameStateLower[i] = 0;
  }
}
void showEndGame() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score:");
  lcd.print(score);
  if(getAction() == 2) {
    score = 0;
    lost = false;
    resetState();
    startGame();
  }
}
void processAction(int action){
  if (action == 1) {
    manLowLevel = false;
  } 
  if (action == 2) {
    manLowLevel = true;
  }
  if (action == 3) {
    if(manLowLevel) {
      if(gameStateLower[2] != 1) {
        gameStateLower[1] = 1;
      }
    } else {
      if(gameStateUpper[2] != 1) {
        gameStateUpper[1] = 1; 
      }
    }
  }
}

void renderScreen() {
  lcd.clear();
  if(manLowLevel){
    lcd.setCursor(0, 1);
    lcd.write(7);
  } else {
    lcd.setCursor(0, 0);
    lcd.write(7);
  }
  for(int i = 1; i < 16; i++) {
    if (gameStateUpper[i] == 0) {
      lcd.setCursor(i, 0);
      lcd.write(' ');
    }
    if (gameStateUpper[i] == 1) {
      lcd.setCursor(i, 0);
      lcd.write(9);
    }
    if (gameStateUpper[i] == 2) {
      lcd.setCursor(i, 0);
      lcd.write(8);
    }
    if (gameStateUpper[i] == 3) {
      lcd.setCursor(i, 0);
      lcd.write(10);
      tone(buzzerPin, explosionSound); // Send 1KHz sound signal...
      soundCount = 2;
    }
    if (gameStateLower[i] == 0) {
      lcd.setCursor(i, 1);
      lcd.write(' ');
    }
    if (gameStateLower[i] == 1) {
      lcd.setCursor(i, 1);
      lcd.write(9);
    }
    if (gameStateLower[i] == 2) {
      lcd.setCursor(i, 1);
      lcd.write(8);
    }
    if (gameStateLower[i] == 3) {
      lcd.setCursor(i, 1);
      lcd.write(10);
      tone(buzzerPin, explosionSound); // Send 1KHz sound signal...
      soundCount = 2;
    }
  }
}

void processNextState() {
  byte nextStateUpper[16] = {0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0,
                            0, 0, 0, 0};
  byte nextStateLower[16] = {0, 0, 0, 0,
                             0, 0, 0, 0,
                             0, 0, 0, 0,
                             0, 0, 0, 0,};

  for(int i = 1; i < 16; i++) {
    //process bullets
    if (gameStateUpper[i] == 1) { //bullet
      if(i == 15){
         
      }
      else if(i == 14) {
        if (gameStateUpper[15] == 2) { //last element on screen is a spaceship and bullet on position 14
          nextStateUpper[15] = 3;
          score++;
        } else {
          nextStateUpper[15] = 1;
        }
        continue;
      }
      else if(gameStateUpper[i + 1] == 2) { //next to a spaceship
        nextStateUpper[i + 1] = 3;
        score++;
        
      }
      else if(gameStateUpper[i + 1] == 1) { //next to an explosion
        nextStateUpper[i + 1] = 1;
      }
      else if(gameStateUpper[i + 2] == 2) {
        nextStateUpper[i + 1] = 3;
        score++;
      }
      else nextStateUpper[i + 1] = 1;
    }
    if (gameStateLower[i] == 1) { //bullet
      if(i == 15){
         
      }
      else if(i == 14) {
        if (gameStateLower[15] == 2) { //last element on screen is a spaceship and bullet on position 14
          nextStateLower[15] = 3;
          score++;
        } else {
          nextStateLower[15] = 1;
        }
        continue;
      }
      else if(gameStateLower[i + 1] == 2) { //next to a spaceship
        nextStateLower[i + 1] = 3;
        score++;
      }
      else if(gameStateLower[i + 1] == 1) { //next to an explosion
        nextStateLower[i + 1] = 1;
      }
      else if(gameStateLower[i + 2] == 2) {
        nextStateLower[i + 1] = 3;
        score++;
      }
      else nextStateLower[i + 1] = 1;
    }
    //done with bullets
    //spaceships
    if(gameStateUpper[i] == 2) {
      if(i == 1) {
        lost = true;
        return;
      }
      if((nextStateUpper[i - 1] == 0) && (nextStateUpper[i] != 3)) {
        nextStateUpper[i - 1] = 2;
      }
    }
    if(gameStateLower[i] == 2) {
      if(i == 1) {
        lost = true;
        return;
      }
      if((nextStateLower[i - 1] == 0) && (nextStateLower[i] != 3)) {
        nextStateLower[i - 1] = 2;
      }
    }
    //done with the ships
  }
  for(int i = 0; i < 16; i++) {
    gameStateUpper[i] = nextStateUpper[i];
    gameStateLower[i] = nextStateLower[i];
  }
}

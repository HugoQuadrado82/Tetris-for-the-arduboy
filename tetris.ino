#include <Arduboy2.h>
#include <Arduboy2Audio.h>
#include <Arduboy2Core.h>
#include <Sprites.h>
#include <ArduboyTones.h>

#include "Arduboy2.h";

static Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

int points[3] = {0,0,0};
int songNum = 1;
int boardWidth = 10;
int boardHeight = 21;
int gridWidth = 6.4;
int gravSpeed = 1.2;
int piecePivot[2] = {1,4};
int currentMinoCoords[4][2] = {{},{},{},{}};
char currentMino = 'o';
char nextMino = 'o';
bool showingNextPiece = false;
bool boardMap[21][10] = {{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}};
int threeSec = ((((1000/60)*round(60/gravSpeed))*1.024)*3)/2;
int twoSec = ((((1000/60)*round(60/gravSpeed))*1.024)*2)/2;
int oneHalfSec = ((((1000/60)*round(60/gravSpeed))*1.024)*1.5)/2;
int oneSec = (((1000/60)*round(60/gravSpeed))*1.024)/2;
int halfSec = ((((1000/60)*round(60/gravSpeed))*1.024)/2)/2;
uint16_t tetrisThemeSong1[] = {
  659.25,oneSec, 493.88,halfSec, 523.25,halfSec, 587.33,oneSec, 523.25, halfSec, 493.88, halfSec, 440.00, oneSec, 440.00, halfSec, 523.25,halfSec, 659.25,oneSec,587.33,halfSec,523.25,halfSec,493.88,oneSec, 493.88,halfSec,523.25,halfSec,587.33,oneSec,659.25,oneSec,523.25,oneSec,440.00,oneSec,440.00,oneSec,0,oneSec,587.33,oneHalfSec,698.46,halfSec,880.00,oneSec,783.99,halfSec,698.46,halfSec,659.25,oneHalfSec,523.25,halfSec,659.25,oneSec,587.33,halfSec,523.25,halfSec,493.88,oneSec,493.88,halfSec,523.25,halfSec,587.33,oneSec,659.25,oneSec,523.25,oneSec,440.00,oneSec,440.00,oneSec,0,oneSec,
  TONES_END };
uint16_t tetrisThemeSong2[] = {
  329.63,twoSec,261.63,twoSec,293.66,twoSec,246.94,twoSec,261.63,twoSec,220.00,twoSec,207.65,twoSec,246.94,twoSec,329.63,twoSec,261.63,twoSec,293.66,twoSec,246.94,twoSec,261.63,oneSec,329.63,oneSec,440.00,twoSec,415.30,threeSec,0,oneSec,
  TONES_END };

bool playMusic(float tempo){
    threeSec = ((((1000/60)*round(60/gravSpeed))*1.024)*3)/2;
    twoSec = ((((1000/60)*round(60/gravSpeed))*1.024)*2)/2;
    oneHalfSec = ((((1000/60)*round(60/gravSpeed))*1.024)*1.5)/2;
    oneSec = (((1000/60)*round(60/gravSpeed))*1.024)/2;
    halfSec = ((((1000/60)*round(60/gravSpeed))*1.024)/2)/2;
    switch(songNum) {
      case 1:
        sound.tonesInRAM(tetrisThemeSong1);
      break;
      case 2:
        sound.tonesInRAM(tetrisThemeSong1);
      break;
      default:
        sound.tonesInRAM(tetrisThemeSong2);
        songNum = 0;
      break;
    };
  };

void clearBoard() {
  for (int i=0; i < boardHeight; i++){
      for (int j=0; j < boardWidth; j++){
            boardMap[i][j] = 0;
      };
    };
};

void rotatePiece(char dir) {
  if (currentMino!='o') {
  piecePivot[0] = currentMinoCoords[0][0];
  piecePivot[1] = currentMinoCoords[0][1];
  int s = -1;
  int c = 0;
  for (int i=0; i < 4; i++){
  currentMinoCoords[i][0] -= piecePivot[0];
  currentMinoCoords[i][1] -= piecePivot[1];

  // rotate point
  int xnew = currentMinoCoords[i][0] * c - currentMinoCoords[i][1] * s;
  int ynew = currentMinoCoords[i][0] * s + currentMinoCoords[i][1] * c;

  // translate point back:
  currentMinoCoords[i][0] = xnew + piecePivot[0];
  currentMinoCoords[i][1] = ynew + piecePivot[1];
  }
  }
};

void drawBoard() {
   for (int i=0; i < boardHeight; i++){
      for (int j=0; j < boardWidth; j++){
        if (boardMap[i][j]) {
            arduboy.drawRect((128-(6.4))-(i*gridWidth),1+(j*gridWidth),gridWidth,gridWidth,WHITE);
        };
      };
    };
    for (int i=0; i < 4; i++){
      arduboy.drawRect((128-(6.4))-(currentMinoCoords[i][0]*gridWidth),1+(currentMinoCoords[i][1]*gridWidth),gridWidth,gridWidth,WHITE);
    };
    arduboy.drawFastVLine(0,0,61);
    arduboy.drawFastHLine(0,0,128);
    arduboy.drawFastHLine(0,61,128);
    arduboy.setCursor(45,0);
    arduboy.print(points[2]);
    arduboy.print(points[1]);
    arduboy.print(points[0]);
    if (showingNextPiece) showNextPiece();
    arduboy.display();
  };

int addPoint(int amnt) {
  points[0]+=amnt;
  gravSpeed += 1.0;
  if (points[0]>=10) {
      points[1]+=(floor(points[0]/boardWidth));
      points[0]=points[0]%boardWidth;
    };
  if (points[1]>=10) {
      points[2]+=(floor(points[1]/boardWidth));
      points[1]=points[1]%boardWidth;
    };
  };

void getNextMino(bool first) {
    switch (round(random(1,7))) {
      case 1:
        nextMino = 'i';
        if (first) currentMino = 'i';
      break;
      case 2:
        nextMino = 'l';
        if (first) currentMino = 'l';
      break;
      case 3:
        nextMino = 'j';
        if (first) currentMino = 'j';
      break;
      case 4:
        nextMino = 't';
        if (first) currentMino = 't';
      break;
      case 5:
        nextMino = 's';
        if (first) currentMino = 's';
      break;
      case 6:
        nextMino = 'z';
        if (first) currentMino = 'z';
      break;
      default:
        nextMino = '0';
        if (first) currentMino = '0';
      break;
      };
  }

void createPiece(char type) {
    switch (type) {
      case 'o':
        currentMinoCoords[0][0] = 0;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 1;
        currentMinoCoords[1][1] = 4;
        currentMinoCoords[2][0] = 0;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 1;
        currentMinoCoords[3][1] = 5;
      break;
      case 'l':
        currentMinoCoords[0][0] = 1;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 1;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 1;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 0;
        currentMinoCoords[3][1] = 5;
      break;
      case 'j':
        currentMinoCoords[0][0] = 1;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 1;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 1;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 0;
        currentMinoCoords[3][1] = 3;
      break;
      case 't':
        currentMinoCoords[0][0] = 1;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 1;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 1;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 0;
        currentMinoCoords[3][1] = 4;
      break;
      case 's':
        currentMinoCoords[0][0] = 1;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 0;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 1;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 0;
        currentMinoCoords[3][1] = 4;
      break;
      case 'z':
        currentMinoCoords[0][0] = 1;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 1;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 0;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 0;
        currentMinoCoords[3][1] = 4;
      break;
      case 'i':
        currentMinoCoords[0][0] = 0;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 0;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 0;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 0;
        currentMinoCoords[3][1] = 6;
      break;
    };
  };

bool isValidMove(int coords[4][2]) {
    for (int i=0; i < 4; i++){
        if (boardMap[coords[i][0]][coords[i][1]] == 1) return false;
        if (coords[i][0] >= boardHeight) return false;
        if (coords[i][1] >= boardWidth) return false;
        if (coords[i][1] < 0) return false;
    };
    return true;
  };

void addPiece() {
    for (int i=0; i < 4; i++){
      boardMap[currentMinoCoords[i][0]][currentMinoCoords[i][1]] = 1;
    };
  };

void testRows() {
    for (int i=0; i < boardHeight; i++) {
      if (isFull(i)) {removeRow(i);addPoint(1);};
      }
  };

bool isFull(int row) {
  int temp = 0;
    for (int i=0; i < boardWidth; i++){
          if (boardMap[row][i]) temp++;
        };
    if (temp >= boardWidth) return true;
    return false;
  };

void checkKeys() {
  arduboy.pollButtons();
  if (arduboy.pressed(LEFT_BUTTON)) updatePiece('d');
  if (arduboy.pressed(A_BUTTON)) {showingNextPiece = true;} else {showingNextPiece = false;}
  if (arduboy.justPressed(RIGHT_BUTTON)) rotatePiece('l');
  if (arduboy.justPressed(B_BUTTON)) pauseMenu();
  if (arduboy.justPressed(DOWN_BUTTON)) updatePiece('r');
  if (arduboy.justPressed(UP_BUTTON)) updatePiece('l');
  }

void removeRow(int row) {
  int tempBoardMap[boardWidth] = {0,0,0,0,0,0,0,0,0,0};
  int temp = 0;
  int skipper = 0;
  for (int i=0; i < boardHeight; i++){
    if (i == row) skipper++;
    for (int j=0; j < boardWidth; j++){
        temp = boardMap[i+skipper][j];
        boardMap[i][j] = tempBoardMap[j];
        tempBoardMap[j] = temp;
      };
    };
  }

void updatePiece(char dir) {
    int next_currentMinoCoords[4][2] = {};
    for (int i=0; i < 4; i++){
      for (int j=0; j < 2; j++){
        next_currentMinoCoords[i][j]=currentMinoCoords[i][j];
      };
    };
    
    switch (dir) {
      case ('d'):
        for (int i=0; i < 4; i++){
            boardMap[currentMinoCoords[i][0]][currentMinoCoords[i][1]] = 0;
            next_currentMinoCoords[i][0]=currentMinoCoords[i][0]+1;
        };
        if (isValidMove(next_currentMinoCoords)) {
          for (int i=0; i < 4; i++){
            for (int j=0; j < 2; j++){
              currentMinoCoords[i][j]=next_currentMinoCoords[i][j];
            };
          };
        } else {
          addPiece();
          createPiece(nextMino);
          if (!isValidMove(currentMinoCoords)) mainMenu();
          currentMino = nextMino;
          getNextMino(false);
        };
      break;
      case ('l'):
        for (int i=0; i < 4; i++){
            boardMap[currentMinoCoords[i][0]][currentMinoCoords[i][1]] = 0;
            next_currentMinoCoords[i][1]=currentMinoCoords[i][1]-1;
        };
        if (isValidMove(next_currentMinoCoords)) {
          for (int i=0; i < 4; i++){
            for (int j=0; j < 2; j++){
              currentMinoCoords[i][j]=next_currentMinoCoords[i][j];
            };
          };
        };
      break;
      case ('r'):
        for (int i=0; i < 4; i++){
            boardMap[currentMinoCoords[i][0]][currentMinoCoords[i][1]] = 0;
            next_currentMinoCoords[i][1]=currentMinoCoords[i][1]+1;
        };
        if (isValidMove(next_currentMinoCoords)) {
          for (int i=0; i < 4; i++){
            for (int j=0; j < 2; j++){
              currentMinoCoords[i][j]=next_currentMinoCoords[i][j];
            };
          };
        };
      break;
    };
  };

void mainMenu() {
  arduboy.pollButtons();
    while(!arduboy.justPressed(B_BUTTON)) {
        arduboy.pollButtons();
        arduboy.clear();
        arduboy.drawRect(0,0,128,64,WHITE);
        arduboy.display();
      }
      arduboy.pollButtons();
      getNextMino(true);
      getNextMino(false);
      createPiece(currentMino);
      clearBoard();
      points[0] = 0;
      points[1] = 0;
      points[2] = 0;
  }

void showNextPiece() {
    arduboy.fillRect(98,32,30,30,BLACK);
    arduboy.drawRect(98,32,30,30,WHITE);
    arduboy.display();
  };

void pauseMenu() {
  arduboy.pollButtons();
    while(!arduboy.justPressed(B_BUTTON)) {
        arduboy.pollButtons();
        arduboy.fillRect(50,5,20,54,BLACK);
        arduboy.drawRect(50,5,20,54,WHITE);
        arduboy.display();
        arduboy.idle();
      }
      arduboy.pollButtons();
  }

void setup() {
  arduboy.begin();
  arduboy.clear();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
  mainMenu();
  arduboy.audio.on();
  sound.volumeMode(VOLUME_ALWAYS_NORMAL);
  playMusic(1.0);
};

void loop() {
  if (!sound.playing()) {songNum++;playMusic(gravSpeed);};
  if (!arduboy.nextFrame()) {
    return;
  };
  checkKeys();
  arduboy.clear();
  drawBoard();
  if (arduboy.everyXFrames(round(60/gravSpeed))) updatePiece('d');
  testRows();
}

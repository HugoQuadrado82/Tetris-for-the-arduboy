#include <Arduboy2.h>
#include <Arduboy2Audio.h>
#include <Arduboy2Core.h>
#include <Sprites.h>
#include <ArduboyTones.h>
#include "glcdfont.c"

#include "Arduboy2.h";

static Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);

uint8_t points[3] = {0,0,0};
uint8_t songNum = 1;
uint8_t boardWidth = 10;
uint8_t boardHeight = 21;
int gridWidth = 6;
float gravSpeed = 1.2;
uint8_t piecePivot[2] = {1,4};
uint8_t currentMinoCoords[4][2] = {{},{},{},{}};
char currentMino = 'o';
char nextMino = 'o';
bool showingNextPiece = false;
bool muted = true;
uint8_t deletingRows[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
bool boardMap[21][10] = {{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}};
uint16_t threeSec = ((((1000/60)*round(60/gravSpeed))*1.024)*3);
uint16_t twoSec = ((((1000/60)*round(60/gravSpeed))*1.024)*2);
uint16_t oneHalfSec = ((((1000/60)*round(60/gravSpeed))*1.024)*1.5);
uint16_t oneSec = (((1000/60)*round(60/gravSpeed))*1.024);
uint16_t halfSec = ((((1000/60)*round(60/gravSpeed))*1.024)/2);
uint16_t tetrisThemeSong1[] = {
  659.25,oneSec, 493.88,halfSec, 523.25,halfSec, 587.33,oneSec, 523.25, halfSec, 493.88, halfSec, 440.00, oneSec, 440.00, halfSec, 523.25,halfSec, 659.25,oneSec,587.33,halfSec,523.25,halfSec,493.88,oneSec, 493.88,halfSec,523.25,halfSec,587.33,oneSec,659.25,oneSec,523.25,oneSec,440.00,oneSec,440.00,oneSec,0,oneSec,587.33,oneHalfSec,698.46,halfSec,880.00,oneSec,783.99,halfSec,698.46,halfSec,659.25,oneHalfSec,523.25,halfSec,659.25,oneSec,587.33,halfSec,523.25,halfSec,493.88,oneSec,493.88,halfSec,523.25,halfSec,587.33,oneSec,659.25,oneSec,523.25,oneSec,440.00,oneSec,440.00,oneSec,0,oneSec,
  TONES_END };
uint16_t tetrisThemeSong2[] = {
  329.63,twoSec,261.63,twoSec,293.66,twoSec,246.94,twoSec,261.63,twoSec,220.00,twoSec,207.65,twoSec,246.94,twoSec,329.63,twoSec,261.63,twoSec,293.66,twoSec,246.94,twoSec,261.63,oneSec,329.63,oneSec,440.00,twoSec,415.30,threeSec,0,oneSec,
  TONES_END };

void playMusic(){
  if (!muted) {
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
  };

void clearBoard() {
  for (uint8_t i=0; i < boardHeight; i++){
      for (uint8_t j=0; j < boardWidth; j++){
            boardMap[i][j] = 0;
      };
    };
};

//i used simple point rotation for each piece and ploted the points as each part of the tetramino
void rotatePiece(char dir) {
  if (currentMino!='o') {
  uint8_t tempMinoCoords[4][2] = {{},{},{},{}};
  piecePivot[0] = currentMinoCoords[0][0];
  piecePivot[1] = currentMinoCoords[0][1];
  for (uint8_t i=0; i < 4; i++){
    tempMinoCoords[i][0] = currentMinoCoords[i][0];
    tempMinoCoords[i][1] = currentMinoCoords[i][1];
  }
  int8_t s = -1;
  uint8_t c = 0;
  for (uint8_t i=0; i < 4; i++){
  tempMinoCoords[i][0] -= piecePivot[0];
  tempMinoCoords[i][1] -= piecePivot[1];

  // rotate point
  uint8_t xnew = tempMinoCoords[i][0] * c - tempMinoCoords[i][1] * s;
  uint8_t ynew = tempMinoCoords[i][0] * s + tempMinoCoords[i][1] * c;

  // translate point back:
  tempMinoCoords[i][0] = xnew + piecePivot[0];
  tempMinoCoords[i][1] = ynew + piecePivot[1];
  }
  if (isValidMove(tempMinoCoords)) {
    for (uint8_t i=0; i < 4; i++){
      currentMinoCoords[i][0] = tempMinoCoords[i][0];
      currentMinoCoords[i][1] = tempMinoCoords[i][1];
    }
   }
  }
};

//draws the game to the screen, pretty simple
void drawBoard() {
   for (uint8_t i=0; i < boardHeight; i++){
      for (uint8_t j=0; j < boardWidth; j++){
        if (boardMap[i][j]) {
            arduboy.drawRect((128-(6.4))-(i*gridWidth),1+(j*gridWidth),gridWidth,gridWidth,WHITE);
          if ((deletingRows[i]>10 && deletingRows[i]<20)||(deletingRows[i]>30 && deletingRows[i]<40)) {
            arduboy.fillRect((128-(6.4))-(i*gridWidth),1+(j*gridWidth),gridWidth,gridWidth,WHITE);
          };
        };
      };
    };
    for (uint8_t i=0; i < 4; i++){
      arduboy.drawRect((128-(6.4))-(currentMinoCoords[i][0]*gridWidth),1+(currentMinoCoords[i][1]*gridWidth),gridWidth,gridWidth,WHITE);
    };
    arduboy.drawFastVLine(0,0,61);
    arduboy.drawFastHLine(0,0,128);
    arduboy.drawFastHLine(0,61,128);
    drawChar_Rot90(127,14,scoreChar(2),WHITE,BLACK,1);
    drawChar_Rot90(127,21,scoreChar(1),WHITE,BLACK,1);
    drawChar_Rot90(127,28,scoreChar(0),WHITE,BLACK,1);
    //a girl that i tutor complained that the points looked low to her, so i just added two zeros to the end lol
    drawChar_Rot90(127,35,'0',WHITE,BLACK,1);
    drawChar_Rot90(127,42,'0',WHITE,BLACK,1);
    if (showingNextPiece) showNextPiece();
    arduboy.display();
  };

// since the screen is sidways, i plan to make a font and so by seperating the score into three seperate digits, putting the score on the screen will be much easier in the future
int addPoint(uint8_t amnt) {
  points[0]+=amnt;
  gravSpeed += 0.1;
  if (points[0]>=10) {
      points[1]+=(floor(points[0]/boardWidth));
      points[0]=points[0]%boardWidth;
    };
  if (points[1]>=10) {
      points[2]+=(floor(points[1]/boardWidth));
      points[1]=points[1]%boardWidth;
    };
  };

//just picks a random mino for either the first piece or the next piece
void getNextMino(bool first) {
    switch (round(random(1,8))) {
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
        nextMino = 'o';
        if (first) currentMino = 'o';
      break;
      };
  }

//get the points for each type of tetramino
void createPiece(char type) {
    switch (type) {
      case 'o':
        currentMinoCoords[0][0] = 1;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 2;
        currentMinoCoords[1][1] = 4;
        currentMinoCoords[2][0] = 1;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 2;
        currentMinoCoords[3][1] = 5;
      break;
      case 'l':
        currentMinoCoords[0][0] = 2;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 2;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 2;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 1;
        currentMinoCoords[3][1] = 5;
      break;
      case 'j':
        currentMinoCoords[0][0] = 2;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 2;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 2;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 1;
        currentMinoCoords[3][1] = 3;
      break;
      case 't':
        currentMinoCoords[0][0] = 2;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 2;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 2;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 1;
        currentMinoCoords[3][1] = 4;
      break;
      case 's':
        currentMinoCoords[0][0] = 2;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 1;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 2;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 1;
        currentMinoCoords[3][1] = 4;
      break;
      case 'z':
        currentMinoCoords[0][0] = 2;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 2;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 1;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 1;
        currentMinoCoords[3][1] = 4;
      break;
      case 'i':
        currentMinoCoords[0][0] = 1;
        currentMinoCoords[0][1] = 4;
        currentMinoCoords[1][0] = 1;
        currentMinoCoords[1][1] = 3;
        currentMinoCoords[2][0] = 1;
        currentMinoCoords[2][1] = 5;
        currentMinoCoords[3][0] = 1;
        currentMinoCoords[3][1] = 6;
      break;
    };
  };

//checks if the move is valid and returns the answer
bool isValidMove(uint8_t coords[4][2]) {
    for (uint8_t i=0; i < 4; i++){
        if (boardMap[coords[i][0]][coords[i][1]] == 1) return false;
        if (coords[i][0] >= boardHeight) return false;
        if (coords[i][1] >= boardWidth) return false;
        if (coords[i][1] < 0) return false;
    };
    return true;
  };

//overlay the piece on the board, i keep the board and the piece seperate until the piece lands
void addPiece() {
    for (uint8_t i=0; i < 4; i++){
      boardMap[currentMinoCoords[i][0]][currentMinoCoords[i][1]] = 1;
    };
  };

//test each row and returns which row is full
void testRows() {
    for (uint8_t i=0; i < boardHeight; i++) {
      if (isFull(i)) {getReadyToRemove(i);};
      }
  };

//actually tests a row to see if it is full
bool isFull(uint8_t row) {
  uint8_t temp = 0;
    for (uint8_t i=0; i < boardWidth; i++){
          if (boardMap[row][i]) temp++;
        };
    if (temp >= boardWidth) return true;
    return false;
  };

//just checks the keys and does what is needed accordingly
void checkKeys() {
  arduboy.pollButtons();
  if (arduboy.pressed(LEFT_BUTTON)) updatePiece('d');
  if (arduboy.pressed(A_BUTTON)) {showingNextPiece = true;} else {showingNextPiece = false;}
  if (arduboy.justPressed(RIGHT_BUTTON)) rotatePiece('l');
  if (arduboy.justPressed(B_BUTTON)) pauseMenu();
  if (arduboy.justPressed(DOWN_BUTTON)) updatePiece('r');
  if (arduboy.justPressed(UP_BUTTON)) updatePiece('l');
  }

void getReadyToRemove(uint8_t row) {
  if (deletingRows[row]<40) deletingRows[row]++;
  if (deletingRows[row]>=40) {removeRow(row);deletingRows[row]=0;}
}

//removes the row that is specified
void removeRow(uint8_t row) {
  bool tempBoardMap[boardWidth] = {0,0,0,0,0,0,0,0,0,0};
  uint8_t temp = 0;
  bool skipper = 0;
  for (uint8_t i=0; i < boardHeight; i++){
    if (i == row) skipper++;
    for (uint8_t j=0; j < boardWidth; j++){
        temp = boardMap[i+skipper][j];
        boardMap[i][j] = tempBoardMap[j];
        tempBoardMap[j] = temp;
      };
    };
  addPoint(1);
  }

//moves a piece in the selected direction if possible
void updatePiece(char dir) {
    uint8_t next_currentMinoCoords[4][2] = {};
    for (uint8_t i=0; i < 4; i++){
      for (uint8_t j=0; j < 2; j++){
        next_currentMinoCoords[i][j]=currentMinoCoords[i][j];
      };
    };
    
    switch (dir) {
      case ('d'):
        for (uint8_t i=0; i < 4; i++){
            boardMap[currentMinoCoords[i][0]][currentMinoCoords[i][1]] = 0;
            next_currentMinoCoords[i][0]=currentMinoCoords[i][0]+1;
        };
        if (isValidMove(next_currentMinoCoords)) {
          for (uint8_t i=0; i < 4; i++){
            for (uint8_t j=0; j < 2; j++){
              currentMinoCoords[i][j]=next_currentMinoCoords[i][j];
            };
          };
        } else {
          addPiece();
          createPiece(nextMino);
          if (!isValidMove(currentMinoCoords)) loseMenu();
          currentMino = nextMino;
          getNextMino(false);
        };
      break;
      case ('l'):
        for (uint8_t i=0; i < 4; i++){
            boardMap[currentMinoCoords[i][0]][currentMinoCoords[i][1]] = 0;
            next_currentMinoCoords[i][1]=currentMinoCoords[i][1]-1;
        };
        if (isValidMove(next_currentMinoCoords)) {
          for (uint8_t i=0; i < 4; i++){
            for (uint8_t j=0; j < 2; j++){
              currentMinoCoords[i][j]=next_currentMinoCoords[i][j];
            };
          };
        };
      break;
      case ('r'):
        for (uint8_t i=0; i < 4; i++){
            boardMap[currentMinoCoords[i][0]][currentMinoCoords[i][1]] = 0;
            next_currentMinoCoords[i][1]=currentMinoCoords[i][1]+1;
        };
        if (isValidMove(next_currentMinoCoords)) {
          for (uint8_t i=0; i < 4; i++){
            for (uint8_t j=0; j < 2; j++){
              currentMinoCoords[i][j]=next_currentMinoCoords[i][j];
            };
          };
        };
      break;
    };
  };

//displays the main menu
void mainMenu() {
  arduboy.pollButtons();
    while(!arduboy.justPressed(B_BUTTON)) {
        arduboy.pollButtons();
        arduboy.clear();
        arduboy.drawRect(0,0,128,64,WHITE);
        drawChar_Rot90(24,16,'M',WHITE,BLACK,1);
        drawChar_Rot90(24,23,'U',WHITE,BLACK,1);
        drawChar_Rot90(24,30,'S',WHITE,BLACK,1);
        drawChar_Rot90(24,37,'I',WHITE,BLACK,1);
        drawChar_Rot90(24,44,'C',WHITE,BLACK,1);
        drawChar_Rot90(15,40,'O',WHITE,BLACK,1);
        drawChar_Rot90(15,47,'F',WHITE,BLACK,1);
        drawChar_Rot90(15,54,'F',WHITE,BLACK,1);
        drawChar_Rot90(15,10,'O',WHITE,BLACK,1);
        drawChar_Rot90(15,17,'N',WHITE,BLACK,1);
        if (arduboy.justPressed(DOWN_BUTTON)) {arduboy.audio.off();muted = true;};
        if (arduboy.justPressed(UP_BUTTON)) {arduboy.audio.on();muted = false;}
        if (!muted) arduboy.drawTriangle(10,5,12,8,14,5);
        if (muted) arduboy.drawTriangle(10,35,12,38,14,35);
        arduboy.display();
        arduboy.idle();
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

//i will make the next piece seen when this function is called, not finished quite yet though
void showNextPiece() {
    arduboy.fillRect(98,32,30,30,BLACK);
    arduboy.drawRect(98,32,30,30,WHITE);
    arduboy.display();
  };

//pauses the game and displays the pause menu
void pauseMenu() {
  arduboy.pollButtons();
    while(!arduboy.justPressed(B_BUTTON)) {
        arduboy.pollButtons();
        arduboy.fillRect(50,5,20,54,BLACK);
        arduboy.drawRect(50,5,20,54,WHITE);
        drawChar_Rot90(62,12,'P',WHITE,BLACK,1);
        drawChar_Rot90(62,19,'A',WHITE,BLACK,1);
        drawChar_Rot90(62,26,'U',WHITE,BLACK,1);
        drawChar_Rot90(62,33,'S',WHITE,BLACK,1);
        drawChar_Rot90(62,40,'E',WHITE,BLACK,1);
        drawChar_Rot90(62,47,'D',WHITE,BLACK,1);
        arduboy.display();
        arduboy.idle();
      }
      arduboy.pollButtons();
  }

char scoreChar(uint8_t place) {
    switch (points[place]) {
      case 0:
        return '0';
      break;
      case 1:
        return '1';
      break;
      case 2:
        return '2';
      break;
      case 3:
        return '3';
      break;
      case 4:
        return '4';
      break;
      case 5:
        return '5';
      break;
      case 6:
        return '6';
      break;
      case 7:
        return '7';
      break;
      case 8:
        return '8';
      break;
      case 9:
        return '9';
      break;
     }
  }

//will show the losing screen
void loseMenu() {
  sound.noTone();
  arduboy.pollButtons();
    while(!arduboy.justPressed(B_BUTTON)) {
        arduboy.pollButtons();
        arduboy.clear();
        arduboy.drawRect(0,0,128,64,WHITE);
        drawChar_Rot90(120,8,'Y',WHITE,BLACK,1);
        drawChar_Rot90(120,14,'O',WHITE,BLACK,1);
        drawChar_Rot90(120,20,'U',WHITE,BLACK,1);
        drawChar_Rot90(120,28,'L',WHITE,BLACK,1);
        drawChar_Rot90(120,34,'O',WHITE,BLACK,1);
        drawChar_Rot90(120,40,'S',WHITE,BLACK,1);
        drawChar_Rot90(120,46,'E',WHITE,BLACK,1);
        drawChar_Rot90(120,52,'!',WHITE,BLACK,1);
        drawChar_Rot90(100,14,'S',WHITE,BLACK,1);
        drawChar_Rot90(100,21,'C',WHITE,BLACK,1);
        drawChar_Rot90(100,28,'O',WHITE,BLACK,1);
        drawChar_Rot90(100,35,'R',WHITE,BLACK,1);
        drawChar_Rot90(100,42,'E',WHITE,BLACK,1);
        drawChar_Rot90(100,47,':',WHITE,BLACK,1);
        drawChar_Rot90(90,14,scoreChar(2),WHITE,BLACK,1);
        drawChar_Rot90(90,21,scoreChar(1),WHITE,BLACK,1);
        drawChar_Rot90(90,28,scoreChar(0),WHITE,BLACK,1);
        drawChar_Rot90(90,35,'0',WHITE,BLACK,1);
        drawChar_Rot90(90,42,'0',WHITE,BLACK,1);
        arduboy.display();
        arduboy.idle();
      }
      mainMenu();
  }

void drawChar_Rot90
(int16_t x, int16_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size)
{
  boolean draw_background = bg != color;

  if ((x > WIDTH) ||              // Clip right   ;this line is modified
    (y > (HEIGHT + 5 * size)) ||  // Clip bottom  ;this line is modified
    ((x - 8 * size - 1) < 0) ||   // Clip left    ;this line is modified
    ((y + 5 * size - 1) < 0)      // Clip top     ;this line is modified
  )
  {
    return;
  }

  for (int8_t i=0; i<6; i++ )
  {
    uint8_t line;
    if (i == 5)
    {
      line = 0x0;
    }
    else
    {
      line = pgm_read_byte(font+(c*5)+i);
    }

    for (int8_t j = 0; j<8; j++)
    {
      uint8_t draw_color = (line & 0x1) ? color : bg;

      if (draw_color || draw_background) {
        for (uint8_t a = 0; a < size; a++ ) {
          for (uint8_t b = 0; b < size; b++ ) {
            arduboy.drawPixel( x - (j * size) - b, y + (i * size) + a, draw_color); //  this line is modified
          }
        }
      }
      line >>= 1;
    }
  }
}

void setup() {
  arduboy.begin();
  arduboy.clear();
  arduboy.setFrameRate(60);
  arduboy.initRandomSeed();
  arduboy.audio.off();
  sound.volumeMode(VOLUME_ALWAYS_NORMAL);
  mainMenu();
  playMusic();
};

void loop() {
  if (!sound.playing()&&!muted) {songNum++;playMusic();};
  if (!arduboy.nextFrame()) {
    return;
  };
  checkKeys();
  arduboy.clear();
  drawBoard();
  if (arduboy.everyXFrames(round(60/gravSpeed))) updatePiece('d');
  testRows();
}

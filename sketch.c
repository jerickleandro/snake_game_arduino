#include <LedControl.h>

int buzzer = 12;

struct Pin {
  static const short joystickX = A0;   
  static const short joystickY = A1;   
  static const short joystickVCC = 7; 


  static const short CLK = 10;   
  static const short CS  = 11;  
  static const short DIN = 12; 
};

const short intensity = 3;

const short messageSpeed = 5;

const short initialSnakeLength = 2;

int notaInicial = 0;
//melodia do Super Mario World
int melodia[] = {660,660,660,510,660,770,380,510,380,320,440,480,450,430,380,660,760,860,700,760,660,520,580,480,510,380,320,440,480,450,430,380,660,760,860,700,760,660,520,580,480,500,760,720,680,620,650,380,430,500,430,500,570,500,760,720,680,620,650,1020,1020,1020,380,500,760,720,680,620,650,380,430,500,430,500,570,585,550,500,380,500,500,500,500,760,720,680,620,650,380,430,500,430,500,570,500,760,720,680,620,650,1020,1020,1020,380,500,760,720,680,620,650,380,430,500,430,500,570,585,550,500,380,500,500,500,500,500,500,500,580,660,500,430,380,500,500,500,500,580,660,870,760,500,500,500,500,580,660,500,430,380,660,660,660,510,660,770,380};

//duraçao de cada nota em Milisegundos
int duracaodasnotas[] = {100,100,100,100,100,100,100,100,100,100,100,80,100,100,100,80,50,100,80,50,80,80,80,80,100,100,100,100,80,100,100,100,80,50,100,80,50,80,80,80,80,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,150,200,80,80,80,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,150,200,80,80,80,100,100,100,100,100,150,150,100,100,100,100,100,100,100,100,100,100,100,100,100,60,80,60,80,80,80,80,80,80,60,80,60,80,80,80,80,80,60,80,60,80,80,80,80,80,80,100,100,100,100,100,100,100};




void setup() {
  Serial.begin(115200);  
  initialize();         
  calibrateJoystick(); 
  showSnakeMessage(); 
}


void loop() {
    //tocarMusica();
  generateFood();   
  scanJoystick();    
  calculateSnake();  
  handleGameStates();
  

 
}


LedControl matrix(Pin::DIN, Pin::CLK, Pin::CS, 1);

struct Point {
  int row = 0, col = 0;
  Point(int row = 0, int col = 0): row(row), col(col) {}
};

struct Coordinate {
  int x = 0, y = 0;
  Coordinate(int x = 0, int y = 0): x(x), y(y) {}
};

bool win = false;
bool gameOver = false;


Point snake;


Point food(-1, -1);


Coordinate joystickHome(500, 500);

// snake parameters
int snakeLength = initialSnakeLength; 
int snakeSpeed = 100; 
int snakeDirection = 0; 

// direction constants
const short up     = 1;
const short right  = 2;
const short down   = 3; 
const short left   = 4; 

const int joystickThreshold = 160;


int gameboard[8][8] = {};






void generateFood() {
  if (food.row == -1 || food.col == -1) {
   
    if (snakeLength >= 64) {
      win = true;
      return; 
    }

    
    do {
      food.col = random(8);
      food.row = random(8);
    } while (gameboard[food.row][food.col] > 0);
  }
}



void scanJoystick() {
  int previousDirection = snakeDirection; 
  long timestamp = millis();

  while (millis() < timestamp + snakeSpeed) {
   
    snakeSpeed = 100;
    
    analogRead(Pin::joystickY) < joystickHome.y - joystickThreshold ? snakeDirection = up    : 0;
    analogRead(Pin::joystickY) > joystickHome.y + joystickThreshold ? snakeDirection = down  : 0;
    analogRead(Pin::joystickX) < joystickHome.x - joystickThreshold ? snakeDirection = left  : 0;
    analogRead(Pin::joystickX) > joystickHome.x + joystickThreshold ? snakeDirection = right : 0;

    
    snakeDirection + 2 == previousDirection && previousDirection != 0 ? snakeDirection = previousDirection : 0;
    snakeDirection - 2 == previousDirection && previousDirection != 0 ? snakeDirection = previousDirection : 0;

    
    matrix.setLed(0, food.row, food.col, millis() % 100 < 50 ? 1 : 0);
  }
}


void calculateSnake() {
  switch (snakeDirection) {
    case up:
      snake.row--;
      fixEdge();
      matrix.setLed(0, snake.row, snake.col, 1);
      break;

    case right:
      snake.col++;
      fixEdge();
      matrix.setLed(0, snake.row, snake.col, 1);
      break;

    case down:
      snake.row++;
      fixEdge();
      matrix.setLed(0, snake.row, snake.col, 1);
      break;

    case left:
      snake.col--;
      fixEdge();
      matrix.setLed(0, snake.row, snake.col, 1);
      break;

    default: 
      return;
  }

  if (gameboard[snake.row][snake.col] > 1 && snakeDirection != 0) {
    gameOver = true;
    return;
  }

  
  if (snake.row == food.row && snake.col == food.col) {
    food.row = -1; 
    food.col = -1;

   
    snakeLength++;

    
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        if (gameboard[row][col] > 0 ) {
          gameboard[row][col]++;
        }
      }
    }
  }


  gameboard[snake.row][snake.col] = snakeLength + 1;

  
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
     
      if (gameboard[row][col] > 0 ) {
        gameboard[row][col]--;
      }

      
      matrix.setLed(0, row, col, gameboard[row][col] == 0 ? 0 : 1);
    }
  }
}


void fixEdge() {
  snake.col < 0 ? snake.col += 8 : 0;
  snake.col > 7 ? snake.col -= 8 : 0;
  snake.row < 0 ? snake.row += 8 : 0;
  snake.row > 7 ? snake.row -= 8 : 0;
}


void handleGameStates() {
  if (gameOver || win) {
    unrollSnake();

    showScoreMessage(snakeLength - initialSnakeLength);

    if (gameOver) showGameOverMessage();
    else if (win) showWinMessage();

   
    win = false;
    gameOver = false;
    snake.row = random(8);
    snake.col = random(8);
    food.row = -1;
    food.col = -1;
    snakeLength = initialSnakeLength;
    snakeDirection = 0;
    memset(gameboard, 0, sizeof(gameboard[0][0]) * 8 * 8);
    matrix.clearDisplay(0);
  }
}


void unrollSnake() {
  matrix.setLed(0, food.row, food.col, 0);

  delay(800);

  for (int i = 0; i < 5; i++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        matrix.setLed(0, row, col, gameboard[row][col] == 0 ? 1 : 0);
      }
    }

    delay(20);

   
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        matrix.setLed(0, row, col, gameboard[row][col] == 0 ? 0 : 1);
      }
    }

    delay(50);

  }


  delay(600);

  for (int i = 1; i <= snakeLength; i++) {
    for (int row = 0; row < 8; row++) {
      for (int col = 0; col < 8; col++) {
        if (gameboard[row][col] == i) {
          matrix.setLed(0, row, col, 0);
          delay(100);
        }
      }
    }
  }
}

void calibrateJoystick() {
  Coordinate values;

  for (int i = 0; i < 10; i++) {
    values.x += analogRead(Pin::joystickX);
    values.y += analogRead(Pin::joystickY);
  }

  joystickHome.x = values.x / 10;
  joystickHome.y = values.y / 10;
}


void initialize() {
  pinMode(Pin::joystickVCC, OUTPUT);
  digitalWrite(Pin::joystickVCC, HIGH);


  matrix.shutdown(0, false);
  matrix.setIntensity(0, intensity);
  matrix.clearDisplay(0);

  randomSeed(analogRead(A5));
  snake.row = random(8);
  snake.col = random(8);
}


void dumpGameBoard() {
  String buff = "\n\n\n";
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      if (gameboard[row][col] < 10) buff += " ";
      if (gameboard[row][col] != 0) buff += gameboard[row][col];
      else if (col == food.col && row == food.row) buff += "@";
      else buff += "-";
      buff += " ";
    }
    buff += "\n";
  }
  Serial.println(buff);
}







const PROGMEM bool snakeMessage[8][56] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

const PROGMEM bool gameOverMessage[8][90] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

const PROGMEM bool scoreMessage[8][58] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

const PROGMEM bool digits[][8][8] = {
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 1, 1, 1, 0},
    {0, 1, 1, 1, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}
  },
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0}
  },
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0}
  },
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}
  },
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0},
    {0, 0, 1, 0, 1, 1, 0, 0},
    {0, 1, 0, 0, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0}
  },
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}
  },
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}
  },
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0}
  },
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}
  },
  {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}
  }
};


void showSnakeMessage() {
  [&] {
    for (int d = 0; d < sizeof(snakeMessage[0]) - 7; d++) {
      for (int col = 0; col < 8; col++) {
        delay(messageSpeed);
        for (int row = 0; row < 8; row++) {
          // this reads the byte from the PROGMEM and displays it on the screen
          matrix.setLed(0, row, col, pgm_read_byte(&(snakeMessage[row][col + d])));
        }
      }

      if (analogRead(Pin::joystickY) < joystickHome.y - joystickThreshold
              || analogRead(Pin::joystickY) > joystickHome.y + joystickThreshold
              || analogRead(Pin::joystickX) < joystickHome.x - joystickThreshold
              || analogRead(Pin::joystickX) > joystickHome.x + joystickThreshold) {
        return; // return the lambda function
      }
    }
  }();

  matrix.clearDisplay(0);

  while (analogRead(Pin::joystickY) < joystickHome.y - joystickThreshold
          || analogRead(Pin::joystickY) > joystickHome.y + joystickThreshold
          || analogRead(Pin::joystickX) < joystickHome.x - joystickThreshold
          || analogRead(Pin::joystickX) > joystickHome.x + joystickThreshold) {}

}


void showGameOverMessage() {
  [&] {
    for (int d = 0; d < sizeof(gameOverMessage[0]) - 7; d++) {
      for (int col = 0; col < 8; col++) {
        delay(messageSpeed);
        for (int row = 0; row < 8; row++) {
          matrix.setLed(0, row, col, pgm_read_byte(&(gameOverMessage[row][col + d])));
        }
      }

      if (analogRead(Pin::joystickY) < joystickHome.y - joystickThreshold
              || analogRead(Pin::joystickY) > joystickHome.y + joystickThreshold
              || analogRead(Pin::joystickX) < joystickHome.x - joystickThreshold
              || analogRead(Pin::joystickX) > joystickHome.x + joystickThreshold) {
        return; 
      }
    }
  }();

  matrix.clearDisplay(0);

  while (analogRead(Pin::joystickY) < joystickHome.y - joystickThreshold
          || analogRead(Pin::joystickY) > joystickHome.y + joystickThreshold
          || analogRead(Pin::joystickX) < joystickHome.x - joystickThreshold
          || analogRead(Pin::joystickX) > joystickHome.x + joystickThreshold) {}

}


void showWinMessage() {
}


void showScoreMessage(int score) {
  if (score < 0 || score > 99) return;

  // specify score digits
  int second = score % 10;
  int first = (score / 10) % 10;

  [&] {
    for (int d = 0; d < sizeof(scoreMessage[0]) + 2 * sizeof(digits[0][0]); d++) {
      for (int col = 0; col < 8; col++) {
        delay(messageSpeed);
        for (int row = 0; row < 8; row++) {
          if (d <= sizeof(scoreMessage[0]) - 8) {
            matrix.setLed(0, row, col, pgm_read_byte(&(scoreMessage[row][col + d])));
          }

          int c = col + d - sizeof(scoreMessage[0]) + 6;
          if (score < 10) c += 8;

          if (c >= 0 && c < 8) {
            if (first > 0) matrix.setLed(0, row, col, pgm_read_byte(&(digits[first][row][c]))); 
          } else {
            c -= 8;
            if (c >= 0 && c < 8) {
              matrix.setLed(0, row, col, pgm_read_byte(&(digits[second][row][c]))); 
            }
          }
        }
      }

      // if the joystick is moved, exit the message
      if (analogRead(Pin::joystickY) < joystickHome.y - joystickThreshold
              || analogRead(Pin::joystickY) > joystickHome.y + joystickThreshold
              || analogRead(Pin::joystickX) < joystickHome.x - joystickThreshold
              || analogRead(Pin::joystickX) > joystickHome.x + joystickThreshold) {
        return; // return the lambda function
      }
    }
  }();

  matrix.clearDisplay(0);

}


// standard map function, but with floats
float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void tocarMusica(){
    int nota = notaInicial;
    int duracaodanota = duracaodasnotas[nota];
 tone(buzzer, melodia[nota],duracaodanota);
 //pausa depois das notas
 int pausadepoisdasnotas[] ={150,300,300,100,300,550,575,450,400,500,300,330,150,300,200,200,150,300,150,350,300,150,150,500,450,400,500,300,330,150,300,200,200,150,300,150,350,300,150,150,500,300,100,150,150,300,300,150,150,300,150,100,220,300,100,150,150,300,300,300,150,300,300,300,100,150,150,300,300,150,150,300,150,100,420,450,420,360,300,300,150,300,300,100,150,150,300,300,150,150,300,150,100,220,300,100,150,150,300,300,300,150,300,300,300,100,150,150,300,300,150,150,300,150,100,420,450,420,360,300,300,150,300,150,300,350,150,350,150,300,150,600,150,300,350,150,150,550,325,600,150,300,350,150,350,150,300,150,600,150,300,300,100,300,550,575};
 delay(pausadepoisdasnotas[nota]);
 notaInicial++;
 if(nota == 155){
     notaInicial = 0;
 }
  Serial.print("notaInicial: ");
  Serial.println(notaInicial);
}
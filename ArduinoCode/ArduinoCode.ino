#include "gameEngine.h"


String currentButton = "N/A";

bool engineState = false;

const byte neoPixelPin = 6;

void pop_back(String& s) {
  if(s.length() == 0) return;
  s.remove(s.length() - 1);
}

void serialHandler() {
  if(Serial.available()) {
    String command =  Serial.readStringUntil('\n');
    pop_back(command);
    if(command.length() < 2) return;
    if(command[0] == '/') {
      switch(command[1]) {
        case 'P': {
          if(command.length() < 3)
            return;
          engineState = (command[2] == '1');
          break;
        }
        case 'E': {
          if(command.length() < 5)
            return;
          currentButton = command[2];
          currentButton += command[3];
          currentButton += command[4];
          break;
        }
          
      }
    }
  }
}

// Constants for the game
const byte SCREEN_HEIGHT = 8;
const byte SCREEN_WIDTH = 8;
const byte PADDLE_HEIGHT = 2;
const byte PADDLE_WIDTH = 1;
const byte BALL_SIZE = 1;

bool multiplayer = false; // Set to true for multiplayer, false for AI control

Window gameWindow(SCREEN_HEIGHT, SCREEN_WIDTH, neoPixelPin); // PIN 6 for NeoPixel matrix


// Create sprites for paddles and ball
Color paddleColor(0, 255, 0); // Green color for paddles
Color paddle2AIColor(255, 0, 0); // Green color for paddles
Color paddle2P2Color(0, 0, 255); // Green color for paddles
Color ballColor(255, 255, 0); // Yellow color for ball

byte paddleAlpha[PADDLE_HEIGHT][PADDLE_WIDTH] = {
  {255},
  {255}
};

Color paddleSprite[PADDLE_HEIGHT][PADDLE_WIDTH] = {
  {paddleColor},
  {paddleColor}
};
Color paddleSprite2[PADDLE_HEIGHT][PADDLE_WIDTH] = {
  {paddle2AIColor},
  {paddle2AIColor}
};
Color paddleSprite3[PADDLE_HEIGHT][PADDLE_WIDTH] = {
  {paddle2P2Color},
  {paddle2P2Color}
};

Sprite pad2AIs = Sprite(paddleSprite2, paddleAlpha);
Sprite pad2Ps = Sprite(paddleSprite3, paddleAlpha);

byte ballAlpha[BALL_SIZE][BALL_SIZE] = {
  {255}
};

Color ballSprite[BALL_SIZE][BALL_SIZE] = {
  {ballColor}
};

const Color backgroundColor = {0, 0, 0};

// Create the paddles and ball
gameObject player1(Box<float>(0, 0, PADDLE_HEIGHT-1, PADDLE_WIDTH-1), Sprite(paddleSprite, paddleAlpha), SCREEN_HEIGHT/2.f, 0);
gameObject player2(Box<float>(0, 0, PADDLE_HEIGHT-1, SCREEN_WIDTH - 1), pad2AIs, SCREEN_HEIGHT/2.0, SCREEN_WIDTH-1);
gameObject ball(Box<float>(0, 0, BALL_SIZE-1, BALL_SIZE-1), Sprite(ballSprite, ballAlpha), SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2);

// Initialize game variables
const float ballSpeedX = 8.5f, ballSpeedY = 8.5f;
float p1Speed = 6.5f, p2Speed = multiplayer ? p1Speed : p1Speed-1;

bool prevMulti = multiplayer;

void setup() {
  Serial.begin(115200);

  gameWindow.setBackgroundColor(backgroundColor);
  gameWindow.setBrightness(0.1);
  ball.vx = ballSpeedX, ball.vy = ballSpeedY;

  if(multiplayer)
    player2.sprite = pad2Ps;
}

bool p1U = false, p1D = false, p2U = false, p2D = false;

void controls() {
  if(currentButton != "N/A") {
    int player = currentButton[0] - '0'+1;
    char button = currentButton[1];
    bool state = (currentButton[2] == '1');

    switch(button) {
      case 'M':
        if(state)
          multiplayer = !multiplayer;
        break;
      
      case 'U':
        if(player == 1)
          p1U = state;
        else if(player == 2)
          p2U = state;

        break;
      
      case 'D':
        if(player == 1)
          p1D = state;
        else if(player == 2)
          p2D = state;
        break;
    }

    currentButton = "N/A";
  }
}

void movement() {
  // Player 1 movement
      if (p1U && player1.x > 0) {
        player1.vx = -p1Speed;
      } else if (p1D == HIGH && player1.x < SCREEN_HEIGHT - PADDLE_HEIGHT) {
        player1.vx = p1Speed;
      } else {
        player1.vx = 0;
      }

    // Player 2 movement
    if (multiplayer) {
        // Human control for player 2
        if (p2U == HIGH && player2.x > 0) {
          player2.vx = -p2Speed;
        } else if (p2D && player2.x < SCREEN_HEIGHT - PADDLE_HEIGHT) {
          player2.vx = p2Speed;
        } else {
          player2.vx = 0;
        }
    } else {
      // Simple AI control for player 2
      if (ball.x < player2.x && player2.x > 0) {
        player2.vx = -p2Speed;
      } else if (ball.x > player2.x + PADDLE_HEIGHT - 1 && player2.x < SCREEN_HEIGHT - PADDLE_HEIGHT) {
        player2.vx = p2Speed;
      } else {
        player2.vx = 0;
      }
    }


    player1.x += player1.vx*dt;
  player2.x += player2.vx*dt;
  if(player1.intersects(ball))
    player1.vx = 0;
  if(player2.intersects(ball))
    player2.vx = 0;
  player1.x -= player1.vx*dt;
  player2.x -= player2.vx*dt;
}

void ballMovement() {
  // Ball collision with top and bottom walls
  if (ball.x <= 0 || ball.x >= SCREEN_HEIGHT - BALL_SIZE) {
    ball.vx *= -1;
  }

  // Ball collision with paddles
  if (ball.intersects(player1)) {
    ball.vy = abs(ball.vy);
  }
  else if(ball.intersects(player2))
    ball.vy = -abs(ball.vy);

  // Ball out of bounds
  if (ball.y < 0 || ball.y >= SCREEN_WIDTH) {
    // Reset the ball position to the center
    ball.x = SCREEN_HEIGHT / 2;
    ball.y = SCREEN_WIDTH / 2;
    ball.vx = ballSpeedX;
    ball.vy = ballSpeedY;
  }
}

void pong() {
  calcDT();

  if(prevMulti != multiplayer) {
    player2.sprite = (multiplayer) ? pad2Ps : pad2AIs;
    prevMulti = multiplayer;
  }
  
  controls();
  movement();
  ballMovement();
  

  // Update paddle and ball positions
  player1.update();
  player2.update();
  ball.update();



  // Clear the window and render objects
  gameWindow.clear();
  player1.render(gameWindow);
  player2.render(gameWindow);
  ball.render(gameWindow);
  gameWindow.render();
  prevTime = millis();
  delay(SPF*1000);
}

void loop() {
  serialHandler();

  if(engineState) 
    pong();
}
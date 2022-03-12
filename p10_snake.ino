// Loosely based on https://www.youtube.com/watch?v=xGmXxpIj6vs&ab_channel=ChrisDeLeonofHomeTeamGameDev

#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD.h>
#include <TimerOne.h>
#include "SystemFont5x7.h"

#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

int mynumber = 0;

bool renderedIntro = false;
bool renderedGameover = false;

bool intro = true;
bool gameover = false;
bool win = false;
int points = 0;

int px = 0; // player pos horizontal
int py = 0; // player pos vertical
int tcx = 32; // tilecount horizontal
int tcy = 16; // tilecount vertical
int ax = 10; // apple pos horizontal
int ay = 10; // apple pos vertical

int xv = 1; //velocity horizontal direction
int yv = 0; //velocyit vertical direction

int appleOn = 0;
int tailLength = 3;
int tail[99][2] = {};

void ScanDMD(){ 
  dmd.scanDisplayBySPI();
}

// Restart arduino function
void(* resetFunc) (void) = 0;

void setup(void) {
  //Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  // Setup tail outside the screen as default so it doesn't blink when it's first used
  for(int i=0; i<99; i++) {
    tail[i][0]=-1;
    tail[i][1]=-1;
  }

   //Timer1 needed for dmd
   Timer1.initialize( 1000 );
   Timer1.attachInterrupt( ScanDMD );
   dmd.clearScreen( true );   //true is all pixel off, false is all pixel on
}


void renderIntro() {
  int image[16][32] = {
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,0,0,1,1,1,1,0,1,0,0,0,1,0,1,1,1,0,0,1,1,1,0,0,0,1,1,1,1},
    {1,0,0,1,1,0,1,0,0,1,0,1,0,0,0,1,1,1,0,1,1,0,1,0,1,1,0,1,1,0,0,1},
    {1,0,0,0,1,0,1,0,0,1,0,1,0,0,0,1,1,0,0,0,1,0,1,0,0,1,0,1,0,0,0,1},
    {1,0,0,0,1,0,1,0,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,0,0,1,0,1,0,0,0,1},
    {1,0,0,1,1,0,1,0,0,0,0,1,0,0,1,1,1,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1},
    {0,1,1,1,0,0,1,1,1,0,0,0,1,1,1,1,0,1,1,1,0,0,1,0,0,1,0,0,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,0,1},
    {0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
  };
  // Render image
  for (int row=0; row<16; row++) {
    for(int col=0; col<32; col++) {
      dmd.writePixel(col, row, GRAPHICS_NORMAL, image[row][col]);
    }
  }
}

void renderGameOver() {
  dmd.clearScreen( true );
  // convert points int to char* for drawSting function
  char buffer[8];
  itoa( points, buffer, 10 );

  char const* pointsString = buffer;

  int gameoverImage[13][13] = {
    {0,0,0,0,1,1,1,1,1},
    {0,0,0,1,0,0,0,0,0,1},
    {0,0,1,0,0,0,0,0,0,0,1},
    {0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,1,0,0,0,1},
    {1,0,0,0,1,0,0,0,1,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,1,1,0,0,0,0,1},
    {0,1,0,0,1,0,0,0,1,0,0,1},
    {0,0,1,0,0,0,0,0,0,0,1},
    {0,0,0,1,0,0,0,0,0,1},
    {0,0,0,0,1,1,1,1,1},
  };

  int smile[2][13]={
    {1,0,0,0,1,0,0,0,1,0,0,0,1},
    {0,1,0,0,0,1,1,1,0,0,0,1},
  };

  int posx=0;
  int posy=0;

  for (int row=1; row<14; row++) {
    for(int col=15; col<28; col++) {
      // Let's put a smile on that face
      if(win && row > 8 && row < 11) {
        dmd.writePixel(col, row, GRAPHICS_NORMAL, smile[posx-8][posy]);
      }
      else {
        dmd.writePixel(col, row, GRAPHICS_NORMAL, gameoverImage[posx][posy]);
      }
      posy=posy+1;
    }
    posx=posx+1;
    posy=0;
  }

  dmd.selectFont(SystemFont5x7);
  dmd.drawString(0,  4, pointsString, 2, GRAPHICS_NORMAL);
}

void loop(void) {
  int btnUp = digitalRead(2);
  int btnDown = digitalRead(3);
  int btnLeft = digitalRead(4);
  int btnRight = digitalRead(5);

  if(intro) {
    if(renderedIntro == false){renderIntro(); renderedIntro = true;}

    if(btnUp == LOW || btnDown == LOW || btnLeft == LOW || btnRight == LOW){
       intro = false;
    }
    delay(200);
  }
  else if(gameover) {
    //Restart after button input, but wait for render and timeout, to avoid accidental skip
    if((btnUp == LOW || btnDown == LOW || btnLeft == LOW || btnRight == LOW) && renderedGameover) {
      delay(500);
      resetFunc();
    }

    if(renderedGameover == false){renderGameOver(); renderedGameover = true; delay(1000);}
    
    delay(200);
  }
  // GAME START
  else {
    // Input reading

    if(btnUp == LOW && yv != 1) { yv=-1; xv=0;}
    else if(btnDown == LOW && yv !=-1) { yv=1; xv=0;}
    else if(btnLeft == LOW && xv !=1) { yv=0; xv=-1;}
    else if(btnRight == LOW && xv !=-1) { yv=0; xv=1;}
    
    dmd.clearScreen( true );

    // Shift tail with one
    for(int j=tailLength-1; j>0; j--){
      tail[j][0]=tail[j-1][0];
      tail[j][1]=tail[j-1][1];
    }

    tail[0][0]=px;
    tail[0][1]=py;

    // Snake movement
    px = px + xv;
    py = py + yv;

    // Self-collision
    for(int k=0; k<tailLength; k++){
      if(tail[k][0] == px && tail[k][1] == py) {
        gameover=true;
      }
    }

    // Warping at walls
    if(px<0) { px=tcx-1; }
    else if(px>tcx-1){ px=0; }
    else if(py<0){ py=tcy-1; }
    else if(py>tcy-1){ py=0; }

    // Collect apple
    if(ax == px && ay == py) {
      points = points + 1;
      if(points == 99){
        gameover = true;
        win = true;
      }
      tailLength = tailLength+1;

      newApple:
      ax=random(31);
      ay=random(15);

      // Try not to create new apple inside the snake
      for(int m=0; m<tailLength; m++) {
        if(tail[m][0] == ax && tail[m][1] == ay) {
          goto newApple; // I know... but what should I do?
        }
      }
    }
  
    // render player
    dmd.writePixel(px, py, GRAPHICS_NORMAL, 1);
    
    for(int i=0; i<tailLength; i++){
      dmd.writePixel(tail[i][0], tail[i][1], GRAPHICS_NORMAL, 1);
    }

    // Render apple
    dmd.writePixel(ax, ay, GRAPHICS_NORMAL, 1);
  }
  // GAME END

  // some delay at the end to slow things down, there should be a proper render loop, but I'm lazy
  delay(100);
}

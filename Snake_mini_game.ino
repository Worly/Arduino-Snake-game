#include <EEPROM.h>
#include <LiquidCrystal.h>

#define STARTING 0
#define UP 55
#define DOWN 66
#define LEFT 77
#define RIGHT 88

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

struct bitmap {
  byte _bitmap[2][4][8];
  void upDate(int _x, int _y) {
    lcd.createChar(byte(_x + _y * 4), _bitmap[_y][_x]);
    lcd.setCursor(_x + STARTING, _y);
    lcd.write(byte(_x + _y * 4));
  }
  void upDate() {
    for(int i = 0; i < 2; i++) 
      for(int j = 0; j < 4; j++) 
        upDate(j, i);
  }
  void pixel(int _x, int _y, bool _on, bool _update) {
   if(_on) _bitmap[_y / 8][_x / 5][_y % 8] |= 1 << 4 - _x % 5;
   else _bitmap[_y / 8][_x / 5][_y % 8] ^= 1 << 4 - _x % 5;
   if(_update) upDate(_x / 5, _y / 8);
  }
  void invert(bool _update) {
    for(int i = 0; i < 2; i++) 
      for(int j = 0; j < 4; j++)
        for(int z = 0; z < 8; z++)
          _bitmap[i][j][z] ^= B11111;

    if(_update) upDate();
  }
  void clean(bool _update) {
    for(int i = 0; i < 2; i++) 
      for(int j = 0; j < 4; j++)
        for(int z = 0; z < 8; z++)
          _bitmap[i][j][z] = B00000;

    if(_update) upDate();
  }
};

struct bitmap bm;

struct snakeBody {
  int x;
  int y;
  void render() {
    bm.pixel(x, y, true, false);
  }
};

struct snakeBody snake[321];
int dir = LEFT;
int lastdir;
int snakeSiz = 4;
int score = 0;
int foodx, foody;
int highscore;
int fz = 8;
int sp = 100;

void makeFood() {
  while(1) {
    opet:;
    foodx = random(10 - fz + 1, 9 + fz);
    foody = random(8 - map(fz, 1, 10, 1, 8) + 1, 7 + map(fz, 1, 10, 1, 8));
    for(int i = 0; i < snakeSiz; i++) {
      if(foodx == snake[i].x && foody == snake[i].y) goto opet;
    }
    break;
  }
}

void renderField() {
  for(int i = 10 - fz ; i < 9 + fz; i++) {
    bm.pixel(i, 8 - map(fz, 1, 10, 1, 8), true, false);
    bm.pixel(i, 7 + map(fz, 1, 10, 1, 8), true, false);
  }
  for(int i = 8 - map(fz, 1, 10, 1, 8); i < 8 + map(fz, 1, 10, 1, 8); i++) {
    bm.pixel(10 - fz, i, true, false);
    bm.pixel(9 + fz, i, true, false);
  }
}

void renderSetting() {
  bm.clean(false);
  for(int i = 0; i < snakeSiz; i++) 
    snake[i].render();
  renderField();
  bm.upDate();
}

void renderSettingUI(int sl) {
  if(sl == 0 || sl == 3) {
    lcd.setCursor(4, 0);
    lcd.print("Field size:");
    lcd.setCursor(4, 1);
    if(fz > 5) {
      lcd.print("  <  ");  
    }
    else lcd.print("     ");
    lcd.print(fz - 1);
    if(fz < 10) {
      lcd.print("  >  ");
    }
    else lcd.print("     ");
  }
  else if(sl == 1) {
    lcd.setCursor(4, 0);
    lcd.print("Snake speed:");
    lcd.setCursor(4, 1);
    if(sp > 10) {
      lcd.print("  <  ");  
    }
    else lcd.print("     ");
    lcd.print(sp);
    if(sp < 160) {
      lcd.print("  >  ");
    }
    else lcd.print("     ");
  }
  else if(sl == 2 || sl == -1) {
    lcd.setCursor(4, 0);
    lcd.print("Start >     ");
    lcd.setCursor(4, 1);
    lcd.print("            ");
  }
}

void setting() {
  int sl = 0;
  int upp = 0;
  int doww = 0;
  int next = 0;
  int prev = 0;
  renderSettingUI(sl);
  while(1) {
    movee();
    for(int i = 0; i < 980 - 5 * sp; i++) {
      delay(1);
      if(sl == 3) sl = 0;
      if(sl == -1) sl = 2;
      if(sl == 0) {
        if(analogRead(A3) > 500) {
          if(doww == 0 && fz > 5) {
            doww = 1;
            fz--;
            renderSetting();
            renderSettingUI(sl);
          }
        }
        else doww = 0;
        if(analogRead(A4) > 500) {
          if(upp == 0 && fz < 10) {
            upp = 1;
            fz++;
            renderSetting();
            renderSettingUI(sl);
          }
        }
        else upp = 0;
        if(analogRead(A1) > 500) {
          if(prev == 0) {
            sl--;
            prev = 1;
            renderSettingUI(sl);
          }
        }
        else prev = 0;
        if(analogRead(A2) > 500) {
          if(next == 0) {
            sl++;
            next = 1;
            renderSettingUI(sl);
          }
        }
        else next = 0;
      }
      else if(sl == 1) {
        if(analogRead(A3) > 500) {
          if(doww == 0 && sp > 10) {
            doww = 1;
            sp -= 10;
            renderSettingUI(sl);
          }
        }
        else doww = 0;
        if(analogRead(A4) > 500) {
          if(upp == 0 && sp < 160) {
            upp = 1;
            sp += 10;
            renderSettingUI(sl);
          }
        }
        else upp = 0;
        if(analogRead(A1) > 500) {
          if(prev == 0) {
            sl--;
            prev = 1;
            renderSettingUI(sl);
          }
        }
        else prev = 0;
        if(analogRead(A2) > 500) {
          if(next == 0) {
            sl++;
            next = 1;
            renderSettingUI(sl);
          }
        }
        else next = 0;
      }
      else if(sl == 2) {
        if(analogRead(A4) > 500) {
          snake[0].x = 10;
          snake[0].y = 8;
          snakeSiz = 1;
          dir = UP;
          return;
        }
        if(analogRead(A1) > 500) {
          if(prev == 0) {
            sl--;
            prev = 1;
            renderSettingUI(sl);
          }
        }
        else prev = 0;
        if(analogRead(A2) > 500) {
          if(next == 0) {
            sl++;
            next = 1;
            renderSettingUI(sl);
          }
        }
        else next = 0;
      }
      
    }
    renderSetting();
    
  }
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.setCursor(0, 0);
  randomSeed(analogRead(A0));
  snake[0].x = 10;
  snake[0].y = 8;
  setting();
  makeFood();
  highscore = EEPROM.read(0);
}

void movee() {
  for(int i = snakeSiz - 1; i >= 1; i--) {
    snake[i].x = snake[i - 1].x;
    snake[i].y = snake[i - 1].y;
  }
  if(dir == UP) {
    if(snake[0].y <= 8 - map(fz, 1, 10, 1, 8) + 1) snake[0].y = 7 + map(fz, 1, 10, 1, 8) - 1;
    else snake[0].y--;
  }
  else if(dir == DOWN) {
    if(snake[0].y >= 7 + map(fz, 1, 10, 1, 8) - 1) snake[0].y = 8 - map(fz, 1, 10, 1, 8) + 1;
    else snake[0].y++;
  }
  else if(dir == LEFT) {
    if(snake[0].x <= 10 - fz + 1) snake[0].x = 9 + fz - 1;
    else snake[0].x--;
  }
  else if(dir == RIGHT) {
    if(snake[0].x >= 10 + fz - 2) snake[0].x = 10 - fz + 1;
    else snake[0].x++;
  }
  lastdir = dir;
}

void checkKeys() {
  Serial.print(analogRead(A1));
  Serial.print(analogRead(A2));
  Serial.print(analogRead(A3));
  Serial.println(analogRead(A4));
  if(analogRead(A1) > 800 && lastdir != DOWN)
    dir = UP;
  if(analogRead(A2) > 800 && lastdir != UP)
    dir = DOWN;
  if(analogRead(A3) > 800 && lastdir != RIGHT)
    dir = LEFT;
  if(analogRead(A4) > 800 && lastdir != LEFT)
    dir = RIGHT;
}

void dead() {
  
  if(score > highscore) EEPROM.write(0, score);
  while(1) {
    bm.invert(true);
    delay(500);
  }
}

void checkDead() {
  if(score == fz * 2 * map(fz, 1, 10, 1, 8) * 2 - 1) dead();
  for(int i = 1; i < snakeSiz; i++) {
    if(snake[0].x == snake[i].x && snake[0].y == snake[i].y) dead();
  }
}

void checkEat() {
  if(snake[0].x == foodx && snake[0].y == foody) {
    snake[snakeSiz].x = snake[snakeSiz-1].x;
    snake[snakeSiz].y = snake[snakeSiz-1].y;
    snakeSiz++;
    score++;
    makeFood();
  }
}

void render() {
  bm.clean(false);
  for(int i = 0; i < snakeSiz; i++) 
    snake[i].render();
  bm.pixel(foodx, foody, true, false);
  renderField();
  bm.upDate();
  lcd.setCursor(4, 0);
  lcd.print("Score:");
  lcd.print(score);
  lcd.setCursor(4, 1);
  lcd.print("Highscr:");
  lcd.print(highscore);
}

void loop() {
  movee();
  render();
  for(int i = 0; i < 210 - sp; i++) {
    delay(1);
    checkKeys();
  }
  checkDead();
  checkEat();

}


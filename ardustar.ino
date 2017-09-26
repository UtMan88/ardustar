#include <Arduboy2.h>
#include <ArdVoice.h>
#include "bitmaps.h"
#include "voices.h"

// Structures & Definitions
#define WORLD_MAX 50
#define WORLD_MIN -50
#define MAX_OBJECTS 5
#define PLAYER_INDX 0
#define VILLAIN_INDX 1
#define BAD_ID "NONE"
#define PLAYER_ID "PLYR"
#define BULLET_TAG ".BLT"
#define ENEMY_ID "ENMY"
#define VILLAIN_ID "VILL"
#define FPS 60
#define GROWTHRATE 10
#define VILLAIN_UPDATE_DELAY 30
#define VILLAIN_SPEED_BASE 12.0
#define MAX_ENEMIES 3

enum GameState : int {
  MAIN = 0,
  GAME,
  GAMEOVER,
};

struct FPoint {
  float x; /**< The X coordinate of the point */
  float y; /**< The Y coordinate of the point */
};

class GameObject {
public:
  String id;
  FPoint position;
  FPoint forward;
  Rect bounds;
  float speed;
  unsigned long maxTimeAlive;
  unsigned long curTimeAlive;
  bool doWrap;

  void Init() {
    this->id = BAD_ID;
    this->position.x = 0;
    this->position.y = 0;
    this->forward.x = 0;
    this->forward.y = 0;
    this->bounds.x = 0;
    this->bounds.y = 0;
    this->bounds.width = 0;
    this->bounds.height = 0;
    this->speed = 0;
    this->maxTimeAlive = 0;
    this->curTimeAlive = 0;
    this->doWrap = true;
  }

  void Update(float dtimef) {
    this->position.x += this->forward.x * (this->speed * dtimef);
    this->position.y += this->forward.y * (this->speed * dtimef);
    this->bounds.x = this->position.x - (this->bounds.width / 2);
    this->bounds.y = this->position.y - (this->bounds.height / 2);
    if(doWrap) {
      if(this->position.x < -10.0f) {
        this->position.x = WIDTH;
      }
      else if(this->position.x > WIDTH + 10) {
        this->position.x = 0;
      } 
      else if(this->position.y < -10.0f) {
        this->position.y = HEIGHT;
      }
      else if(this->position.y > HEIGHT + 10) {
        this->position.y = 0;
      }
    }
  }
};

// Globals
Arduboy2 arduboy;
ArdVoice ardvoice;
GameState state = MAIN;
unsigned long previousTime = 0;
unsigned long g_timer = 0;
uint8_t g_enemyCount = 0;
uint8_t g_count = 0;
boolean g_toggle = false;
float g_villSize = 0.0f;
int g_score = 0;

// Bitmap
#define ARDBITMAP_SBUF arduboy.getBuffer()
#include <ArdBitmap.h>
ArdBitmap<WIDTH, HEIGHT> ardbitmap;

// Object Management
GameObject g_objects[MAX_OBJECTS];
GameObject g_bullets[MAX_OBJECTS];

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FPS);
  arduboy.initRandomSeed();
  g_enemyCount = 0;
  
}

void loop() {
  // put your main code here, to run repeatedly:
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;
  arduboy.clear();

  unsigned long curTime = millis();
  unsigned long deltaTime = curTime - previousTime;
  previousTime = curTime;
  
  switch(state) {
    default:
    case MAIN:
      if(titleScreen(deltaTime) == true) {
        state = GAME;
        // Init Game here.
        resetGameState();
      }
      break;
    case GAME:
      if(GameLogic(deltaTime) == true) {
        g_timer = 0;
        g_count = 0;
        g_toggle = true;
        state = GAMEOVER; 
        ardvoice.playVoice(YELL_SOUND);
      }
      break;
    case GAMEOVER:
      if(GameOverScreen(deltaTime) == true) {
        state = MAIN;
        ardvoice.stopVoice();
      }
      break;
  }
  arduboy.display();
}

void resetGameState() {
  g_timer = 0;
  g_enemyCount = 0;
  g_score = 0;
  for(int i = 0; i < MAX_OBJECTS; ++i) {
    // Init GameObjects
    g_objects[i].Init();
    g_bullets[i].Init();
  }
  g_objects[PLAYER_INDX].id = PLAYER_ID;
  g_objects[PLAYER_INDX].position.x = 50;
  g_objects[PLAYER_INDX].position.y = 0;
  g_objects[PLAYER_INDX].forward.x = 0.0f;
  g_objects[PLAYER_INDX].forward.y = 1.0f;
  g_objects[PLAYER_INDX].bounds.width = 8;
  g_objects[PLAYER_INDX].bounds.height = 8;
  g_objects[PLAYER_INDX].speed = 10.0f;

  g_objects[VILLAIN_INDX].id = VILLAIN_ID;
  g_objects[VILLAIN_INDX].position.x = WIDTH / 2;
  g_objects[VILLAIN_INDX].position.y = HEIGHT / 2;
  g_objects[VILLAIN_INDX].forward.x = 0.0f;
  g_objects[VILLAIN_INDX].forward.y = 1.0f;
  g_objects[VILLAIN_INDX].bounds.width = 16;
  g_objects[VILLAIN_INDX].bounds.height = 16;
  g_objects[VILLAIN_INDX].speed = VILLAIN_SPEED_BASE;
  g_objects[VILLAIN_INDX].doWrap = false;
  g_villSize = 0.0f;
}

boolean titleScreen(unsigned long dtime) {
  arduboy.setCursor(16,22);
  arduboy.setTextSize(2);
  arduboy.print("ArduSTAR");
  g_timer += dtime;

  if(g_timer >= 750) {
    g_timer = 0;
    g_toggle = !g_toggle;
  }
  if(g_toggle) {
    arduboy.setCursor(31, 53);
    arduboy.setTextSize(1);
    arduboy.print("PRESS FIRE!");
  }
  arduboy.pollButtons();
  if (arduboy.justPressed(A_BUTTON)) {
    return true;
  }
  return false;
}

boolean GameOverScreen(unsigned long dtime) {
  g_timer += dtime;
  if(g_count < 20) {
    if(g_timer >= 250) {
      g_timer = 0;
      g_toggle = !g_toggle;
    }
    if(g_toggle) {
      arduboy.fillScreen();
      ++g_count;
    }
  }
  else {
    arduboy.setCursor(16,22);
    arduboy.setTextSize(2);
    arduboy.print("GameOVER");
    if(g_timer >= 750) {
      g_timer = 0;
      g_toggle = !g_toggle;
    }
    if(g_toggle) {
      arduboy.setCursor(31, 53);
      arduboy.setTextSize(1);
      arduboy.print("PRESS FIRE!");
    }
    else {
      arduboy.setCursor(31, 53);
      arduboy.setTextSize(1);
      arduboy.print("SCORE: " + String(g_score));
    }
    arduboy.pollButtons();
    if (arduboy.justPressed(A_BUTTON)) {
      return true;
    }
  }
  return false;
}

boolean GameLogic(unsigned long dtime) {
  float dtimef = ((float)dtime / 1000.0f);
  g_timer += dtime;

  if(g_timer >= 750) {
    g_timer = 0;
    g_toggle = !g_toggle;
  }
  
  // Input
  arduboy.pollButtons();
  /*if (arduboy.everyXFrames(5)) {
    if(arduboy.pressed(UP_BUTTON)) {
      //g_objects[PLAYER_INDX].forward.x = 0;
      //g_objects[PLAYER_INDX].forward.y = -1;
      FPoint dir;
      dir.x = 0;
      dir.y = -1;
      FPoint steering = turnDirection(dir, g_objects[PLAYER_INDX].forward);
      g_objects[PLAYER_INDX].forward = steering;
    }
    else if(arduboy.pressed(DOWN_BUTTON)) {
      //g_objects[PLAYER_INDX].forward.x = 0;
      //g_objects[PLAYER_INDX].forward.y = 1;
      FPoint dir;
      dir.x = 0;
      dir.y = 1;
      FPoint steering = turnDirection(dir, g_objects[PLAYER_INDX].forward);
      g_objects[PLAYER_INDX].forward = steering;
    }
    else if(arduboy.pressed(RIGHT_BUTTON)) {
      //g_objects[PLAYER_INDX].forward.x = 1;
      //g_objects[PLAYER_INDX].forward.y = 0;
      FPoint dir;
      dir.x = 1;
      dir.y = 0;
      FPoint steering = turnDirection(dir, g_objects[PLAYER_INDX].forward);
      g_objects[PLAYER_INDX].forward = steering;
    }
    else if(arduboy.pressed(LEFT_BUTTON)) {
      //g_objects[PLAYER_INDX].forward.x = -1;
      //g_objects[PLAYER_INDX].forward.y = 0;
      FPoint dir;
      dir.x = -1;
      dir.y = 0;
      FPoint steering = turnDirection(dir, g_objects[PLAYER_INDX].forward);
      g_objects[PLAYER_INDX].forward = steering;
    }
  }*/
  if(arduboy.pressed(UP_BUTTON)) {
    g_objects[PLAYER_INDX].forward.x = 0;
    g_objects[PLAYER_INDX].forward.y = -1;
  }
  else if(arduboy.pressed(DOWN_BUTTON)) {
    g_objects[PLAYER_INDX].forward.x = 0;
    g_objects[PLAYER_INDX].forward.y = 1;
  }
  else if(arduboy.pressed(RIGHT_BUTTON)) {
    g_objects[PLAYER_INDX].forward.x = 1;
    g_objects[PLAYER_INDX].forward.y = 0;
  }
  else if(arduboy.pressed(LEFT_BUTTON)) {
    g_objects[PLAYER_INDX].forward.x = -1;
    g_objects[PLAYER_INDX].forward.y = 0;
  }
  if (arduboy.justPressed(A_BUTTON) && !DoesBulletExist(PLAYER_ID)) {
    spawnBullet(PLAYER_ID, g_objects[PLAYER_INDX].position, g_objects[PLAYER_INDX].forward);
  }

  //AI Update
  // Villain AI
  if(g_villSize >= 1.0f && arduboy.everyXFrames(VILLAIN_UPDATE_DELAY)) {
    FPoint steering = steer(g_objects[PLAYER_INDX].position, g_objects[VILLAIN_INDX].position, g_objects[VILLAIN_INDX].forward);
    g_objects[VILLAIN_INDX].forward = steering;
  }
  if(arduboy.everyXFrames(FPS)) {
    spawnEnemies();
  }
  // Update
  if(arduboy.everyXFrames(FPS)) {
    ++g_score;
  }
  for(int i = 0; i < MAX_OBJECTS; ++i) {
    if(g_objects[i].id == BAD_ID)
      continue;
    if(i == VILLAIN_INDX) {
      if(g_villSize >= 1.0f) {
        g_villSize = 1.0f;
        g_objects[i].speed += 0.05 * dtimef;
        g_objects[i].Update(dtimef);
      }
      else {
        g_objects[i].speed = 0.0f;
        g_villSize += dtimef / GROWTHRATE;
        if(g_villSize >= 1.0f) {
          ardvoice.playVoice(BEWARE_SOUND);
          g_objects[i].speed = VILLAIN_SPEED_BASE;
        }
      }
    }
    else if(g_objects[i].id == ENEMY_ID) {
      if(arduboy.everyXFrames(VILLAIN_UPDATE_DELAY)) {
        FPoint steering = steer(g_objects[PLAYER_INDX].position, g_objects[i].position, g_objects[i].forward);
        g_objects[i].forward = steering;
      }
      g_objects[i].Update(dtimef);
    }
    else {
      g_objects[i].Update(dtimef);
    }
  }
  for(int i = 0; i < MAX_OBJECTS; ++i) {
    if(g_bullets[i].id == BAD_ID)
      continue;
    g_bullets[i].Update(dtimef);
    //TODO check collisions
    g_bullets[i].curTimeAlive += dtime;
    if(g_bullets[i].curTimeAlive >= g_bullets[i].maxTimeAlive) {
      g_bullets[i].Init();
      continue;
    }
    BulletCollisionCheck(g_bullets[i]);
  }
  if(EnemyCollisionCheck()) {
    return true;
  }
  // Draw
  arduboy.setCursor(0, HEIGHT - 8);
  arduboy.setTextSize(1);
  arduboy.print(String(g_score));
  //arduboy.print(String(g_objects[PLAYER_INDX].position.x) + "," + g_objects[PLAYER_INDX].position.y);
  //arduboy.print(String(g_objects[PLAYER_INDX].forward.x) + "," + g_objects[PLAYER_INDX].forward.y);
  //arduboy.print(String(g_objects[PLAYER_INDX].speed));
  drawPlayer();
  drawEnemies();
  drawBullets();
  drawVillain();
  return false;
}

void drawPlayer() {
  if(g_objects[PLAYER_INDX].forward.x == 0) {
    if(g_objects[PLAYER_INDX].forward.y == -1) {
      // UP
      ardbitmap.drawCompressed(g_objects[PLAYER_INDX].position.x, g_objects[PLAYER_INDX].position.y, PLYR, WHITE, ALIGN_CENTER, MIRROR_VERTICAL);
    }
    else {
      // DOWN
      ardbitmap.drawCompressed(g_objects[PLAYER_INDX].position.x, g_objects[PLAYER_INDX].position.y, PLYR, WHITE, ALIGN_CENTER, MIRROR_NONE);
    }
  }
    else {
  if(g_objects[PLAYER_INDX].forward.x == -1) {
    // LEFT
    ardbitmap.drawCompressed(g_objects[PLAYER_INDX].position.x, g_objects[PLAYER_INDX].position.y, PLYR_L, WHITE, ALIGN_CENTER, MIRROR_NONE);
    }
  else {
    // RIGHT
    ardbitmap.drawCompressed(g_objects[PLAYER_INDX].position.x, g_objects[PLAYER_INDX].position.y, PLYR_L, WHITE, ALIGN_CENTER, MIRROR_HORIZONTAL);
    }
  }
}

void drawBullets() {
  if(arduboy.everyXFrames(10))
    return;
  for(int i = 0; i < MAX_OBJECTS; ++i)
  {
    if(g_bullets[i].id == BAD_ID) 
      continue;
    arduboy.drawPixel(g_bullets[i].position.x, g_bullets[i].position.y);
  }
}

void drawEnemies() {
  for(int i = 0; i < MAX_OBJECTS; ++i)
  {
    if(i == PLAYER_INDX || i == VILLAIN_INDX || g_objects[i].id == BAD_ID) 
      continue;
    if(g_objects[i].forward.x == 0) {
      if(g_objects[i].forward.y == -1) {
        // UP
        ardbitmap.drawCompressed(g_objects[i].position.x, g_objects[i].position.y, ENMY, WHITE, ALIGN_CENTER, MIRROR_VERTICAL);
      }
      else {
        // DOWN
        ardbitmap.drawCompressed(g_objects[i].position.x, g_objects[i].position.y, ENMY, WHITE, ALIGN_CENTER, MIRROR_NONE);
      }
    }
    else {
      if(g_objects[i].forward.x == -1) {
      // LEFT
      ardbitmap.drawCompressed(g_objects[i].position.x, g_objects[i].position.y, ENMY, WHITE, ALIGN_CENTER, MIRROR_NONE);
    }
    else {
      // RIGHT
      ardbitmap.drawCompressed(g_objects[i].position.x, g_objects[i].position.y, ENMY, WHITE, ALIGN_CENTER, MIRROR_HORIZONTAL);
      }
    }
  }
}

void drawVillain() {
  ardbitmap.drawCompressedResized(g_objects[VILLAIN_INDX].position.x, g_objects[VILLAIN_INDX].position.y, VILL, WHITE, ALIGN_CENTER, MIRROR_NONE, g_villSize);
}

FPoint steer(const FPoint& target, const FPoint& pos, const FPoint& curForward) {
  FPoint dir;
  dir.x = target.x - pos.x;
  dir.y = target.y - pos.y;
  float mag;
  mag = sqrt(sq(dir.x) + sq(dir.y));
  dir.x = dir.x/mag;
  dir.y = dir.y/mag;

  return turnDirection(dir, curForward);
}

FPoint turnDirection(const FPoint& dir, const FPoint& curForward) {
  float mag;
  FPoint fwdSum; 
  fwdSum.x = dir.x + curForward.x;
  fwdSum.y = dir.y + curForward.y;
  mag = sqrt(sq(fwdSum.x) + sq(fwdSum.y));
  FPoint steerVel;
  steerVel.x = fwdSum.x/mag;
  steerVel.y = fwdSum.y/mag;
  return steerVel;
}

void spawnBullet(String id, const FPoint& position, const FPoint& forward) {
  int newBullet = -1;
  for(int i = 0; i < MAX_OBJECTS; ++i)
  {
    if(g_bullets[i].id == BAD_ID) {
      newBullet = i;
      break;
    }
  }

  if(newBullet == -1)
    return;
  
  g_bullets[newBullet].id = id + BULLET_TAG;
  g_bullets[newBullet].position.x = position.x;
  g_bullets[newBullet].position.y = position.y;
  g_bullets[newBullet].forward.x = forward.x;
  g_bullets[newBullet].forward.y = forward.y;
  g_bullets[newBullet].speed = 40.0f;
  g_bullets[newBullet].curTimeAlive = 0;
  g_bullets[newBullet].maxTimeAlive = 1250;
  playTone(600, 150);
}

void spawnEnemies() {
  if(g_enemyCount >= MAX_ENEMIES)
    return;
  int newEnemy = -1;
  for(int i = 0; i < MAX_OBJECTS; ++i)
  {
    if(g_objects[i].id == BAD_ID) {
      newEnemy = i;
      break;
    }
  }
  if(newEnemy == -1)
    return;
  ++g_enemyCount;
  g_objects[newEnemy].id = ENEMY_ID;
  if(g_toggle) {
    g_objects[newEnemy].position.x = WIDTH;
    g_objects[newEnemy].position.y = random(0, HEIGHT);
  }
  else {
    g_objects[newEnemy].position.x = random(0, WIDTH);
    g_objects[newEnemy].position.y = HEIGHT;
  }
  g_objects[newEnemy].forward.x = g_objects[PLAYER_INDX].position.x - g_objects[newEnemy].position.x;
  g_objects[newEnemy].forward.y = g_objects[PLAYER_INDX].position.x - g_objects[newEnemy].position.y;
  g_objects[newEnemy].speed = 4.0f;
  g_objects[newEnemy].bounds.width = 8;
  g_objects[newEnemy].bounds.height = 8;
  g_objects[newEnemy].doWrap = false;
}

bool DoesBulletExist(String id) {
  const String bid = id + BULLET_TAG;
  for(int i = 0; i < MAX_OBJECTS; ++i)
  {
    if(g_bullets[i].id.startsWith(bid)) {
      return true;
    }
  }
  return false;
}

bool BulletCollisionCheck(const GameObject& bullet) {
  bool isPlayerBullet = bullet.id.startsWith(PLAYER_ID);
  Point pt;
  pt.x = bullet.position.x;
  pt.y = bullet.position.y;
  if(isPlayerBullet) {
    for(int i = 0; i < MAX_OBJECTS; ++i) {
      if(g_objects[i].id == ENEMY_ID && arduboy.collide(pt, g_objects[i].bounds)) {
        g_objects[i].Init();
        bullet.Init();
        --g_enemyCount;
        g_villSize = constrain(g_villSize - 0.1f, 0.0f, 1.0f);
        playTone(523, 250);
        g_score += 10;
        break;
      }
      else if(g_villSize >= 1.0f && 
              g_objects[i].id == VILLAIN_ID &&
              arduboy.collide(pt, g_objects[i].bounds)) {
        g_villSize = constrain(g_villSize - 0.25f, 0.0f, 1.0f);
        bullet.Init();
        playTone(200, 250);
        g_score += 20;
        break;
      }
    }
  }
  else {
    //for(int i = 0; i < MAX_OBJECTS; ++i) {
      // TODO
    //}
  }
}

bool EnemyCollisionCheck() {
  for(int i = 0; i < MAX_OBJECTS; ++i) {
    if(i == VILLAIN_INDX && 
       g_villSize >= 1.0f && 
       arduboy.collide(g_objects[PLAYER_INDX].bounds, g_objects[i].bounds)) {
      return true;
    }
    if(g_objects[i].id == ENEMY_ID && arduboy.collide(g_objects[PLAYER_INDX].bounds, g_objects[i].bounds)) {
      // Game Over!
      return true;
    }
  }
  return false;
}

// Wrap the Arduino tone() function so that the pin doesn't have to be
// specified each time. Also, don't play if audio is set to off.
void playTone(unsigned int frequency, unsigned long duration)
{
  //if (arduboy.audio.enabled() == true)
  {
    tone(PIN_SPEAKER_1, frequency, duration);
  }
}

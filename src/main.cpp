#include "raylib.h"
#include <cstdlib>
#include <ctime>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float FPS = 144.0f;
const float PARTICLE_SPEED = 60.0f;

const int MAX_FIREWORKS = 20;
const int MAX_SPARKS = 500;

const float CHANCE_OF_EXPLOSION = 0.05f;
const int HEIGHT_THRESHOLD = 6 * SCREEN_HEIGHT / 10;

struct Spark {
  float vx;
  float vy;
  float weight;
  int red;
  int green;
  int blue;
};

struct Firework {
  float x;
  float y;
  float age;
  bool explode;
  std::vector<Spark> sparks;
};

std::vector<Firework> fireworks;

float RandFloat(float min, float max) {
  return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

void ResetFirework(Firework &fw) {
  fw.x = RandFloat(0, SCREEN_WIDTH);
  fw.y = SCREEN_HEIGHT;
  fw.age = 0;
  fw.explode = false;
}

void InitFireworks(std::vector<Firework> &fws) {
  for (int i = 0; i < MAX_FIREWORKS; i++) {
    Firework fw;
    fw.sparks.clear();

    for (int n = 0; n < MAX_SPARKS; n++) {
      Spark s;
      s.vx = RandFloat(0.5f, 5.5f);
      s.vy = RandFloat(0.5f, 5.5f);
      s.weight = RandFloat(0.03f, 0.33f);
      s.red = rand() % 2;
      s.green = rand() % 2;
      s.blue = rand() % 2;

      if (rand() % 2)
        s.vx = -s.vx;
      if (rand() % 2)
        s.vy = -s.vy;

      fw.sparks.push_back(s);
    }

    ResetFirework(fw);
    fws.push_back(fw);
  }
}

void UpdateDrawFireworks(std::vector<Firework> &fws) {
  float deltaTime = GetFrameTime();

  for (size_t i = 0; i < fws.size(); i++) {
    Firework &fw = fws[i];

    if (fw.explode) {
      for (auto &s : fw.sparks) {
        for (int t = 0; t < 10; t++) {
          float trailAge = fw.age + (float)t;

          float x = fw.x + s.vx * trailAge;
          float y = fw.y + s.vy * trailAge +
                    s.weight * trailAge * s.weight * trailAge;

          int fade = t * 20 - (int)(fw.age * 2);
          if (fade < 0)
            fade = 0;

          Color c = {(unsigned char)(s.red * fade),
                     (unsigned char)(s.green * fade),
                     (unsigned char)(s.blue * fade), 255};

          DrawRectangle((int)x, (int)y, 4, 4, c);
        }
      }

      fw.age += PARTICLE_SPEED * deltaTime;

      if (fw.age > 100 && RandFloat(0, 1) < 0.05f) {
        ResetFirework(fw);
      }
    } else {
      fw.y -= 600.0f * deltaTime;

      for (int s = 0; s < 15; s++) {
        Color c = {(unsigned char)(i * 50), (unsigned char)(s * 17), 0, 255};

        float x = fw.x + RandFloat(-s / 2.0f, s / 2.0f);
        float y = fw.y + s * 4;

        DrawRectangle((int)x, (int)y, 4, 4, c);
      }

      if (RandFloat(0, 1) < CHANCE_OF_EXPLOSION && fw.y < HEIGHT_THRESHOLD) {
        fw.explode = true;
      }
    }
  }
}

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Fireworks");
  SetTargetFPS(FPS);
  srand((unsigned)time(nullptr));
  InitFireworks(fireworks);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    UpdateDrawFireworks(fireworks);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
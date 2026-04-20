#include <math.h>
#include <raylib.h>
#include <stdio.h>

void draw_double_pendulum(float t1, float t2);
void show_path(Vector2 dot);

#define WIDTH 800
#define HEIGHT 600

#define L1 300
#define L2 100

#define QUEUE_LEN 100
Vector2 queue[QUEUE_LEN];
int len = 0;

int main() {
  InitWindow(WIDTH, HEIGHT, "double pendulum");
  SetTargetFPS(60);
  float t = 0;
  while (!WindowShouldClose()) {
    ClearBackground(BLACK);
    BeginDrawing();

    draw_double_pendulum(t, t * t);

    EndDrawing();

    t += 0.1;
  }
}

void draw_double_pendulum(float t1, float t2) {
  float x0, y0, x1, y1, x2, y2;
  x0 = WIDTH / 2;
  y0 = 0;

  x1 = x0 + L1 * sinf(t1);
  y1 = y0 + L1 * cosf(t1);

  x2 = x1 + L2 * sinf(t2);
  y2 = y1 + L2 * cosf(t2);
  DrawLine(x0, y0, x1, y1, RAYWHITE);
  DrawCircle(x1, y1, 10, BLUE);
  DrawLine(x1, y1, x2, y2, RAYWHITE);
  DrawCircle(x2, y2, 20, RED);

  show_path((Vector2){x2, y2});
}

void show_path(Vector2 dot) {
  if (len < QUEUE_LEN) {
    queue[len] = dot;
    len++;
  } else {
    for (int i = 0; i < len - 1; i++) {
      queue[i] = queue[i + 1];
    }
    queue[len - 1] = dot;
  }
  for (int i = 0; i < len; i++) {
    int a = 255 * i / len;
    DrawCircleV(queue[i], 2, CLITERAL(Color){230, 41, 55, a});
  }
}

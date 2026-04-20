#include <math.h>
#include <raylib.h>
#include <stdio.h>

void draw_double_pendulum(float t1, float t2);
void show_path(Vector2 dot);

#define WIDTH 800
#define HEIGHT 600

#define L1 200
#define L2 200

#define QUEUE_LEN 1000

typedef struct {
  Vector2 pose;
  float velocity;
} queue_cell;

queue_cell queue[QUEUE_LEN];

int len = 0;

float t1, t2, t1_d, t2_d, m1, m2;

#define g 10

#define dt 0.1

void step() {
  float t1_dd = (-g * (2 * m1 + m2) * sinf(t1) - m2 * g * sinf(t1 - 2 * t2) -
                 2 * sinf(t1 - t2) * m2 *
                     (t2_d * t2_d * L2 + t1_d * t1_d * L1 * cosf(t1 - t2))) /
                (L1 * (2 * m1 + m2 - m2 * cosf(2 * t1 - 2 * t2)));

  float t2_dd = (2 * sinf(t1 - t2) *
                 (t1_d * t1_d * L1 * (m1 + m2) + g * (m1 + m2) * cosf(t1) +
                  t2_d * t2_d * L2 * m2 * cosf(t1 - t2)) /
                 (L2 * (2 * m1 + m2 - m2 * cosf(2 * t1 - 2 * t2))));

  t1_d += t1_dd * dt;
  t2_d += t2_dd * dt;

  t1 += t1_d * dt;
  t2 += t2_d * dt;
}

void init_step() {
  t1_d = 0;
  t2_d = 0;
  t1 = 0;
  t2 = M_PI + 0.01;
  m1 = 100;
  m2 = 100;
}

int main() {
  InitWindow(WIDTH, HEIGHT, "double pendulum");
  SetTargetFPS(60);
  init_step();
  while (!WindowShouldClose()) {
    ClearBackground(BLACK);
    BeginDrawing();

    draw_double_pendulum(t1, t2);
    step();

    EndDrawing();
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

float getV(Vector2 cur, Vector2 pre) {
  float diffx = cur.x - pre.x;
  float diffy = cur.y - pre.y;
  float v = sqrt(diffx * diffx + diffy * diffy);
  return v;
}

float min(float i, float j) { return i < j ? i : j; }

void show_path(Vector2 dot) {
  if (len < QUEUE_LEN) {
    len++;
  } else {
    for (int i = 0; i < len - 1; i++) {
      queue[i] = queue[i + 1];
    }
  }

  queue[len - 1].pose = dot;

  float v;
  if (len > 1) {
    v = getV(dot, queue[len - 2].pose);
    printf("vel:%.2f\n", v);
  } else {
    v = 0;
  }

  queue[len - 1].velocity = v;

  for (int i = 0; i < len; i++) {
    int a = 255 * i / len;
    float radius = min(10 / queue[i].velocity, 5);
    DrawCircleV(queue[i].pose, radius, CLITERAL(Color){230, 41, 55, a});
  }
}

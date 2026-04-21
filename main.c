#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void draw_double_pendulum(float t1, float t2);
void show_path(Vector2 dot);

#define WIDTH 800
#define HEIGHT 800

#define L1 200
#define L2 200

#define BUF_LEN 1000

struct cell {
  Vector2 pose;
  float velocity;
};

struct {
  struct cell data[BUF_LEN];
  int end;   // write pose
  int start; // read pose
} ring_buf;

float t1, t2, t1_d, t2_d, m1, m2;

#define g 10

#define FPS 400

#define dt (12.0 / FPS)

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
  SetRandomSeed(time(NULL));
  t1 = M_PI / 180.0 * GetRandomValue(-180, 180);
  t2 = M_PI / 180.0 * GetRandomValue(-180, 180);
  m1 = 500;
  m2 = 500;
}

int main() {
  InitWindow(WIDTH, HEIGHT, "double pendulum");
  SetTargetFPS(FPS);
  init_step();
  while (!WindowShouldClose()) {
    ClearBackground(BLACK);
    BeginDrawing();
    DrawFPS(10, 10);

    if (IsKeyPressed(KEY_SPACE)) {
      ring_buf.start = 0;
      ring_buf.end = 0;
      init_step();
    }

    draw_double_pendulum(t1, t2);
    step();

    EndDrawing();
  }
}

void draw_double_pendulum(float t1, float t2) {
  float x0, y0, x1, y1, x2, y2;
  x0 = (float)WIDTH / 2;
  y0 = (float)HEIGHT / 2;

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

float clamp(float i, float min, float max) {
  if (i < min) {
    return min;
  }
  if (i > max) {
    return max;
  }
  return i;
}

int getLen(void) {
  if (ring_buf.start <= ring_buf.end) {
    return ring_buf.end - ring_buf.start;
  } else {
    return BUF_LEN - ring_buf.start + ring_buf.end;
  }
}

struct cell get(int n) {
  if (n >= getLen()) {
    return (struct cell){};
  }
  int idx = ring_buf.start + n;
  if (idx >= BUF_LEN) {
    idx -= BUF_LEN;
  }
  return ring_buf.data[idx];
}

void append(struct cell cell) {
  int idx = ring_buf.end;
  ring_buf.data[idx] = cell;
  ring_buf.end++;
  if (ring_buf.end >= BUF_LEN) {
    ring_buf.end = 0;
  }

  if (ring_buf.end == ring_buf.start) {
    ring_buf.start++;
    if (ring_buf.start >= BUF_LEN) {
      ring_buf.start = 0;
    }
  }
}

void pop(void) {
  ring_buf.start++;
  if (ring_buf.start >= BUF_LEN) {
    ring_buf.start = 0;
  }
}

void show_path(Vector2 dot) {
  // TODO: append
  struct cell cell;
  cell.pose = dot;

  float v;
  if (getLen() > 1) {
    // TODO: get last 2
    v = getV(dot, get(getLen() - 1).pose);
    // printf("vel:%.2f\n", v);
  } else {
    v = 0;
  }

  // TODO: append velocity
  cell.velocity = v;
  append(cell);

  for (int i = 0; i < getLen(); i++) {
    struct cell pose = get(i);
    int a = 255 * i / getLen();
    // TODO: for each
    float radius = clamp(3.3 / pose.velocity, 1.5, 3);
    DrawCircleV(pose.pose, radius, CLITERAL(Color){230, 41, 55, a});
  }
}

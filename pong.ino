#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

// OLED information
#define OLED_RESET 4
#define I2C_ADDR 0x3C
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SH1106 display(OLED_RESET);

// Paddle pins
#define P1_DOWN 2
#define P1_UP 3
#define P2_DOWN 4
#define P2_UP 5

// Other constants
#define PADDLE_HEIGHT 8

struct Paddle
{
    int x;
    int y;
    int downPin;
    int upPin;
};

struct Ball
{
    int x;
    int y;
    int dx; // direction
    int dy;
};

void setup()
{
    init_display();
    init_pins();
    init_game();
}

void game(Paddle *paddle1, Paddle *paddle2, Ball *ball)
{
    ball->x += ball->dx;
    ball->y += ball->dy;

    move_paddle(paddle1);
    move_paddle(paddle2);

    if (is_colliding(paddle1, ball) || is_colliding(paddle2, ball))
    {
        ball->dx *= -1;
        return game(paddle1, paddle2, ball);
    }

    if (ball->y == 0 || ball->y == SCREEN_HEIGHT - 1)
    {
        ball->dy *= -1;
    }

    if (ball->x == 0 || ball->x == SCREEN_WIDTH - 1)
    {
        ball->x = SCREEN_WIDTH / 2;
        ball->y = SCREEN_HEIGHT / 2;
        paddle1->y = SCREEN_HEIGHT / 2;
        paddle2->y = SCREEN_HEIGHT / 2;
    }

    display.clearDisplay();
    draw_paddle(paddle1->x, paddle1->y);
    draw_paddle(paddle2->x, paddle2->y);
    draw_ball(ball->x, ball->y);
    display.display();

    game(paddle1, paddle2, ball);
}

boolean is_colliding(Paddle *paddle, Ball *ball)
{
    return (ball->x == paddle->x && ball->y >= paddle->y && ball->y <= paddle->y + PADDLE_HEIGHT);
}

void draw_paddle(int x, int y)
{
    display.drawFastVLine(x, y, PADDLE_HEIGHT, WHITE);
}

void draw_ball(int x, int y)
{
    display.drawPixel(x, y, WHITE);
}

void move_paddle(Paddle *paddle)
{
    boolean down = digitalRead(paddle->downPin);
    boolean up = digitalRead(paddle->upPin);

    if (down == LOW)
    {
        paddle->y--;
    }
    else if (up == LOW)
    {
        paddle->y++;
    }
}

void init_pin(int pin)
{
    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH);
}

void init_pins()
{
    init_pin(P1_DOWN);
    init_pin(P1_UP);
    init_pin(P2_DOWN);
    init_pin(P2_UP);
}

void init_display()
{
    display.begin(SH1106_SWITCHCAPVCC, I2C_ADDR);
}

void init_game()
{
    Paddle paddle1 = {0, SCREEN_HEIGHT / 2, P1_UP, P1_DOWN};
    Paddle paddle2 = {SCREEN_WIDTH - 1, SCREEN_HEIGHT / 2, P2_UP, P2_DOWN};
    Ball ball = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 1, 1};
    game(&paddle1, &paddle2, &ball);
}

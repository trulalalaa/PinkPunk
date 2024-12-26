#include <raylib.h>
#include <iostream>

bool pause = false;
bool isMusicPlaying = true;
// Texture
Texture2D Background;
Texture2D menu;
Texture2D ThemeBG;
Texture2D themes[3];  
Texture2D poweruptexture;
Texture2D Mute;
// Suara 
Sound paddlehitsound;
Sound poweruphitsound;
Music ThemeSong;
Sound score;
Sound theme; 
// Gkepake :v
Color Green = Color{0, 0, 204, 255};
Color Dark_Green = Color{0, 0, 255, 255};
Color Light_Green = Color{255, 20, 147, 0};
Color Yellow = Color{10, 10, 10, 255};
Color Pause = Color{255, 255, 255, 130};
// Nilai awal 
int player_score = 0;
int cpu_score = 0;
int difficulty = 1;
int currentTheme = 0;
// BIar suara ga double 
float lastSoundTime = 0.0f;
const float soundCooldown = 0.5f; 

enum LastTouch {
    NONE,
    PLAYER,
    CPU
}; LastTouch lastTouchBall = NONE;

enum PowerUpType {
    PADDLE_SIZE,
    SPEED_CHANGE,
    BALL_SPEED
} currentPowerUp;

class Ball {
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw() {
        DrawCircle(x, y, radius, WHITE);
    }
    void Update() {
        x += speed_x;
        y += speed_y;
        if (y + radius >= GetScreenHeight() or y - radius <= 0) {
            speed_y *= -1;
        }
        if (x + radius >= GetScreenWidth()) {
            cpu_score++;
            ResetBall();
        }
        if (x - radius <= 0) {
            player_score++;
            ResetBall();
        }
    }
    void ResetBall() {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];
    }
};
class Paddle {
protected:
    void LimitMovement() {
        if (y <= 0) {
            y = 0;
        }
        if (y + height >= GetScreenHeight()) {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed;

    void Draw() {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }

    void Update() {
        if (IsKeyDown(KEY_UP)) {
            y = y - speed;
        }
        if (IsKeyDown(KEY_DOWN)) {
            y = y + speed;
        }
        LimitMovement();
    }
};

class CpuPaddle : public Paddle {
public:
    void Update(int ball_y) {
        if (y + height / 2 > ball_y) {
            y = y - speed;
        }
        if (y + height / 2 <= ball_y) {
            y = y + speed;
        }
        LimitMovement();
    }
};

class PowerUp {
public:
    float x, y;
    float width, height;
    Color color;
    bool active;

    PowerUp() {
        width = 55;
        height = 55;
        color = WHITE;
        active = false;
    }

    void Draw() {
        if (active) {
            DrawTexture(poweruptexture, x, y, WHITE); 
        }
    }

    void Spawn() {
        x = GetRandomValue(100, GetScreenWidth() - 100);
        y = GetRandomValue(100, GetScreenHeight() - 100);
        active = true;
    }

    void Reset() {
        active = false;
    }
};

Ball ball;
Paddle player;
CpuPaddle cpu;
PowerUp powerUp;
// Show Window awal 
void ShowIntroMenu() {
    float blinkTimer = 0.0f;
    const float blinkInterval = 1.0f; 

    while (true) {
        blinkTimer += GetFrameTime();
        UpdateMusicStream(ThemeSong);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(menu, 0, 0, WHITE);

        int textWidth = MeasureText("PINK PUNK", 100);
        int centerX = GetScreenWidth() / 2 - textWidth / 2;
        int centerY = GetScreenHeight() / 2 - 240;

        // Efek bayangan 
        DrawText("PINK PUNK", centerX - 2, centerY, 102, BLACK);
        DrawText("PINK PUNK", centerX + 2, centerY, 102, BLACK);
        DrawText("PINK PUNK", centerX, centerY - 2, 102, BLACK);
        DrawText("PINK PUNK", centerX, centerY + 2, 102, BLACK);

        // Judul 
        DrawText("PINK PUNK", centerX, centerY, 100, WHITE);

        DrawText("Bagas Aryo Dananjoyo (5024241031)", 10, GetScreenHeight() - 25, 15, WHITE);

        // Kedap kedip
        if (blinkTimer >= blinkInterval) {
            blinkTimer = 0.0f;
        }

        if (blinkTimer < blinkInterval / 2.0f) {
            int textWidth2 = MeasureText("Press [Enter] to Start Game", 40);
            DrawText("Press [Enter] to Start Game", GetScreenWidth() / 2 - textWidth2 / 2, GetScreenHeight() - 200, 40, WHITE);
        }
        // Lanjutt
        if (IsKeyPressed(KEY_ENTER) or IsKeyPressed(KEY_SPACE)) {
            break;
        }
        EndDrawing();
    }
}
// Menu pemilihan difficulty 
void ShowMenu() {
    while (true) {
        BeginDrawing();
        ClearBackground(BLACK);
        UpdateMusicStream(ThemeSong);
        DrawTexture(Background, 0, 0, WHITE);

        DrawText("Select Difficulty", GetScreenWidth() / 2 - 180, GetScreenHeight() / 4, 40, WHITE);
        DrawText("1. Easy", GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 - 60, 30, WHITE);
        DrawText("2. Medium", GetScreenWidth() / 2 - 100, GetScreenHeight() / 2, 30, WHITE);
        DrawText("3. Hard", GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 + 60, 30, WHITE);
        // Ez
        if (IsKeyPressed(KEY_ONE)) {
            difficulty = 1;
            break;
        }
        // B aja
        if (IsKeyPressed(KEY_TWO)) {
            difficulty = 2;
            break;
        }
        // Mayan lah 
        if (IsKeyPressed(KEY_THREE)) {
            difficulty = 3;
            break;
        }
        EndDrawing();
    }
}
// Pemilihan tema lapangan 
void ShowTheme(){
    while (true) {
        BeginDrawing();
        ClearBackground(BLACK);
        UpdateMusicStream(ThemeSong);
        DrawTexture(ThemeBG, 0, 0, WHITE);

        DrawText("Select Theme", GetScreenWidth() / 2 - 150, GetScreenHeight() / 4, 40, WHITE);
        DrawText("A) Sunsetz", GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 - 60, 30, WHITE);
        DrawText("B) Galaxy", GetScreenWidth() / 2 - 100, GetScreenHeight() / 2, 30, WHITE);
        DrawText("C) City Night", GetScreenWidth() / 2 - 100, GetScreenHeight() / 2 + 60, 30, WHITE);

        if (IsKeyPressed(KEY_A)) {
            currentTheme = 0;
            break;
        }
        if (IsKeyPressed(KEY_B)) {
            currentTheme = 1;
            break;
        }
        if (IsKeyPressed(KEY_C)) {
            currentTheme = 2;
            break;
        }
        EndDrawing();
    }
}
// Pencet M buat mute yah.. (Berlaku hanya di Ingame)
void ControlMusic() {
    if (IsKeyPressed(KEY_M)) {
        isMusicPlaying = !isMusicPlaying;
        if (isMusicPlaying)
            ResumeMusicStream(ThemeSong);
        else
            PauseMusicStream(ThemeSong);
    }
    if(!isMusicPlaying){
    DrawTexture(Mute, GetScreenWidth() - 40, GetScreenHeight() - 40,  WHITE);
    }
}
int main() {
    using namespace std;
    cout << "Starting the game" << endl;
    const int screen_width = 1280;
    const int screen_height = 800;
    InitAudioDevice();
    InitWindow(screen_width, screen_height, "Pink Punk : Kanaeru");
    // BG Awal ama diff 
    Background = LoadTexture("Background/menuuuu.png");
    menu = LoadTexture("Background/menuuu.png");
    ThemeBG = LoadTexture("Background/menuuuu.png");
    // BG buat ingame 
    themes[0] = LoadTexture("Background/sunsetz.png");  
    themes[1] = LoadTexture("Background/galaxy.png");  
    themes[2] = LoadTexture("Background/city.png");  
    // Buat Pixel 
    poweruptexture = LoadTexture("pixel/powerupkwori.png");
    Mute = LoadTexture("pixel/mute.png");
    // Buat suara 
    paddlehitsound = LoadSound("Sound/bola.wav");
    poweruphitsound = LoadSound("Sound/PowerUp.wav"); 
    ThemeSong = LoadMusicStream("Sound/ThemeSong.mp3");
    // Buat volum
    SetSoundVolume(paddlehitsound, 7.0f);
    SetSoundVolume(poweruphitsound, 5.0f); 
    SetMusicVolume(ThemeSong, 0.3f);
    PlayMusicStream(ThemeSong);
    SetTargetFPS(80);
    //Ngeshow  
    ShowIntroMenu(); 
    ShowMenu();
    ShowTheme(); 
    // Detect difficulty 
    int ball_speed = (difficulty == 1) ? 5 : (difficulty == 2) ? 7 : 12;
    int cpu_speed = (difficulty == 1) ? 4 : (difficulty == 2) ? 6 : 9;
    // Data bola
    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = ball_speed;
    ball.speed_y = ball_speed;
    // Data Paddle player (Kanan) 
    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;
    // Data Paddle CPU (Kiri)
    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = cpu_speed;
    // Buat itung PowerUp
    float powerUpTimer = 0.0f;
    const float powerUpSpawnTime = 15.0f;
    float powerUpEffectDuration = 6.0f;
    float effectTimer = 0.0f;
    bool powerUpActive = false;
    // Mulai 
    while (!WindowShouldClose()) {
        UpdateMusicStream(ThemeSong);
        ControlMusic();
        BeginDrawing();
        // Spasi = Pause 
        if (IsKeyPressed(KEY_SPACE)) {
            pause = !pause;
        }
        // Selama blm di pause : 
        if (!pause) {
            ball.Update();
            player.Update();
            cpu.Update(ball.y);

            powerUpTimer += GetFrameTime();
            if (powerUpTimer >= powerUpSpawnTime) {
                powerUp.Spawn();
                powerUpTimer = 0.0f;
            }
        }
        // Bola nabrak paddle 
        if (CheckCollisionCircleRec(
            {ball.x, ball.y}, ball.radius, {player.x, player.y, player.width, player.height})) {
            ball.speed_x *= -1;
            lastTouchBall = PLAYER;
            if (GetTime() - lastSoundTime >= soundCooldown) {
                StopSound(paddlehitsound);
                PlaySound(paddlehitsound);
                lastSoundTime = GetTime(); 
            }
        }
        // Bola nabrak Paddle juga (CPU tapi)
        if (CheckCollisionCircleRec(
            {ball.x, ball.y}, ball.radius, {cpu.x, cpu.y, cpu.width, cpu.height})) {
            ball.speed_x *= -1;
            
            lastTouchBall = CPU;
            if (GetTime() - lastSoundTime >= soundCooldown) {
                StopSound(paddlehitsound);
                PlaySound(paddlehitsound);
                lastSoundTime = GetTime(); 
    }
        }
        // Bola nabrak PowerUp 
        if (powerUp.active && CheckCollisionCircleRec(
            {ball.x, ball.y}, ball.radius,
            {powerUp.x, powerUp.y, powerUp.width, powerUp.height})) {
                StopSound(poweruphitsound);
                PlaySound(poweruphitsound);
            // Milih random 
            currentPowerUp = (PowerUpType)GetRandomValue(0, 2);
            // Bola punya Player
            if (lastTouchBall == PLAYER) {
                switch(currentPowerUp) {
                    case PADDLE_SIZE:
                        player.height *= 2.5;
                        break;
                    case SPEED_CHANGE:
                        cpu.speed *= 0.7;
                        break;
                    case BALL_SPEED:
                        ball.speed_x *= 1.5;
                        ball.speed_y *= 1.5;
                        break;
                }
            } 
            // Bola punya CPU 
            else if (lastTouchBall == CPU) {
                switch(currentPowerUp) {
                    case PADDLE_SIZE:
                        cpu.height *= 2.5;
                        break;
                    case SPEED_CHANGE:
                        player.speed *= 0.7;
                        break;
                    case BALL_SPEED:
                        ball.speed_x *= 1.5;
                        ball.speed_y *= 1.5;
                        break;
                }
            }
            
            powerUp.Reset();
            powerUpActive = true;
            effectTimer = 0.0f;
        }

        if (powerUpActive) {
            effectTimer += GetFrameTime();
            if (effectTimer >= powerUpEffectDuration) {
                if (lastTouchBall == PLAYER) {
                    switch(currentPowerUp) {
                        case PADDLE_SIZE:
                            if (player.height > 120) player.height = 120;
                            break;
                        case SPEED_CHANGE:
                            if (cpu.speed < cpu_speed) cpu.speed = cpu_speed;
                            break;
                        case BALL_SPEED:
                            if (ball.speed_x > 0) ball.speed_x = ball_speed;
                            else ball.speed_x = -ball_speed;
                            if (ball.speed_y > 0) ball.speed_y = ball_speed;
                            else ball.speed_y = -ball_speed;
                            break;
                    }
                } else if (lastTouchBall == CPU) {
                    switch(currentPowerUp) {
                        case PADDLE_SIZE:
                            if (cpu.height > 120) cpu.height = 120;
                            break;
                        case SPEED_CHANGE:
                            if (player.speed < 6) player.speed = 6;
                            break;
                        case BALL_SPEED:
                            if (ball.speed_x > 0) ball.speed_x = ball_speed;
                            else ball.speed_x = -ball_speed;
                            if (ball.speed_y > 0) ball.speed_y = ball_speed;
                            else ball.speed_y = -ball_speed;
                            break;
                    }
                }
                powerUpActive = false;
                lastTouchBall = NONE;
            }
        }
        // Gambar2 
        ClearBackground(BLUE);
        // Tema 
        DrawTexture(themes[currentTheme], 0, 0, WHITE);
        DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);
        DrawLine(screen_width / 2, 0, screen_width/2, screen_height, WHITE);
        DrawLine(screen_width / 2 + 1, 0, screen_width/2 + 1, screen_height, WHITE);
        DrawLine(screen_width / 2 + 2, 0, screen_width/2 + 2, screen_height, WHITE);
        DrawCircleLines(screen_width / 2, screen_height / 2, 200, WHITE);
        DrawCircleLines(screen_width / 2, screen_height / 2, 201, WHITE);
        DrawCircleLines(screen_width / 2, screen_height / 2, 202, WHITE);
        ball.Draw();
        cpu.Draw();
        player.Draw();
        powerUp.Draw();
        // Nulis2
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);
        
        // Buat Pause
        if (pause) {
            DrawText("Paused", 560, 380, 40, GREEN);
            DrawRectangle(0, 0, screen_width, screen_height, Fade(BLACK, 0.5f));
        }
        
        EndDrawing();
    }
    // Unload
    UnloadSound(paddlehitsound);
    UnloadSound(poweruphitsound);
    UnloadMusicStream(ThemeSong);
    UnloadTexture(themes[0]);
    UnloadTexture(themes[1]);
    UnloadTexture(themes[2]);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

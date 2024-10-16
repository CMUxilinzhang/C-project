#include "fssimplewindow.h"
#include "yssimplesound.h"
#include <iostream>
#include <cmath>
#include <time.h>
#include <cstdlib>
#include <vector>

#define PI 3.1415927
#define num_spray 400

// Time step (Fs) and gravity
const float dt = 0.010f;
const float gravity = 170.0f;
// Height of window
const int windowHeight = 600;
// Surface of the water
const float waterLevel = 480.0f;



const float waveAmplitude = 5.0f;    // Height of the waves
const float waveLength = 100.0f;      // Distance between wave peaks
const float waveSpeed = 4.0f;         // Speed of wave movement
float wavePhase = 0.0f;               // Current phase of the wave



// Diver class definition
class Diver
{
public:
    // Diver properties
    float x;
    float y;
    float vx;
    float vy;
    float angle;
    float angularVelocity;
    int R_head; int G_head; int B_head;
    int R_body; int G_body; int B_body;

    // States
    bool START;
    bool JUMPING;
    bool LANDED;
    int pause;

    // Flag to ensure vy is set only once when crossing x >= 150
    bool hasJumped;

    // Spray particles
    static const int num_spray_particles = 550;
    float sx[num_spray_particles], sy[num_spray_particles], svx[num_spray_particles], svy[num_spray_particles];
    bool spray_state;
    int spray_step;

    // Constructor
    Diver();

    // Methods
    void Initialize();
    void Update();
    void Draw();
    void DrawReflection();
    void InitializeSpray();
    void UpdateSpray();
    void DrawSpray();
};

// Diver class methods implementation

Diver::Diver() 
{
    Initialize();
    hasJumped = false;
    spray_state = false;
    spray_step = 0;

    // Initialize spray particles to zero
    for (int i = 0; i < num_spray_particles; ++i) 
    {
        sx[i] = 0.0f;
        sy[i] = 0.0f;
        svx[i] = 0.0f;
        svy[i] = 0.0f;
    }
}

void Diver::Initialize() 
{
    x = 0.0f;
    y = 50.0f;
    vx = 0.0f;
    vy = 0.0f;
    angle = 0.0f;

    int range = rand() % 2;  // 随机选择 0 或 1 来决定使用哪个区间
    if (range == 0) 
    {
        // 区间 (-600, -300)
        angularVelocity = rand() % 301 - 800;  // rand() % 301 生成 0 到 300，减去 600 得到 -600 到 -300
    }
    else {
        // 区间 (200, 400)
        angularVelocity = rand() % 301 + 500;  // rand() % 201 生成 0 到 200，加上 200 得到 200 到 400
    }
    // angularVelocity = 0.0f;
    // Do not reset START, JUMPING, LANDED here       
    //std::cout << "角速度为" << angularVelocity;
}

void Diver::Update() 
{
    // Check if diver has crossed x >= 150 and hasn't jumped yet
    if (x >= 150.0f && !hasJumped) 
    {
        vy = -50.0f;
        hasJumped = true;
        // Debug output
        std::cout << "Diver has crossed x >= 150, vy set to -50.0f\n";
    }


    if (x >= 150.0f) 
    {
        vx = 40.0f;
        // Update velocity and position
        vy += gravity * dt;
        x += vx * dt;
        y += vy * dt;
        if (y < 310 && y > 100)
        {
            angle += angularVelocity * dt;
        }
        // std::cout << "角度为" << angle << "角速度为" << angularVelocity;
    }
    else {
        x += vx * dt;
    }

    // Check if landed
    if (y > waterLevel && JUMPING) 
    {
        LANDED = true;
        JUMPING = false;
        // Debug output
        std::cout << "Diver has landed in water.\n";
    }
}

void Diver::Draw() {
    // Set the diver.x and diver.y as the geometric center
    glPushMatrix();
    glTranslatef(x, y, 0.0f); // Set the geometric center
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    // Activate alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw the head
    glColor3ub(R_head, G_head, B_head); // The color of the head
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, -10.0f); // The center of the head
    for (int i = 0; i <= 360; i += 30)
    {
        float rad = i * PI / 180.0f;
        glVertex2f(3.0f * cos(rad), 10.0f * sin(rad) - 10.0f); // Draw the head
    }
    glEnd();

    // Draw the body
    glColor4ub(R_body, G_body, B_body, 255); // Black
    glBegin(GL_QUADS);
    glVertex2f(-4.0f, -5.0f);
    glVertex2f(4.0f, -5.0f);
    glVertex2f(4.0f, 12.0f);
    glVertex2f(-4.0f, 12.0f);
    glEnd();



    glColor4ub(0, 0, 0, 255);
    if (y <= 310.0f && x != 150.0f)
    {
        if (y > 100 && angularVelocity != 0)
        {
            // Draw the left arm
            glBegin(GL_LINES);
            glVertex2f(-4.0f, -5.0f);
            glVertex2f(10.0f, 4.0f);
            glEnd();

            // Right arm
            glBegin(GL_LINES);
            glVertex2f(0.0f, -8.0f);
            glVertex2f(10.0f, 1.0f);
            glEnd();

            // Draw the left leg
            glBegin(GL_LINES);
            glVertex2f(0.0f, 10.0f);
            glVertex2f(7.0f, -7.0f);
            glEnd();

            // Right leg
            glBegin(GL_LINES);
            glVertex2f(0.0f, 10.0f);
            glVertex2f(7.0f, -4.0f);
            glEnd();
        }
        else
        {
            // Draw the left arm
            glBegin(GL_LINES);
            glVertex2f(-4.0f, -5.0f);
            glVertex2f(-10.0f, 4.0f);
            glEnd();

            // Right arm
            glBegin(GL_LINES);
            glVertex2f(4.0f, -5.0f);
            glVertex2f(10.0f, 4.0f);
            glEnd();

            // Draw the left leg
            glBegin(GL_LINES);
            glVertex2f(0.0f, 10.0f);
            glVertex2f(-7.0f, 30.0f);
            glEnd();

            // Right leg
            glBegin(GL_LINES);
            glVertex2f(0.0f, 10.0f);
            glVertex2f(7.0f, 30.0f);
            glEnd();
        }
    }
    else if (fabs(x - 150.0f) < 0.1f) // Using epsilon for float comparison
    {
        // Draw the left arm
        glBegin(GL_LINES);
        glVertex2f(-4.0f, -5.0f);
        glVertex2f(-10.0f, 4.0f);
        glEnd();

        // Right arm
        glBegin(GL_LINES);
        glVertex2f(4.0f, -5.0f);
        glVertex2f(10.0f, 4.0f);
        glEnd();

        // Left leg (bent, ready to jump)
        glBegin(GL_LINES);
        glVertex2f(0.0f, 10.0f);
        glVertex2f(-5.0f, 20.0f);
        glVertex2f(-5.0f, 20.0f);
        glVertex2f(-2.0f, 30.0f);
        glEnd();

        // Right leg (bent, ready to jump)
        glBegin(GL_LINES);
        glVertex2f(0.0f, 10.0f);
        glVertex2f(5.0f, 20.0f);
        glVertex2f(5.0f, 20.0f);
        glVertex2f(2.0f, 30.0f);
        glEnd();
    }
    else
    {
        // Left and right arm
        glBegin(GL_LINES);
        glVertex2f(-5.0f, -7.0f);
        glVertex2f(0.0f, -40.0f);
        glEnd();

        glBegin(GL_LINES);
        glVertex2f(5.0f, -7.0f);
        glVertex2f(0.0f, -40.0f);
        glEnd();

        // Left leg and right leg
        glBegin(GL_LINES);
        glVertex2f(-2.0f, 12.0f);
        glVertex2f(-2.0f, 40.0f);
        glEnd();

        glBegin(GL_LINES);
        glVertex2f(2.0f, 12.0f);
        glVertex2f(2.0f, 40.0f);
        glEnd();
    }
    glDisable(GL_BLEND);

    glPopMatrix();
}

void Diver::DrawReflection() {
    // Shadow of diver
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    // Alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Diver body shadow
    glColor4ub(255, 224, 189, 150);
    glBegin(GL_QUADS);
    glVertex2f(-5.0f, 0.0f);
    glVertex2f(5.0f, 0.0f);
    glVertex2f(5.0f, 30.0f);
    glVertex2f(-5.0f, 30.0f);
    glEnd();

    // Diver head shadow
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, -5.0f); // The top of the head
    for (int i = 0; i <= 360; i += 30)
    {
        float rad = i * PI / 180.0f;
        glVertex2f(5.0f * cos(rad), -5.0f * sin(rad) - 5.0f);
    }
    glEnd();

    glDisable(GL_BLEND);

    glPopMatrix();
}

void Diver::InitializeSpray() {
    // Detect the position of entering the water
    // Horizontal entry

    std::cout << "angle into the water:"<<angle<<'\n';
    if ((fmod(fabs(angle), 360.0) >= 35 && fmod(fabs(angle), 360.0) <= 145) ||
        (fmod(fabs(angle), 360.0) >= 215 && fmod(fabs(angle), 360.0) <= 325)) {
        for (int i = 0; i < num_spray_particles; ++i) {
            if (i < num_spray_particles / 2) {
                sx[i] = rand() % 60 + x - 30;
                sy[i] = y;
                svx[i] = (rand() % 20 - 10) / 11.0f;
                svy[i] = (rand() % 200 - 200) / 111.0f;
            }
            else if (i >= 5 * num_spray_particles / 6) {
                sx[i] = rand() % 55 + x - 27;
                sy[i] = y;
                svx[i] = (rand() % 25 - 12) / 11.0f;
                svy[i] = (rand() % 300 - 300) / 111.0f;
            }
            else {
                sx[i] = rand() % 50 + x - 25;
                sy[i] = y;
                svx[i] = (rand() % 50 - 25) / 11.0f;
                svy[i] = (rand() % 200 - 200) / 111.0f;
            }
        }
    }
    // Vertical entry
    else {
        for (int i = 0; i < num_spray_particles; ++i) {
            if (i < num_spray_particles / 3) {
                sx[i] = rand() % 10 + x - 5;
                sy[i] = y;
                svx[i] = (rand() % 20 - 10) / 11.0f;
                svy[i] = (rand() % 160 - 160) / 111.0f;
            }
            else if (i >= 2 * num_spray_particles / 3) {
                sx[i] = rand() % 5 + x - 2.5f;
                sy[i] = y;
                svx[i] = (rand() % 10 - 5) / 11.0f;
                svy[i] = (rand() % 200 - 200) / 111.0f;
            }
            else {
                sx[i] = rand() % 2 + x - 1;
                sy[i] = y;
                svx[i] = (rand() % 10 - 5) / 11.0f;
                svy[i] = (rand() % 240 - 240) / 111.0f;
            }
        }
    }
    spray_state = true;
    spray_step = 0;
    // Debug output
    std::cout << "Spray particles initialized.\n";
}

void Diver::UpdateSpray() {
    for (int i = 0; i < num_spray_particles; ++i) {
        sx[i] += svx[i];
        sy[i] += svy[i];
        svy[i] += 3 * dt;
    }
    spray_step++;
}

void Diver::DrawSpray() 
{

    if (spray_step < 300)     {

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4ub(25, 25, 112, 60);
        glPointSize(2);
        glBegin(GL_POINTS);
        for (int i = 0; i < num_spray_particles; ++i) {
            if (sy[i] <= waterLevel + 10) {   // 仅绘制水面以上的喷雾
                glVertex2i(static_cast<int>(sx[i]), static_cast<int>(sy[i]));
            }
        }
        glEnd();
        glDisable(GL_BLEND);
    }
}

void DrawPlatform()
{
    // Jumping board
    glColor3ub(139, 69, 19);
    glBegin(GL_QUADS);
    glVertex2i(0, 80);
    glVertex2i(150, 80);
    glVertex2i(150, 100);
    glVertex2i(0, 100);
    glEnd();

    // Supporting column
    glColor3ub(105, 105, 105);
    glBegin(GL_QUADS);
    glVertex2i(90, 100);
    glVertex2i(110, 100);
    glVertex2i(110, 500);
    glVertex2i(90, 500);
    glEnd();
}

void DrawWater()
{
    // Alpha blending to implement the transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw the swimming pool background
    //glBegin(GL_QUADS);
    //glColor4ub(0, 191, 255, 150); // Light blue with transparency
    //glVertex2i(0, (int)waterLevel);
    //glVertex2i(800, (int)waterLevel);
    //glColor4ub(25, 25, 112, 150); // Darker blue with transparency
    //glVertex2i(800, windowHeight);
    //glVertex2i(0, windowHeight);
    //glEnd();

    // Draw multiple wave layers for realism
    glColor4ub(25, 25, 112, 230); // Slightly opaque waves
    glBegin(GL_TRIANGLE_STRIP);
    int numPoints = 800;
    for (int x = 0; x <= numPoints; ++x)
    {
        float currentX = static_cast<float>(x);
        float y = waterLevel;

        // First wave layer
        y += waveAmplitude * 0.7f * sin((2 * PI / waveLength) * currentX + wavePhase);

        // Second wave layer with different frequency and amplitude
        y += (waveAmplitude * 0.3f) * sin((2 * PI / (waveLength / 2)) * currentX + wavePhase * 1.5f);

        glVertex2f(currentX, y);
        glVertex2f(currentX, windowHeight);
    }
    glEnd();

    // Disable blending
    glDisable(GL_BLEND);
}


// audience with perspective
void DrawSpectatorStand(int windowWidth, int windowHeight, float waterLevel)
{
    // parameters of the audience
    int numRows = 40;               // 观众席行数，根据窗口高度调整
    int seatsPerRow = 30;           // 每行座位数，根据窗口宽度调整
    float baseSeatWidth = 20.0f;    // 前排座位宽度
    float baseSeatHeight = 6.0f;   // 前排座位高度
    float seatSpacingX = 5.0f;      // 座位间横向间距
    float seatSpacingY = 10.0f;      // 行间纵向间距
    float initialScale = 1.0f;      // 前排座位的初始缩放比例
    float minScale = 0.65f;          // 最后一行座位的最小缩放比例
    float scaleStep = (initialScale - minScale) / (numRows - 1); // 线性缩放步长

    // 座位深度（用于斜二测画法）
    float seatDepth = 5.0f; // 可以根据需要调整

    // 斜二测的角度（通常为45度）
    float obliqueAngle = 45.0f;
    float obliqueRad = obliqueAngle * PI / 180.0f;
    float obliqueFactor = cos(obliqueRad); // 水平方向的缩放因子
    float obliqueHeight = sin(obliqueRad) * seatDepth; // 垂直方向的偏移
     
    // 前排座位的起始位置（靠近窗口底部）
    float yStart = waterLevel - baseSeatHeight - 20.0f; // 调整偏移量以避免与水面重叠

    // 初始化随机种子（如果尚未初始化）
    static bool seedInitialized = false;
    if (!seedInitialized)
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        seedInitialized = true;
    }

    // 遍历每一行
    for (int row = 0; row < numRows; row++)
    {
        // 计算当前行的缩放比例（线性缩放）
        float currentScale = initialScale - row * scaleStep;
        float seatWidth = baseSeatWidth * currentScale;
        float seatHeight = baseSeatHeight * currentScale;

        // 计算当前行的Y坐标
        float y = yStart - row * (seatHeight + seatSpacingY);

        // 确保当前行不超出窗口顶部
        if (y + seatHeight < 0)
            break;

        // 颜色渐变：后方颜色较浅
        float intensity = 1.0f - (static_cast<float>(row) / numRows) * 0.5f; // 颜色变化不宜过大
        glColor3f(0.6f * intensity, 0.3f * intensity, 0.0f); // 棕色渐变

        // 计算当前行的总宽度
        float totalWidth = seatsPerRow * (seatWidth + seatSpacingX);

        // 计算当前行的起始X坐标，使其居中
        float xStart = (windowWidth - totalWidth) / 2.0f + 200 ;

        // 绘制每个座位
        for (int seat = 0; seat < seatsPerRow; seat++)
        {
            float x = xStart + seat * (seatWidth + seatSpacingX);

            // 绘制座位的前面（前视面）
            glColor3f(0.6f * intensity, 0.3f * intensity, 0.0f); // 棕色
            glBegin(GL_QUADS);
            glVertex2f(x, y); // 左上
            glVertex2f(x + seatWidth, y); // 右上
            glVertex2f(x + seatWidth, y + seatHeight); // 右下
            glVertex2f(x, y + seatHeight); // 左下
            glEnd();

            // 绘制座位的右侧面
            glColor3f(0.5f * intensity, 0.25f * intensity, 0.0f); // 深一点的棕色
            glBegin(GL_QUADS);
            glVertex2f(x + seatWidth, y); // 前右上
            glVertex2f(x + seatWidth + obliqueFactor * seatDepth, y - obliqueHeight); // 后右上
            glVertex2f(x + seatWidth + obliqueFactor * seatDepth, y + seatHeight - obliqueHeight); // 后右下
            glVertex2f(x + seatWidth, y + seatHeight); // 前右下
            glEnd();

            // 绘制座位的顶部面
            glColor3f(0.55f * intensity, 0.275f * intensity, 0.0f); // 中间色
            glBegin(GL_QUADS);
            glVertex2f(x, y); // 前左上
            glVertex2f(x + seatWidth, y); // 前右上
            glVertex2f(x + seatWidth + obliqueFactor * seatDepth, y - obliqueHeight); // 后右上
            glVertex2f(x + obliqueFactor * seatDepth, y - obliqueHeight); // 后左上
            glEnd();

            // 恢复颜色用于下一个座位
            glColor3f(0.6f * intensity, 0.3f * intensity, 0.0f);

            // 绘制座位间隔线（前视面的垂直线）
            glColor3f(0.0f, 0.0f, 0.0f); // 黑色
            glBegin(GL_LINES);
            glVertex2f(x, y);
            glVertex2f(x, y + seatHeight);
            glEnd();
        }

        // 绘制观众
        for (int seat = 0; seat < seatsPerRow; seat++)
        {
            float x = xStart + seat * (seatWidth + seatSpacingX) + seatWidth / 2.0f;
            float ySeatCenter = y + seatHeight / 2.0f;

            // 设置颜色为黑色
            glColor3f(0.0f, 0.0f, 0.0f); // 黑色

            // 根据缩放比例调整火柴人的大小
            float scale = currentScale * 0.7f; // 调整火柴人的整体大小
            float headRadius = 3.0f * scale;
            float bodyLength = 12.0f * scale;
            float armLength = 6.0f * scale;
            float legLength = 9.0f * scale;

            // 火柴人头部位置（头部在身体上方）
            float headY = ySeatCenter - bodyLength - headRadius;

            // 头部
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, headY); // 头部中心
            int numSegments = 8;
            for (int angle = 0; angle <= 360; angle += 360 / numSegments)
            {
                float rad = angle * PI / 180.0f;
                glVertex2f(x + headRadius * cos(rad), headY + headRadius * sin(rad));
            }
            glEnd();

            // 身体
            glBegin(GL_LINES);
            glVertex2f(x, headY + headRadius); // 颈部
            glVertex2f(x, ySeatCenter); // 腰部
            glEnd();

            // 手臂
            glBegin(GL_LINES);
            // 左臂
            glVertex2f(x, headY + headRadius + bodyLength * 0.3f); // 身体中上部
            glVertex2f(x - armLength, headY + headRadius + bodyLength * 0.3f - armLength); // 左臂
            // 右臂
            glVertex2f(x, headY + headRadius + bodyLength * 0.3f); // 身体中上部
            glVertex2f(x + armLength, headY + headRadius + bodyLength * 0.3f - armLength); // 右臂
            glEnd();

            // 腿
            glBegin(GL_LINES);
            // 左腿
            glVertex2f(x, ySeatCenter); // 腰部
            glVertex2f(x - legLength, ySeatCenter + legLength); // 左腿
            // 右腿
            glVertex2f(x, ySeatCenter); // 腰部
            glVertex2f(x + legLength, ySeatCenter + legLength); // 右腿
            glEnd();
        }
    }
}







int main(void)
{
    FsOpenWindow(0, 0, 800, windowHeight, 1);

    // Manage multiple Diver objects
    std::vector<Diver> divers;

    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));

    // Create the first Diver and start jumping
    Diver firstDiver;
    firstDiver.START = false;
    firstDiver.JUMPING = true;
    firstDiver.LANDED = false;
    firstDiver.vx = 100.0f; // Initialize horizontal speed
    firstDiver.vy = 0.0f;   // Initialize vertical speed
    // firstDiver.angularVelocity = 0.0f;
    firstDiver.angle = 0.0f;
    firstDiver.pause = 50;
    firstDiver.hasJumped = false;
    firstDiver.R_head = 0; firstDiver.G_head = 0; firstDiver.B_head = 255;
    divers.push_back(firstDiver);

    while (true)
    {
        FsPollDevice();

        // Check for exit condition
        if (0 != FsGetKeyState(FSKEY_ESC))
        {
            break;
        }

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        // Update wave phase for animation
        wavePhase += waveSpeed * dt;
        if (wavePhase > 2 * PI)
        {
            wavePhase -= 2 * PI;
        }


        DrawSpectatorStand(800,600,400);
        DrawWater();
        DrawPlatform();

        // Iterate through all Diver objects
        for (size_t i = 0; i < divers.size(); )
        {
            Diver& diver = divers[i];

            if (diver.JUMPING && !diver.LANDED)
            {

                
                //// Control angular velocity (no rotation)
                //diver.angularVelocity = 0.0f;

                //// detect the input
                //if (0 != FsGetKeyState(FSKEY_UP) || 0 != FsGetKeyState(FSKEY_LEFT))
                //{
                //    diver.angularVelocity = -500.0f;
                //}
                //// detect the input
                //else if (0 != FsGetKeyState(FSKEY_DOWN) || 0 != FsGetKeyState(FSKEY_RIGHT))
                //{
                //    diver.angularVelocity = 500.0f;
                //}
                //else
                //{
                //    // stop rotating if there is no input
                //    diver.angularVelocity = 0.0f;
                //}
                



                // if pauseCounter > 0，stop updating
                if (diver.x == 150 && diver.pause > 0)
                {
                    diver.pause--;
                }
                else
                {
                    diver.Update();
                }

                // Draw diver
                diver.Draw();

                // If diver has landed, initialize spray and add a new Diver
                if (diver.LANDED)
                {
                    diver.InitializeSpray();

                    // Add a new Diver to the vector
                    Diver newDiver;
                    newDiver.START = false;
                    newDiver.JUMPING = true;
                    newDiver.LANDED = false;
                    newDiver.vx = 100.0f; // Initialize horizontal speed
                    newDiver.vy = 0.0f;   // Initialize vertical speed
                    // newDiver.angularVelocity = 0.0f;
                    newDiver.angle = 0.0f;
                    newDiver.hasJumped = false;
                    newDiver.pause = 50;
                    newDiver.R_head = rand() % 255;
                    newDiver.G_head = rand() % 255;
                    newDiver.B_head = rand() % 255;
                    newDiver.R_body = rand() % 255;
                    newDiver.G_body = rand() % 255;
                    newDiver.B_body = rand() % 255;
                    divers.push_back(newDiver);

                    // Debug output
                    std::cout << "New Diver added. Total Divers: " << divers.size() << "\n";
                }
            }


            // Handle spray effects
            if (diver.spray_state)
            {
                diver.UpdateSpray();
                //DrawWater();
                diver.DrawSpray();
                //DrawWater();

                if (diver.spray_step >= 300)
                {
                    diver.spray_state = false;
                }
            }

            // Draw diver's reflection
            glPushMatrix();
            glTranslatef(0.0f, 2 * waterLevel + 20, 0.0f);
            glScalef(1.0f, -1.0f, 1.0f);

            DrawPlatform();

            if (diver.JUMPING)
            {
                diver.DrawReflection();
            }

            glPopMatrix();

            // Remove Diver if spray is done and diver has landed
            if (!diver.JUMPING && !diver.spray_state) {
                divers.erase(divers.begin() + i);
                std::cout << "Diver removed. Remaining Divers: " << divers.size() << "\n";
                // Do not increment i since the current index is removed
            }
            else {
                ++i;
            }
        }

        FsSwapBuffers();
        FsSleep(10);
    }
    return 0;
}

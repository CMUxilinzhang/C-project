#include "fssimplewindow.h"
#include "yssimplesound.h"
#include "ysglfontdata.h"
#include <iostream>
#include <cmath>
#include <time.h>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <string> 

#define PI 3.1415927
#define num_spray 400

const float dt = 0.010f;              // Time step
const float gravity = 170.0f;         // Gravity
const int windowHeight = 600;         // Height of window
const float waterLevel = 480.0f;      // Surface of the water
const float waveAmplitude = 5.0f;     // Height of the waves
const float waveLength = 100.0f;      // Distance between wave peaks
const float waveSpeed = 4.0f;         // Speed of wave movement
float wavePhase = 0.0f;               // Current phase of the wave


// Game Status
enum class GameState
{
    GAME,
    RESULTS
};


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
    float score;
    int R_head; int G_head; int B_head;
    int R_body; int G_body; int B_body;

    // States
    bool START;
    bool JUMPING;
    bool LANDED;
    bool soundPlayed;
    bool isActive;
    int pause;
    int pause2;

    // Ensure vy is set only once when crossing x >= 150
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
    soundPlayed = false;
    isActive = true;

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

    int range = rand() % 2;  // 0 or 1 (angularVelosity is positive or negative)
    if (range == 0)
    {
        // region (-1300, -500)
        angularVelocity = rand() % 801 - 1300;
    }
    else
    {
        // region (500, 1300)
        angularVelocity = rand() % 801 + 500;
    }
    //std::cout << "angularVelosity" << angularVelocity;
}

void Diver::Update()
{
    if (!isActive)
        return;

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
    }
    else
    {
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

void Diver::Draw()
{
    if (!isActive)
        return;
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
        glVertex2f(3.0f * cos(rad), 10.0f * sin(rad) - 10.0f);
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
            // Bend the body
            // Left arm
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
    else if (fabs(x - 150.0f) < 0.1f) // float comparison Using epsilon for 
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

void Diver::InitializeSpray() 
{
    // Detect the position of entering the water

    // Horizontal entry
    std::cout << "angle into the water:" << angle << '\n';
    std::cout << "angleVelosity :" << angularVelocity << '\n';

    if ((fmod(fabs(angle), 360.0) >= 100 && fmod(fabs(angle), 360.0) <= 260))
    {
        score = fmod(fabs(angle), 360.0);
        score = 100.0f - fabs((score - 180.0f));
        std::cout << "THE SCORE IS " << score << '\n';
    }
    else
    {
        score = 0;
        std::cout << "THE SCORE IS " << score << '\n';
    }

    if ((fmod(fabs(angle), 360.0) >= 35 && fmod(fabs(angle), 360.0) <= 145) ||
        (fmod(fabs(angle), 360.0) >= 215 && fmod(fabs(angle), 360.0) <= 325)) 
    {
        for (int i = 0; i < num_spray_particles; ++i) 
        {
            if (i < num_spray_particles / 2) 
            {
                sx[i] = rand() % 60 + x - 30;
                sy[i] = y;
                svx[i] = (rand() % 20 - 10) / 11.0f;
                svy[i] = (rand() % 200 - 200) / 111.0f;
            }
            else if (i >= 5 * num_spray_particles / 6) 
            {
                sx[i] = rand() % 55 + x - 27;
                sy[i] = y;
                svx[i] = (rand() % 25 - 12) / 11.0f;
                svy[i] = (rand() % 300 - 300) / 111.0f;
            }
            else 
            {
                sx[i] = rand() % 50 + x - 25;
                sy[i] = y;
                svx[i] = (rand() % 50 - 25) / 11.0f;
                svy[i] = (rand() % 200 - 200) / 111.0f;
            }
        }
    }

    // Vertical entry
    else 
    {
        for (int i = 0; i < num_spray_particles; ++i) 
        {
            if (i < num_spray_particles / 3) 
            {
                sx[i] = rand() % 10 + x - 5;
                sy[i] = y;
                svx[i] = (rand() % 20 - 10) / 11.0f;
                svy[i] = (rand() % 160 - 160) / 111.0f;
            }
            else if (i >= 2 * num_spray_particles / 3) 
            {
                sx[i] = rand() % 5 + x - 2.5f;
                sy[i] = y;
                svx[i] = (rand() % 10 - 5) / 11.0f;
                svy[i] = (rand() % 200 - 200) / 111.0f;
            }
            else 
            {
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

    if (spray_step < 300) 
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4ub(25, 25, 112, 60);
        glPointSize(2);
        glBegin(GL_POINTS);
        for (int i = 0; i < num_spray_particles; ++i) 
        {
            if (sy[i] <= waterLevel + 10) // Draw the spray above water only
            {
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
    int numRows = 40;               
    int seatsPerRow = 30;           
    float baseSeatWidth = 20.0f;   
    float baseSeatHeight = 6.0f;   
    float seatSpacingX = 5.0f;     
    float seatSpacingY = 10.0f;    
    float initialScale = 1.0f;     
    float minScale = 0.65f;        
    float scaleStep = (initialScale - minScale) / (numRows - 1);

    // Depth of the seat (for oblique projection)
    float seatDepth = 5.0f;

    // Angle for oblique projection  (45)
    float obliqueAngle = 45.0f;
    float obliqueRad = obliqueAngle * PI / 180.0f;
    float obliqueFactor = cos(obliqueRad); // Horizontal scaling factor
    float obliqueHeight = sin(obliqueRad) * seatDepth; // Vertical offset

    // Starting position of the front row
    float yStart = waterLevel - baseSeatHeight - 20.0f; // Adjust offset to avoid overlap with water surface


    // Initialize random seed (if not already initialized)
    static bool seedInitialized = false;
    if (!seedInitialized)
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        seedInitialized = true;
    }

    // Iterate through each row
    for (int row = 0; row < numRows; row++)
    {
        // Calculate the scaling factor for the current row (linear scaling)
        float currentScale = initialScale - row * scaleStep;
        float seatWidth = baseSeatWidth * currentScale;
        float seatHeight = baseSeatHeight * currentScale;

        // Calculate the Y coordinate for the current row
        float y = yStart - row * (seatHeight + seatSpacingY);

        // Ensure the current row does not exceed the top of the window
        if (y + seatHeight < 0)
            break;

        // Color gradient: seats further back are lighter
        float intensity = 1.0f - (static_cast<float>(row) / numRows) * 0.5f; // Avoid too much color change
        glColor3f(0.6f * intensity, 0.3f * intensity, 0.0f); // Brown gradient

        // Calculate the total width of the current row
        float totalWidth = seatsPerRow * (seatWidth + seatSpacingX);

        // Calculate the starting X coordinate for the current row, centering it
        float xStart = (windowWidth - totalWidth) / 2.0f + 200;

        // Draw each seat
        for (int seat = 0; seat < seatsPerRow; seat++)
        {
            float x = xStart + seat * (seatWidth + seatSpacingX);

            // Draw the front face of the seat
            glColor3f(0.6f * intensity, 0.3f * intensity, 0.0f); // Brown
            glBegin(GL_QUADS);
            glVertex2f(x, y); // Top left
            glVertex2f(x + seatWidth, y); // Top right
            glVertex2f(x + seatWidth, y + seatHeight); // Bottom right
            glVertex2f(x, y + seatHeight); // Bottom left
            glEnd();

            // Draw the right side of the seat
            glColor3f(0.5f * intensity, 0.25f * intensity, 0.0f); // Darker brown
            glBegin(GL_QUADS);
            glVertex2f(x + seatWidth, y); // Front top right
            glVertex2f(x + seatWidth + obliqueFactor * seatDepth, y - obliqueHeight); // Back top right
            glVertex2f(x + seatWidth + obliqueFactor * seatDepth, y + seatHeight - obliqueHeight); // Back bottom right
            glVertex2f(x + seatWidth, y + seatHeight); // Front bottom right
            glEnd();

            // Draw the top face of the seat
            glColor3f(0.55f * intensity, 0.275f * intensity, 0.0f); // Intermediate color
            glBegin(GL_QUADS);
            glVertex2f(x, y); // Front top left
            glVertex2f(x + seatWidth, y); // Front top right
            glVertex2f(x + seatWidth + obliqueFactor * seatDepth, y - obliqueHeight); // Back top right
            glVertex2f(x + obliqueFactor * seatDepth, y - obliqueHeight); // Back top left
            glEnd();

            // Reset color for the next seat
            glColor3f(0.6f * intensity, 0.3f * intensity, 0.0f);

            // Draw seat spacing line (vertical line on the front face)
            glColor3f(0.0f, 0.0f, 0.0f); // Black
            glBegin(GL_LINES);
            glVertex2f(x, y);
            glVertex2f(x, y + seatHeight);
            glEnd();
        }

        // »æÖÆ¹ÛÖÚ
        // Draw spectators
        for (int seat = 0; seat < seatsPerRow; seat++)
        {
            float x = xStart + seat * (seatWidth + seatSpacingX) + seatWidth / 2.0f;
            float ySeatCenter = y + seatHeight / 2.0f;

            // Set color to black
            glColor3f(0.0f, 0.0f, 0.0f); // Black

            // Adjust the size of the stick figure based on scaling factor
            float scale = currentScale * 0.78f; // Adjust the overall size of the stick figure
            float headRadius = 3.0f * scale;
            float bodyLength = 12.0f * scale;
            float armLength = 6.0f * scale;
            float legLength = 9.0f * scale;

            // Position of the stick figure's head (above the body)
            float headY = ySeatCenter - bodyLength - headRadius;

            // Head
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, headY); // Center of the head
            int numSegments = 8;
            for (int angle = 0; angle <= 360; angle += 360 / numSegments)
            {
                float rad = angle * PI / 180.0f;
                glVertex2f(x + headRadius * cos(rad), headY + headRadius * sin(rad));
            }
            glEnd();

            // Body
            glBegin(GL_LINES);
            glVertex2f(x, headY + headRadius); // Neck
            glVertex2f(x, ySeatCenter); // Waist
            glEnd();

            // Arms
            glBegin(GL_LINES);
            // Left arm
            glVertex2f(x, headY + headRadius + bodyLength * 0.3f); // Upper body
            glVertex2f(x - armLength, headY + headRadius + bodyLength * 0.3f - armLength); // Left arm
            // Right arm
            glVertex2f(x, headY + headRadius + bodyLength * 0.3f); // Upper body
            glVertex2f(x + armLength, headY + headRadius + bodyLength * 0.3f - armLength); // Right arm
            glEnd();

            // Legs
            glBegin(GL_LINES);
            // Left leg
            glVertex2f(x, ySeatCenter); // Waist
            glVertex2f(x - legLength, ySeatCenter + legLength); // Left leg
            // Right leg
            glVertex2f(x, ySeatCenter); // Waist
            glVertex2f(x + legLength, ySeatCenter + legLength); // Right leg
            glEnd();
        }
    }
}


void Drawclaps(int windowWidth, int windowHeight, float waterLevel)
{
    // parameters of the audience
    int numRows = 40;           
    int seatsPerRow = 30;       
    float baseSeatWidth = 20.0f;
    float baseSeatHeight = 6.0f;
    float seatSpacingX = 5.0f;  
    float seatSpacingY = 10.0f; 
    float initialScale = 1.0f;  
    float minScale = 0.65f;     
    float scaleStep = (initialScale - minScale) / (numRows - 1); // Linear scaling step


    // Starting position of the front row (near the bottom of the window)
    float yStart = waterLevel - baseSeatHeight - 20.0f; // Adjust offset to avoid overlap with water surface

    // Initialize random seed (if not already initialized)
    static bool seedInitialized = false;
    if (!seedInitialized)
    {
        srand(static_cast<unsigned int>(time(nullptr)));
        seedInitialized = true;
    }

    // Iterate through each row
    for (int row = 0; row < numRows; row++)
    {
        // Calculate the scaling factor for the current row (linear scaling)
        float currentScale = initialScale - row * scaleStep;
        float seatWidth = baseSeatWidth * currentScale;
        float seatHeight = baseSeatHeight * currentScale;

        // Calculate the Y coordinate for the current row
        float y = yStart - row * (seatHeight + seatSpacingY);

        // Ensure the current row does not exceed the top of the window
        if (y + seatHeight < 0)
            break;

        // Color gradient: seats further back are lighter
        float intensity = 1.0f - (static_cast<float>(row) / numRows) * 0.5f; // Avoid too much color change
        glColor3f(0.6f * intensity, 0.3f * intensity, 0.0f); // Brown gradient

        // Calculate the total width of the current row
        float totalWidth = seatsPerRow * (seatWidth + seatSpacingX);

        // Calculate the starting X coordinate for the current row, centering it
        float xStart = (windowWidth - totalWidth) / 2.0f + 200;

        // Draw spectators
        for (int seat = 0; seat < seatsPerRow; seat++)
        {
            float x = xStart + seat * (seatWidth + seatSpacingX) + seatWidth / 2.0f;
            float ySeatCenter = y + seatHeight / 2.0f;

            // Random color representing different spectators
            glColor3ub(rand() % 256, rand() % 256, rand() % 256);

            // Adjust spectator size based on scaling factor
            float radius = 4.0f * currentScale; // Adjust the spectator radius

            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, ySeatCenter); // Center of the spectator
            int numSegments = 8;
            for (int angle = 0; angle <= 360; angle += 360 / numSegments)
            {
                float rad = angle * PI / 180.0f;
                glVertex2f(x + radius * cos(rad), ySeatCenter + radius * sin(rad));
            }
            glEnd();
        }
    }
}


void DrawTextOnScreen(const char* text, int x, int y, int r, int g, int b)
{

    glColor3ub(r, g, b);
    // Set the position for the text
    glRasterPos2i(x, y);

    // Render the text using 6x7 font, you can change it to other sizes as needed
    YsGlDrawFontBitmap20x28(text);
}


int main(void)
{
    FsOpenWindow(0, 0, 800, windowHeight, 1);


    YsSoundPlayer player;
    YsSoundPlayer::SoundData landingSound, yeah, boo, victory, fail;

    //
    if (YSOK != landingSound.LoadWav("splash.wav"))                //ATTENTION
    {                                                              //DO NOT PUT 
        std::cerr << "Failed to load splash.wav.\n";               //player.Start()
        return -1;                                                 //INTO FOR LOOP  
    }                                                             
    else                                                          
    {                                                             
        std::cout << "splash.wav sound loaded successfully.\n";   
    }                                                             

    //
    if (YSOK != yeah.LoadWav("yeah.wav"))
    {
        std::cerr << "fail to load yeah.wav. \n";
    }
    else
    {
        std::cout << "yeah.wav sound loaded successfully.\n";
    }

    //
    if (YSOK != boo.LoadWav("boo.wav"))
    {
        std::cerr << "fail to load boo.wav.\n";
    }
    else
    {
        std::cout << "boo.wav sound loaded successfully.\n";
    }

    //
    if (YSOK != victory.LoadWav("victory.wav"))
    {
        std::cerr << "fail to load boo.wav.\n";
    }
    else
    {
        std::cout << "boo.wav sound loaded successfully.\n";
    }

    //
    if (YSOK != fail.LoadWav("fail.wav"))
    {
        std::cerr << "fail to load boo.wav.\n";
    }
    else
    {
        std::cout << "boo.wav sound loaded successfully.\n";
    }


    player.Start();

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
    firstDiver.pause2 = 0;
    firstDiver.hasJumped = false;
    firstDiver.R_head = 0; firstDiver.G_head = 0; firstDiver.B_head = 255;
    firstDiver.R_body = 0;
    firstDiver.G_body = 0;
    firstDiver.B_body = 0;
    divers.push_back(firstDiver);


    GameState gameState = GameState::GAME;
    int maxDivers = 7; // 7 - 1 = 6     six divers
    int exitgame = 901;

    while (true)
    {
        FsPollDevice();

        // Check for exit condition
        if (0 != FsGetKeyState(FSKEY_ESC))
        {
            for (int j = 0; j < divers.size() - 1; j++)
            {
                std::cout << "The score of " << j + 1 << "th diver is " << divers[j].score << '\n';
            }
            break;
        }


        if (gameState == GameState::GAME)
        {
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            // Update wave phase for animation
            wavePhase += waveSpeed * dt;
            if (wavePhase > 2 * PI)
            {
                wavePhase -= 2 * PI;
            }


            DrawSpectatorStand(800, 600, 400);
            DrawWater();
            DrawPlatform();

            // Iterate through all Diver objects
            for (size_t i = 0; i < divers.size(); )
            {
                Diver& diver = divers[i]; // ATTENTION: use & 

                if (!diver.isActive)
                {
                    ++i;
                    continue;
                }

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


                    // if pauseCounter > 0£¬stop updating
                    if (diver.x == 150 && diver.pause > 0)
                    {
                        diver.pause--;
                        std::cout << diver.pause;
                    }
                    else if (diver.x == 0 && diver.pause2 > 0)
                    {
                        diver.pause2--;

                        if (divers[i - 1].score > 0)
                        {
                            Drawclaps(800, 600, 400);
                        }
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
                        newDiver.pause2 = 200;
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


                // Check if diver just landed and sound hasn't been played
                if (diver.LANDED && !diver.soundPlayed)
                {
                    std::cout << "Diver landed, playing sound.\n";

                    player.PlayOneShot(landingSound);

                    if (diver.score > 0)
                    {
                        player.PlayOneShot(yeah); //only play sound in for loop, DO NOT read files here
                    }
                    else if (diver.score == 0)
                    {
                        player.PlayOneShot(boo);
                    }
                    diver.soundPlayed = true;
                    std::cout << "Remaining Divers: " << divers.size() << "\n";
                }


                // Handle spray effects
                if (diver.spray_state)
                {
                    diver.UpdateSpray();
                    //DrawWater();
                    diver.DrawSpray();
                    //DrawWater();

                    if (diver.spray_step >= 100)
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
                if (!diver.JUMPING && !diver.spray_state)
                {
                    diver.isActive = false;
                    // divers.erase(divers.begin() + i);
                    // std::cout << "Diver removed. Remaining Divers: " << divers.size() << "\n";
                    // Do not increment i since the current index is removed
                }
                ++i;
            }
            if (divers.size() >= maxDivers)
            {
                gameState = GameState::RESULTS;
            }
        }

        else if (gameState == GameState::RESULTS)
        {
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            // When the game is over, display the result
            float maxscore = divers[0].score;
            int k = 0;

            for (int i = 0; i < divers.size(); i++)
            {
                if (divers[i].score > maxscore)
                {
                    maxscore = divers[i].score;
                    k = i;
                }
            }

            std::string exitgameText = "Time remaining to exit :" + std::to_string(exitgame / 100 + 1)+ "s";

            if (maxscore != 0)
            {
                player.PlayBackground(victory);

                std::string scoreText = "Score: " + std::to_string(maxscore) + "/100";

                DrawTextOnScreen("winner!!!", 250, 250, divers[k].R_head, divers[k].G_head, divers[k].B_head);  // Adjust position
                DrawTextOnScreen(scoreText.c_str(), 180, 350, divers[k].R_body, divers[k].G_body, divers[k].B_body);
                DrawTextOnScreen(exitgameText.c_str(), 130, 500, divers[k].R_head, divers[k].G_head, divers[k].B_head);
                DrawTextOnScreen("Or press ESC to exit immediately", 90, 550, divers[k].R_body, divers[k].G_body, divers[k].B_body);

                divers[k].x = 470;
                divers[k].y = 230;
                divers[k].vx = 0;
                divers[k].vy = 0;
                divers[k].angle = 0;
                divers[k].angularVelocity = 0;
                divers[k].isActive = true;
                divers[k].Draw();
            }
            else if (maxscore == 0)
            {
                player.PlayBackground(fail);
                DrawTextOnScreen("No winner", 300, 300, 0, 0, 0);  // Adjust position
                DrawTextOnScreen(exitgameText.c_str(), 130, 450, divers[k].R_head, divers[k].G_head, divers[k].B_head);
                DrawTextOnScreen("Or press ESC to exit immediately", 90, 550, divers[k].R_body, divers[k].G_body, divers[k].B_body);
            }

            if (exitgame > 0)
            {
                exitgame--;
                //std::cout << exitgame;
            }
            else
            {
                break;
            }
        }
        FsSwapBuffers();
        FsSleep(10);
    }
    player.End();
    return 0;
}

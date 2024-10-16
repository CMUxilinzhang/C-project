#include "fssimplewindow.h"
#include <iostream>
#include <cmath>
#include <time.h>
#include <cstdlib>
#define PI 3.1415927
#define num_spray 400


// define diver's features
struct Diver {
    float x;
    float y;
    float vx;
    float vy;
    float angle;
    float angularVelocity;
} diver;
// time step(Fs) and g
const float dt = 0.010;
const float gravity = 130.0;
// height of window
const int windowHeight = 600;
// surface of the water
const float waterLevel = 480.0f;



void InitializeDiver()
{
    // initialize the position and velosity
    diver.x = 0.0f;
    diver.y = 50.0f;
    diver.vx = 0.0f;
    diver.vy = 0.0f;
    diver.angle = 0.0f;
    diver.angularVelocity = 0.0f;
}

void UpdateDiver(bool& LANDED, bool& START, bool& JUMPING)
{

    if (diver.x >= 150)
    {
        if (diver.x == 150)
        {
            diver.vy = -50.0f;
        }
        diver.vx = 40.0f;
        // update the position and velosity
        diver.vy += gravity * dt;
        diver.x += diver.vx * dt;
        diver.y += diver.vy * dt;
        diver.angle += diver.angularVelocity * dt;
    }
    if (diver.x < 150)
    {
        diver.x += diver.vx * dt;
    }

    // check landed
    if (diver.y > waterLevel)
    {
        LANDED = true;
        START = true;
        JUMPING = false;
    }
}

void DrawPlatform()
{
    // jumping board
    glColor3ub(139, 69, 19);
    glBegin(GL_QUADS);
    glVertex2i(0, 80);
    glVertex2i(150, 80);
    glVertex2i(150, 100);
    glVertex2i(0, 100);
    glEnd();

    // supporting column
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
    // alpha blending to implement the transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw the swimming pool
    glBegin(GL_QUADS);
    glColor4ub(0, 191, 255, 150); // transparency (150/255)
    glVertex2i(0, (int)waterLevel);
    glVertex2i(800, (int)waterLevel);
    glColor4ub(25, 25, 112, 150); // transparency (150/255)
    glVertex2i(800, windowHeight);
    glVertex2i(0, windowHeight);
    glEnd();

    // disable the blend
    glDisable(GL_BLEND);
}


void DrawDiver()
{
    // Set the diver.x and diver.y as the geometric center
    glPushMatrix();
    glTranslatef(diver.x, diver.y, 0.0f); // Set the geometric center
    glRotatef(diver.angle, 0.0f, 0.0f, 1.0f);

    // activate the alpha blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw the head
    glColor3f(0, 0, 1); // the color of the head
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, -10.0f); // the center of the head
    for (int i = 0; i <= 360; i += 30)
    {
        float rad = i * PI / 180.0f;
        glVertex2f(3.0f * cos(rad), 10.0f * sin(rad) - 10.0f); // draw the head
    }
    glEnd();

    // draw the body
    glColor4ub(0, 0, 0, 255); // black
    glBegin(GL_QUADS);
    glVertex2f(-4.0f, -5.0f);
    glVertex2f(4.0f, -5.0f);
    glVertex2f(4.0f, 12.0f);
    glVertex2f(-4.0f, 12.0f);
    glEnd();

    if (diver.y <= 250.0f && diver.x != 150.0f)
    {
        // draw the left arm
        glBegin(GL_LINES);
        glVertex2f(0.0f, -5.0f); 
        glVertex2f(-10.0f, 4.0f);
        glEnd();

        // right arm
        glBegin(GL_LINES);
        glVertex2f(0.0f, -5.0f);
        glVertex2f(10.0f, 4.0f);
        glEnd();

        // draw the left leg
        glBegin(GL_LINES);
        glVertex2f(0.0f, 10.0f);
        glVertex2f(-7.0f, 30.0f);
        glEnd();

        // right leg
        glBegin(GL_LINES);
        glVertex2f(0.0f, 10.0f);
        glVertex2f(7.0f, 30.0f);
        glEnd();
    }
    else if (diver.x == 150.0f)
    {
        // draw the left arm
        glBegin(GL_LINES);
        glVertex2f(0.0f, -5.0f);
        glVertex2f(-10.0f, 4.0f);
        glEnd();

        // right arm
        glBegin(GL_LINES);
        glVertex2f(0.0f, -5.0f);
        glVertex2f(10.0f, 4.0f);
        glEnd();

        // left leg(blend, ready to jump)
        glBegin(GL_LINES);
        glVertex2f(0.0f, 10.0f); 
        glVertex2f(-5.0f, 20.0f);
        glVertex2f(-5.0f, 20.0f);
        glVertex2f(-2.0f, 30.0f);
        glEnd();

        // right leg(blend, ready to jump)
        glBegin(GL_LINES);
        glVertex2f(0.0f, 10.0f);
        glVertex2f(5.0f, 20.0f);
        glVertex2f(5.0f, 20.0f);
        glVertex2f(2.0f, 30.0f);
        glEnd();
    }
    else
    {
        // left and right arm
        glBegin(GL_LINES);
        glVertex2f(-5.0f, -7.0f);
        glVertex2f(0.0f, -40.0f);
        glEnd();

        glBegin(GL_LINES);
        glVertex2f(5.0f, -7.0f); 
        glVertex2f(0.0f, -40.0f);
        glEnd();

        // left leg and right leg
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


void DrawDiverReflection()
{
    // shadow of diver
    glPushMatrix();
    glTranslatef(diver.x, diver.y, 0.0f);
    glRotatef(diver.angle, 0.0f, 0.0f, 1.0f);

    // alpha blend
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // diver body shadow
    glColor4ub(255, 224, 189, 150);
    glBegin(GL_QUADS);
    glVertex2f(-5.0f, 0.0f);
    glVertex2f(5.0f, 0.0f);
    glVertex2f(5.0f, 30.0f);
    glVertex2f(-5.0f, 30.0f);
    glEnd();

    // diver head shadow
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, -5.0f); // the top of the head
    for (int i = 0; i <= 360; i += 30)
    {
        float rad = i * PI / 180.0f;
        glVertex2f(5.0f * cos(rad), -5.0f * sin(rad) - 5.0f);
    }
    glEnd();

    glDisable(GL_BLEND);

    glPopMatrix();
}



int main(void)
{
    bool START, JUMPING, LANDED;  
    LANDED = false;
    START = true;
    JUMPING = false;

    FsOpenWindow(0, 0, 800, windowHeight, 1);

    InitializeDiver();

    int pause = 0;
    bool spray_state = false;
    float sx[num_spray], sy[num_spray], svx[num_spray], svy[num_spray];
    int spray_step = 0;
    
    while (true)
    {
        FsPollDevice();
        srand(time(nullptr));

        //check input
        if (0 != FsGetKeyState(FSKEY_ESC))
        {
            break;
        }

        if (true == START)
        {
            if (0 != FsGetKeyState(FSKEY_SPACE))
            {
                JUMPING = true;
                START = false;
                InitializeDiver();
                LANDED = false;
                diver.vx = 100.0f; // initialize the vx
                diver.vy = 0.0f;   // initialize the vy = 0
                diver.angularVelocity = 0.0f;
                pause = 50;
                spray_step = 0;
                diver.angle = 0;
            }
        }

        if (true == JUMPING)
        {
            // detect the input
            if (0 != FsGetKeyState(FSKEY_UP) || 0 != FsGetKeyState(FSKEY_LEFT))
            {
                diver.angularVelocity = -500.0f;
            }
            // detect the input
            else if (0 != FsGetKeyState(FSKEY_DOWN) || 0 != FsGetKeyState(FSKEY_RIGHT))
            {
                diver.angularVelocity = 500.0f;
            }
            else
            {
                // stop rotating if there is no input
                diver.angularVelocity = 0.0f;
            }
            // update the state

            // if pauseCounter > 0£¬stop updating
            if (diver.x == 150 && pause > 0)
            {
                pause--;
            }
            else
            {
                UpdateDiver(LANDED, START, JUMPING);
            }
        }
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            DrawWater();

            DrawPlatform();

            if (true == JUMPING && false == LANDED)
            {
                DrawDiver();            
                //std::cout << diver.x <<'\t' << diver.y << '\n';
            }

            if (LANDED == true)
            {
                spray_state = true;
                // detect the position of entering the water
                // horizontal entry
                if (fmod(fabs(diver.angle), 360.0) > 35 && fmod(fabs(diver.angle), 360.0) < 145)
                {
                    for (int i = 0; i < num_spray; ++i)
                    {
                        if (i < num_spray / 2)
                        {
                            sx[i] = rand() % 60 + diver.x - 30;
                            //sx[i] = diver.x;
                            sy[i] = diver.y;
                            svx[i] = (rand() % 20 - 10) / 11.0f;
                            svy[i] = (rand() % 200 - 200) / 111.0f;
                        }
                        else if (i >= 5 * num_spray / 6)
                        {
                            sx[i] = rand() % 55 + diver.x - 27;
                            //sx[i] = diver.x;
                            sy[i] = diver.y;
                            svx[i] = (rand() % 25 - 12) / 11.0f;
                            svy[i] = (rand() % 300 - 300) / 111.0f;
                        }
                        else
                        {
                            sx[i] = rand() % 50 + diver.x - 25;
                            //sx[i] = diver.x;
                            sy[i] = diver.y;
                            svx[i] = (rand() % 50 - 25) / 11.0f;
                            svy[i] = (rand() % 200 - 200) / 111.0f;
                        }
                    }
                }
                // verticle entry
                else
                {
                    for (int i = 0; i < 150; ++i)
                    {
                        if (i < 150 / 3)
                        {
                            sx[i] = rand() % 10 + diver.x - 5;
                            //sx[i] = diver.x;
                            sy[i] = diver.y;
                            svx[i] = (rand() % 20 - 10) / 11.0f;
                            svy[i] = (rand() % 160 - 160) / 111.0f;
                        }
                        else if (i >= 2 * 150 / 3)
                        {
                            sx[i] = rand() % 5 + diver.x - 2.5;
                            //sx[i] = diver.x;
                            sy[i] = diver.y;
                            svx[i] = (rand() % 10 - 5) / 11.0f;
                            svy[i] = (rand() % 200 - 200) / 111.0f;
                        }
                        else
                        {
                            sx[i] = rand() % 2 + diver.x - 1;
                            //sx[i] = diver.x;
                            sy[i] = diver.y;
                            svx[i] = (rand() % 10 - 5) / 11.0f;
                            svy[i] = (rand() % 240 - 240) / 111.0f;
                        }
                    }
                }
                LANDED = false;
            }

            if (spray_state)
            {
                for (int i = 0; i < num_spray; ++i)
                {
                    sx[i] += svx[i];
                    sy[i] += svy[i];
                    svy[i] += 3 * dt;
                }
                spray_state++;

                if (spray_step < 100)
                {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                    glColor4ub(60, 60, 112, 150);
                    glPointSize(2);
                    glBegin(GL_POINTS);
                    for (int i = 0; i < num_spray; ++i)
                    {
                        if (sy[i] <= 480.0)   // only draw sprays above the waterlevel
                        {
                            glVertex2i(sx[i], sy[i]);
                        }
                    }
                    glEnd();
                    glDisable(GL_BLEND);
                }
            }


            // draw diver and reflection
            glPushMatrix();
            glTranslatef(0.0f, 2 * waterLevel + 20, 0.0f);
            glScalef(1.0f, -1.0f, 1.0f);

            DrawPlatform();

            if (true == JUMPING)
            {
                DrawDiverReflection();
            }

            glPopMatrix();
            FsSwapBuffers();
            FsSleep(10);
    }
    return 0;
}



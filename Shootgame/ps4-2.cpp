#include "fssimplewindow.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#define PI 3.1415927
#define num_obs 5

void DrawCannonball(float cx, float cy, float r, int s, int ball_Color)
{
    switch(ball_Color)
    {
        case 0 :
            glColor3f(0, 0, 1);
            break;
        case 1 :
            glColor3f(0, 1, 1);
            break;
        case 2 :
            glColor3f(1, 1, 0);
            break;
        case 3 :
            glColor3f(1, 0, 1);
            break;
        case 4 :
            glColor3f(1, 0, 0);
            break;
    }
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= s; i++) {
        float a = 2.0 * 3.1415926 * float(i) / float(s);
        float x = r * cosf(a);
        float y = r * sinf(a);
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
}

void DrawObstacle(float x, float y, float w, float h, float Ob_hit_num)
{
    if(Ob_hit_num == 0)
    {
        glColor3f(0, 1, 0);
    }
    else if(Ob_hit_num == 1)
    {
        glColor3f(1, 1, 0);
    }
    glBegin(GL_QUADS);
    glVertex2i(x - w/2, y + h/2);
    glVertex2i(x + w/2, y + h/2);
    glVertex2i(x + w/2, y - h/2);
    glVertex2i(x - w/2, y - h/2);
    glEnd();
}

void DrawCannon(float angle)
{
    glColor3f(0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2i(50, 550);
    glVertex2i(60, 550);
    glVertex2i(60, 540);
    glVertex2i(50, 540);
    glEnd();

    float angle_pi = PI * angle / 180;
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    glVertex2i(55, 545);
    glVertex2f(55 + 25 * cos(angle_pi), 545 - 20 * sin(angle_pi));
    glEnd();
}

void DrawTarget(int x, int y)
{
    glColor3f(1, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x+100, y);
    glVertex2i(x+100, y-100);
    glVertex2i(x, y-100);
    glEnd();

}


int main(void)
{
    srand(time(nullptr));

    //parameters about cannon and cannonball
    bool ball_State = false;
    float angle = 0;
    int ball_Color = 0;
    float ball_v = 400 / 40;   
    /*The for loop runs 40 times per second(because of the 0.025 second delay).If the window display speed is 40m / s, that's 400 pixels per second, or 10 pixels per for loop.*/
    float ball_vx = 0, ball_vy = 0;
    float ball_x = 55, ball_y = 545;
    bool ball_g_action = false;


    //parameters about obstacles
    float obs_x[num_obs], obs_y[num_obs], obs_w[num_obs], obs_h[num_obs], Ob_hit_num[num_obs];
    for (int i = 0; i < num_obs; i++)
    {
        obs_x[i] = 40 + rand() % 720;
        obs_y[i] = 40 + rand() % 520;
        obs_w[i] = 80 + rand() % 70;
        obs_h[i] = 80 + rand() % 70;
        Ob_hit_num[i] = 0;
    }


    //parameters about target
    int t_x = 700, t_y = 100, t_vy = 100 / 40;
    bool target_state = true;


    FsOpenWindow(0, 0, 800, 600, 1);

    for ( ; ; )
    {
        FsPollDevice();
        auto key = FsInkey();
        if (FSKEY_ESC == key)
        {
            break;
        }

        switch (key)
        {
        case FSKEY_UP:
            if (angle < 90)
            {
                angle += 3;
            }
            break;
        case FSKEY_DOWN:
            if (angle > 0)
            {
                angle -= 3;
            }
            break;
        case FSKEY_SPACE:
            if (false == ball_State && ball_Color <= 4)     //这里是只让打五个球，删掉后半部分就能打很多了
            {
                ball_State = true;
                //parameters changes about cannonball
                ball_vx = ball_v * cos(PI * angle / 180);
                ball_vy = ball_v * sin(PI * angle / 180);
                ball_x = 55.0;
                ball_y = 545.0;
            }
            break;
        }

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        DrawCannon(angle);

        if (ball_State)
        {
            ball_x += ball_vx;
            ball_y = ball_y - ball_vy;
            ball_vy -= 0.0625;
            DrawCannonball(ball_x, ball_y, 5, 90, ball_Color);

            if (ball_x < 0 || ball_x > 800 || ball_y < 0 || ball_y > 600)
            {
                ball_State = false;
                ball_Color++;
            }

            for (int i = 0; i < num_obs; i++)
            {
                if (Ob_hit_num[i] == 0 || Ob_hit_num[i] == 1)
                {
                    if (ball_x > obs_x[i] - obs_w[i] / 2      
                 //这段把这个碰撞检测放在if (Ob_hit_num[i] == 0 || Ob_hit_num[i] == 1)这个条件下是因为，如果不放在这个下面，每次小球到这个obstacle的区域都会消失，因为本质上只是不draw了，但区域还在
                        && ball_x < obs_x[i] + obs_w[i] / 2
                        && ball_y < obs_y[i] + obs_h[i] / 2
                        && ball_y > obs_y[i] - obs_h[i] / 2)
                    {
                        ball_State = false;
                        Ob_hit_num[i]++;
                        ball_Color++;
                    }
                }
            }
        }

        for (int i = 0; i < num_obs; i++)
        {
            if (Ob_hit_num[i] == 0 || Ob_hit_num[i] == 1)
            {
                DrawObstacle(obs_x[i], obs_y[i], obs_w[i], obs_h[i], Ob_hit_num[i]);
            }
        }

        t_y += t_vy;
        if(t_y > 600 || t_y < 100)
        {
            t_vy = -t_vy;
        }

        if (target_state)
        {
            DrawTarget(t_x, t_y);

            if (ball_x > t_x
                && ball_x < t_x + 100
                && ball_y < t_y
                && ball_y > t_y - 100)
            {
                target_state = false;
                ball_State = false;
                ball_Color++;
            }
        }

        FsSwapBuffers(); // FsSwapBuffers() for double-buffered mode, glFlush() for single-buffered mode.

        FsSleep(25);
    }
}

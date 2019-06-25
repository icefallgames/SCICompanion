/***************************************************************************
    Copyright (c) 2019 Philip Fortier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
***************************************************************************/
#include "stdafx.h"
#include "Particles.h"
#include "View.h"
#include <random>


struct Particle
{
    float x;
    float y;
    float ax;
    float ay;
    float vx;
    float vy;
    byte color;

    int lifetime;
};

std::random_device g_rd;
std::mt19937 g_mt(g_rd());

bool IsParticleDead(const Particle &p) { return p.lifetime <= 0; }

void TempFrame(std::vector<Particle> &particles, const Cel &cel)
{
    for (int i = 0; i < 100; i++)
    {
        // Generate new particles each frame
        Particle p;
        {
            std::uniform_int_distribution<int32_t> distribution(0, cel.size.cx);
            p.x = distribution(g_mt);
        }
        {
            std::uniform_int_distribution<int32_t> distribution(0, cel.size.cy);
            p.y = distribution(g_mt);
        }

        p.vx = 0;
        p.vy = -5.0f * (float)(cel.size.cy - p.y) / (float)cel.size.cy;
        p.ax = 0;
        p.ay = -1;
        p.color = 0xff;
        p.lifetime = 7;

        particles.push_back(p);
    }
}

void ProcessParticle(Particle &p)
{
    p.vx += p.ax;
    p.vy += p.ay;
    p.x += p.vx;
    p.y += p.vy;
    p.lifetime--;
}

void DrawParticle(const Particle &p, Cel &cel)
{
    int x = max(0, min(cel.size.cx - 1, p.x));
    int y = max(0, min(cel.size.cy - 1, p.y));
    cel.Data[y * cel.GetStride() + x] = p.color;
}

void Simulate(std::vector<Cel> &cels)
{
    std::vector<Particle> particles;

    for (Cel &cel : cels)
    {
        TempFrame(particles, cel);

        for (Particle &p : particles)
        {
            DrawParticle(p, cel);
            ProcessParticle(p);
        }
        particles.erase(std::remove_if(particles.begin(), particles.end(), IsParticleDead), particles.end());
    }

    int index = 0;
    while (!particles.empty())
    {
        for (Particle &p : particles)
        {
            DrawParticle(p, cels[index % cels.size()]);
            ProcessParticle(p);
        }
        particles.erase(std::remove_if(particles.begin(), particles.end(), IsParticleDead), particles.end());

        index++;
    }
}

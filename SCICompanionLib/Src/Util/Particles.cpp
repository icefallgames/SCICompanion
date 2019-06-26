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
#include <random>
#include "chaiscript.hpp"


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

int CountPixelsOfColor(const Cel &cel, byte value)
{
    int count = 0;
    for (size_t i = 0; i < cel.Data.size(); i++)
    {
        if (cel.Data[i] == value)
        {
            count++;
        }
    }
    return count;
}

void GetXYOfColorPixelAtIndex(const Cel &cel, byte value, int index, float &x, float &y)
{
    int count = 0;
    for (size_t i = 0; i < cel.Data.size(); i++)
    {
        if (cel.Data[i] == value)
        {
            if (index == count)
            {
                x = i % cel.GetStride();
                y = i / cel.GetStride();
                return;
            }
            count++;
        }
    }
    x = 0;
    y = 0;
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

RasterComponent *g_raster;
Cel *g_currentCel;
Loop *g_currentLoop;
std::map<Cel*, std::vector<Particle>> g_particles;

void addParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, byte color)
{
    Particle p;
    p.x = point.x;
    p.y = point.y;
    p.ax = ax;
    p.ay = ay;
    p.vx = vx;
    p.vy = vy;
    p.color = color;
    p.lifetime = lifetime;
    g_particles[cel].push_back(p);
}

void SimulateParticlesForCel(Loop &loop, int celIndexStart)
{
    Cel *startCel = &loop.Cels[celIndexStart];
    if (g_particles.find(startCel) != g_particles.end())
    {
        std::vector<Particle> &particles = g_particles[startCel];

        int celIndex = celIndexStart;
        while (!particles.empty())
        {
            celIndex %= loop.Cels.size();
            for (Particle &p : particles)
            {
                DrawParticle(p, loop.Cels[celIndex]);
                ProcessParticle(p);
            }
            particles.erase(std::remove_if(particles.begin(), particles.end(), IsParticleDead), particles.end());

            celIndex++;
        }
    }
}

void simulateParticles()
{
    for (Loop &loop : g_raster->Loops)
    {
        int celIndex = 0;
        for (Cel &cel : loop.Cels)
        {
            SimulateParticlesForCel(loop, celIndex);

            celIndex++;
        }
    }
}

void drawPixel(Cel *cel, int x, int y, byte color)
{
    x = max(0, min(x, cel->size.cx - 1));
    y = max(0, min(y, cel->size.cy - 1));
    cel->Data[x + y * cel->GetStride()] = color;
}

Cel *getCel(int loop, int cel)
{
    return &g_raster->GetCel(CelIndex(loop, cel));
}

void RunChaiScript(const std::string &filename, RasterComponent &rasterIn, CelIndex selectedCel)
{
    // Give ourselves context.
    g_particles.clear();
    g_raster = &rasterIn;
    g_currentLoop = &rasterIn.Loops[selectedCel.loop];
    g_currentCel = &g_currentLoop->Cels[selectedCel.cel];

    chaiscript::ChaiScript chai;
    chai.add(chaiscript::var(g_currentCel), "cel");
    chai.add(chaiscript::var(g_currentLoop), "loop");
    chai.add(chaiscript::fun(&getCel), "getCel");
    chai.add(chaiscript::fun(&addParticle), "addParticle");
    chai.add(chaiscript::fun(&simulateParticles), "simulateParticles");
    chai.add(chaiscript::fun(&Cel::drawPixel), "drawPixel");
    chai.add(chaiscript::fun(&Cel::getRandomPoint), "getRandomPoint");
    chai.add(chaiscript::fun(&Cel::getRandomPointFromColor), "getRandomPointFromColor");
    chai.add(chaiscript::fun(&Loop::getCel), "getCel");
    chai.add(chaiscript::fun(&Loop::celCount), "celCount");

    try
    {
        chai.eval_file(filename);
    }
    catch (const chaiscript::exception::eval_error &e)
    {
        AfxMessageBox(e.pretty_print().c_str(), MB_OK | MB_ICONWARNING);
    }
}



void Cel::drawPixel(point16 point, byte color)
{
    int x = max(0, min(point.x, size.cx - 1));
    int y = max(0, min(point.y, size.cy - 1));
    Data[x + y * GetStride()] = color;
}
point16 Cel::getRandomPoint()
{
    point16 p;
    {
        std::uniform_int_distribution<int32_t> distribution(0, size.cx);
        p.x = distribution(g_mt);
    }
    {
        std::uniform_int_distribution<int32_t> distribution(0, size.cy);
        p.y = distribution(g_mt);
    }
    return p;
}
point16 Cel::getRandomPointFromColor(byte color)
{
    int whiteCount = CountPixelsOfColor(*this, 0xff);
    std::uniform_int_distribution<int32_t> distribution(0, whiteCount - 1);

    int index = distribution(g_mt);

    int count = 0;
    for (size_t i = 0; i < Data.size(); i++)
    {
        if (Data[i] == color)
        {
            if (index == count)
            {
                return point16((int16_t)(i % GetStride()), (int16_t)(i / GetStride()));
            }
            count++;
        }
    }
    return point16(0, 0);
}

int Loop::celCount()
{
    return (int)Cels.size();
}
Cel *Loop::getCel(int index)
{
    return &Cels[index];
}

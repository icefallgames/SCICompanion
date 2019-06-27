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
#define CHAISCRIPT_NO_THREADS 1
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

    bool line;

    float xPrev;
    float yPrev;
};

std::random_device g_rd;
std::mt19937 g_mt(g_rd());

bool IsParticleDead(const Particle &p) { return p.lifetime <= 0; }

void ProcessParticle(Particle &p)
{
    p.xPrev = p.x;
    p.yPrev = p.y;
    p.vx += p.ax;
    p.vy += p.ay;
    p.x += p.vx;
    p.y += p.vy;
    p.lifetime--;
}

void bresenham(Cel &cel, byte color, float x1, float y1, float x2, float y2)
{
    // Bresenham's line algorithm
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if (steep)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if (x1 > x2)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    for (int x = (int)x1; x < maxX; x++)
    {
        if (steep)
        {
            cel.Data[x * cel.GetStride() + y] = color;
        }
        else
        {
            cel.Data[y * cel.GetStride() + x] = color;
        }

        error -= dy;
        if (error < 0)
        {
            y += ystep;
            error += dx;
        }
    }
}

void DrawParticle(const Particle &p, Cel &cel)
{
    int x = max(0, min(cel.size.cx - 1, p.x));
    int y = max(0, min(cel.size.cy - 1, p.y));

    if (p.line)
    {
        int xPrev = max(0, min(cel.size.cx - 1, p.xPrev));
        int yPrev = max(0, min(cel.size.cy - 1, p.yPrev));
        bresenham(cel, p.color, xPrev, yPrev, x, y);
    }
    else
    {
        cel.Data[y * cel.GetStride() + x] = p.color;
    }
}

std::map<Cel*, std::vector<Particle>> g_particles;

void _addParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, byte color, bool isLine)
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
    p.line = isLine;
    p.xPrev = p.x;
    p.yPrev = p.y;
    g_particles[cel].push_back(p);
}

void addParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, byte color)
{
    _addParticle(cel, point, vx, vy, ax, ay, lifetime, color, false);
}

void addLineParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, byte color)
{
    _addParticle(cel, point, vx, vy, ax, ay, lifetime, color, true);
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
RasterComponent *g_raster;
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

std::unique_ptr<chaiscript::ChaiScript> g_chai;

int randomRange(int min, int maxExclusive)
{
    std::uniform_int_distribution<int32_t> distribution(min, maxExclusive - 1);
    return distribution(g_mt);
}
float randomRangeF(float min, float max)
{
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(g_mt);
}

const double DegreesToReadians = 0.0174533;

double sinDegrees(double angle)
{
    return sin(angle * DegreesToReadians);
}
double cosDegrees(double angle)
{
    return cos(angle * DegreesToReadians);
}

void RunChaiScript(const std::string &script, RasterComponent &rasterIn, CelIndex selectedCel)
{
    // Give ourselves context.
    g_particles.clear();

    if (!g_chai)
    {
        g_chai = std::make_unique<chaiscript::ChaiScript>();

        g_chai->add(chaiscript::fun(&randomRange), "rand");
        g_chai->add(chaiscript::fun(&randomRangeF), "randf");

        g_chai->add(chaiscript::fun(&addParticle), "addParticle");
        g_chai->add(chaiscript::fun(&addLineParticle), "addLineParticle");
        g_chai->add(chaiscript::fun(&simulateParticles), "simulateParticles");

        g_chai->add(chaiscript::fun(&point16::x), "x");
        g_chai->add(chaiscript::fun(&point16::y), "y");
        g_chai->add(chaiscript::fun(&Pixel::color), "color");
        g_chai->add(chaiscript::fun(&Pixel::pos), "pos");

        g_chai->add(chaiscript::fun(&sinDegrees), "sin");
        g_chai->add(chaiscript::fun(&cosDegrees), "cos");

        g_chai->add(chaiscript::user_type<Pixel>(), "Pixel");
        g_chai->add(chaiscript::bootstrap::standard_library::vector_type<std::vector<Pixel> >("PixelVector"));

        g_chai->add(chaiscript::fun(&Cel::getPixel), "getPixel");
        g_chai->add(chaiscript::fun(&Cel::getWidth), "getWidth");
        g_chai->add(chaiscript::fun(&Cel::getHeight), "getHeight");
        g_chai->add(chaiscript::fun(&Cel::getTransparent), "getTransparent");
        g_chai->add(chaiscript::fun(&Cel::drawPixel), "drawPixel");
        g_chai->add(chaiscript::fun(&Cel::getRandomPixel), "getRandomPixel");
        g_chai->add(chaiscript::fun(&Cel::selectPixelsOfColor), "selectPixelsOfColor");
        g_chai->add(chaiscript::fun(&Cel::selectPixelsNotOfColor), "selectPixelsNotOfColor");
        g_chai->add(chaiscript::fun(&Cel::clear), "clear");
        g_chai->add(chaiscript::fun(&Loop::getCel), "getCel");
        g_chai->add(chaiscript::fun(&Loop::celCount), "celCount");
        g_chai->add(chaiscript::fun(&Loop::clear), "clear");
        g_chai->add(chaiscript::fun(&RasterComponent::getCel), "getCel");

    }

    const auto base_state = g_chai->get_state();
    const auto base_locals = g_chai->get_locals();

    Loop *g_currentLoop = &rasterIn.Loops[selectedCel.loop];
    Cel *g_currentCel = &g_currentLoop->Cels[selectedCel.cel];
    g_raster = &rasterIn;
    g_chai->add(chaiscript::var(g_currentCel), "cel");
    g_chai->add(chaiscript::var(g_currentLoop), "loop");
    g_chai->add(chaiscript::var(g_raster), "view");

    try
    {
        g_chai->eval(script);
    }
    catch (const chaiscript::exception::eval_error &e)
    {
        AfxMessageBox(e.pretty_print().c_str(), MB_OK | MB_ICONWARNING);
    }

    // So weird we need to do this.
    g_chai->set_state(base_state);
    g_chai->set_locals(base_locals);
}

int Cel::getWidth()
{
    return size.cx;
}
int Cel::getHeight()
{
    return size.cy;
}
byte Cel::getTransparent()
{
    return TransparentColor;
}
Pixel Cel::getPixel(point16 point)
{
    int x = max(0, min(point.x, size.cx - 1));
    int y = max(0, min(point.y, size.cy - 1));
    return Pixel{ point, Data[x + y * GetStride()] };
}

void Cel::drawPixel(point16 point, byte color)
{
    int x = max(0, min(point.x, size.cx - 1));
    int y = max(0, min(point.y, size.cy - 1));
    Data[x + y * GetStride()] = color;
}
Pixel Cel::getRandomPixel()
{
    point16 p;
    {
        std::uniform_int_distribution<int32_t> distribution(0, size.cx - 1);
        p.x = distribution(g_mt);
    }
    {
        std::uniform_int_distribution<int32_t> distribution(0, size.cy - 1);
        p.y = distribution(g_mt);
    }

    return Pixel { p,Data[p.x + p.y * GetStride()] };
}

std::vector<Pixel> Cel::selectPixelsOfColor(byte color)
{
    std::vector<Pixel> pixels;
    for (size_t i = 0; i < Data.size(); i++)
    {
        if (Data[i] == color)
        {
            int16_t x = (int16_t)(i % GetStride());
            int16_t y = (int16_t)(i / GetStride());
            pixels.push_back(Pixel{ point16(x, y), color });
        }
    }
    return pixels;
}
std::vector<Pixel> Cel::selectPixelsNotOfColor(byte color)
{
    std::vector<Pixel> pixels;
    for (size_t i = 0; i < Data.size(); i++)
    {
        if (Data[i] != color)
        {
            int16_t x = (int16_t)(i % GetStride());
            int16_t y = (int16_t)(i / GetStride());
            pixels.push_back(Pixel{ point16(x, y), Data[i] });
        }
    }
    return pixels;
}

void Cel::clear()
{
    for (size_t i = 0; i < Data.size(); i++)
    {
        Data[i] = this->TransparentColor;
    }
}

int Loop::celCount()
{
    return (int)Cels.size();
}
Cel *Loop::getCel(int index)
{
    return &Cels[index];
}
void Loop::clear()
{
    for (Cel &cel : Cels)
    {
        cel.clear();
    }
}

Cel *RasterComponent::getCel(int loop, int cel)
{
    return &GetCel(CelIndex(loop, cel));
}
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
//#include <algorithm>


struct Particle
{
    Particle() = default;
    Particle(const Particle &p) = delete;
    Particle(Particle &&p) = default;
    Particle& operator=(Particle&&) = default;

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

    std::unique_ptr<Cel> mask;
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
    if (p.mask)
    {
        Cel &celBrush = *p.mask;
        for (int yBase = 0; yBase < celBrush.size.cy; yBase++)
        {
            for (int xBase = 0; xBase < celBrush.size.cx; xBase++)
            {
                int index = xBase + yBase * celBrush.GetStride();
                if (celBrush.Data[index] != celBrush.TransparentColor)
                {
                    int xTarget = p.x + xBase + celBrush.placement.x - celBrush.size.cx / 2 ; // REVIEW: may not be right
                    int yTarget = p.y + yBase + celBrush.placement.y - celBrush.size.cy + 1;
                    if (xTarget >= 0 && yTarget >= 0 && xTarget < cel.size.cx && yTarget < cel.size.cy)
                    {
                        cel.Data[yTarget * cel.GetStride() + xTarget] = celBrush.Data[index];
                    }
                }
            }
        }
    }
    else
    {
        if (p.x >= 0 && p.x < cel.size.cx && p.y >= 0 && p.y < cel.size.cy) // I changed this from accumulate on edge to discard... might want it to be an option instead?
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
    }
}

std::map<Cel*, std::vector<Particle>> g_particles;

void _addParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, byte color, bool isLine, Cel *brushCelTemplate, Cel *brushSourceCel)
{
    g_particles[cel].emplace_back();
    Particle &p = g_particles[cel].back();
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
    if (brushCelTemplate)
    {
        p.mask = std::make_unique<Cel>(*brushCelTemplate);
        // Fill it in with actual pixel values!
        Cel &celBrush = *p.mask;
        for (int yBase = 0; yBase < celBrush.size.cy; yBase++)
        {
            for (int xBase = 0; xBase < celBrush.size.cx; xBase++)
            {
                int index = xBase + yBase * celBrush.GetStride();
                if (celBrush.Data[index] != celBrush.TransparentColor)
                {
                    int xTarget = p.x + xBase + celBrush.placement.x - celBrush.size.cx / 2; // REVIEW: may not be right
                    int yTarget = p.y + yBase + celBrush.placement.y - celBrush.size.cy + 1;
                    byte brushColor = celBrush.TransparentColor;
                    if (xTarget >= 0 && yTarget >= 0 && xTarget < brushSourceCel->size.cx && yTarget < brushSourceCel->size.cy)
                    {
                        brushColor = brushSourceCel->Data[xTarget + yTarget * brushSourceCel->GetStride()];
                    }
                    celBrush.Data[index] = brushColor;
                }
            }
        }
    }
}

void addParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, byte color)
{
    _addParticle(cel, point, vx, vy, ax, ay, lifetime, color, false, nullptr, nullptr);
}

void addBrushParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, Cel *brushCel, Cel *sourceCel)
{
    _addParticle(cel, point, vx, vy, ax, ay, lifetime, 0, false, brushCel, sourceCel);
}

void addLineParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, byte color)
{
    _addParticle(cel, point, vx, vy, ax, ay, lifetime, color, true, nullptr, nullptr);
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
            auto it = std::remove_if(particles.begin(), particles.end(), IsParticleDead);
            particles.erase(it, particles.end());

            celIndex++;
        }
    }
}

void SimulateParticlesForCelOneAtATime(Loop &loop, int celIndexStart)
{
    Cel *startCel = &loop.Cels[celIndexStart];
    if (g_particles.find(startCel) != g_particles.end())
    {
        std::vector<Particle> &particles = g_particles[startCel];

        for (Particle &p : particles)
        {
            int celIndex = celIndexStart;
            while (!IsParticleDead(p))
            {
                celIndex %= loop.Cels.size();
                DrawParticle(p, loop.Cels[celIndex]);
                ProcessParticle(p);
                celIndex++;
            }
        }
        particles.clear();
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

point16 makePoint(int x, int y)
{
    return point16(x, y);
}

void randomizePixelList(std::vector<Pixel> &inList)
{
    std::random_shuffle(inList.begin(), inList.end());
}

int lerpPower(double power, double from, double to, double start, double end, double current)
{
    float progress = (current - start) / (end - start); // Ostensibly between 0 and 1
    float value = pow(progress, power);
    value = from + (to - from) * value;
    return (int)round(value);
}


double lerpf(double from, double to, double start, double end, double current)
{
    return lerpPower(1.0, from, to, start, end, current);
}

double lerpPowerf(double power, double from, double to, double start, double end, double current)
{
    float progress = (current - start) / (end - start); // Ostensibly between 0 and 1
    float value = pow(progress, power);
    value = from + (to - from) * value;
    return value;
}

double signHelper(double value)
{
    return signbit(value) ? -1.0 : 1.0;
}
double absHelper(double value)
{
    return abs(value);
}

int lerp(double from, double to, double start, double end, double current)
{
    return lerpPower(1.0, from, to, start, end, current);
}


void simulateParticlesOneAtATime()
{
    for (Loop &loop : g_raster->Loops)
    {
        int celIndex = 0;
        for (Cel &cel : loop.Cels)
        {
            SimulateParticlesForCelOneAtATime(loop, celIndex);
            celIndex++;
        }
    }
}

void SimulateOneParticle(Loop &loop, int celIndexStart, Particle &p)
{
    int celIndex = celIndexStart;
    while (!IsParticleDead(p))
    {
        celIndex %= loop.Cels.size();
        DrawParticle(p, loop.Cels[celIndex]);
        ProcessParticle(p);
        celIndex++;
    }
}

void simulateParticlesRoundRobin()
{
    for (Loop &loop : g_raster->Loops)
    {
        bool hadSome = true;
        while (hadSome)
        {
            hadSome = false;
            int celIndex = 0;
            for (int celIndex = 0; celIndex < loop.celCount(); celIndex++)
            {
                std::vector<Particle> &particles = g_particles[&loop.Cels[celIndex]];
                if (!particles.empty())
                {
                    SimulateOneParticle(loop, celIndex, particles.back());
                    particles.pop_back();
                    hadSome = true;
                }
            }
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

int randomRange(int min, int max)
{
    std::uniform_int_distribution<int32_t> distribution(min, max - 1); // -1 since I want it to be exclusive.
    return distribution(g_mt);
}
float randomRangeF(float min, float max)
{
    std::uniform_real_distribution<float> distribution(min, max);
    return distribution(g_mt);
}

void seedRandom(int seed)
{
    g_mt.seed(seed);
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
double atanDegrees(double y, double x)
{
    return atan2(y, x) / DegreesToReadians;
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
        g_chai->add(chaiscript::fun(&seedRandom), "randseed");

        g_chai->add(chaiscript::fun(&addParticle), "addParticle");
        g_chai->add(chaiscript::fun(&addLineParticle), "addLineParticle");
        g_chai->add(chaiscript::fun(&addBrushParticle), "addBrushParticle");
        g_chai->add(chaiscript::fun(&simulateParticles), "simulateParticles");
        g_chai->add(chaiscript::fun(&simulateParticlesOneAtATime), "simulateParticlesOneAtATime");
        g_chai->add(chaiscript::fun(&simulateParticlesRoundRobin), "simulateParticlesRoundRobin");
        
        g_chai->add(chaiscript::fun(&lerp), "lerp");
        g_chai->add(chaiscript::fun(&lerpf), "lerpf");
        g_chai->add(chaiscript::fun(&lerpPower), "lerpPower");
        g_chai->add(chaiscript::fun(&lerpPowerf), "lerpPowerf");
        g_chai->add(chaiscript::fun(&signHelper), "sign");
        g_chai->add(chaiscript::fun(&absHelper), "abs");

        g_chai->add(chaiscript::fun(&point16::x), "x");
        g_chai->add(chaiscript::fun(&point16::y), "y");
        g_chai->add(chaiscript::fun(&Pixel::color), "color");
        g_chai->add(chaiscript::fun(&Pixel::pos), "pos");

        g_chai->add(chaiscript::fun(&sinDegrees), "sin");
        g_chai->add(chaiscript::fun(&cosDegrees), "cos");
        g_chai->add(chaiscript::fun(&atanDegrees), "atan");

        g_chai->add(chaiscript::user_type<point16>(), "point16");
        g_chai->add(chaiscript::fun(&makePoint), "makePoint");

        g_chai->add(chaiscript::user_type<Pixel>(), "Pixel");
        g_chai->add(chaiscript::bootstrap::standard_library::vector_type<std::vector<Pixel> >("PixelVector"));

        g_chai->add(chaiscript::fun(&randomizePixelList), "randomizePixelList");
        g_chai->add(chaiscript::fun(&Cel::getPixel), "getPixel");
        g_chai->add(chaiscript::fun(&Cel::getWidth), "getWidth");
        g_chai->add(chaiscript::fun(&Cel::getHeight), "getHeight");
        g_chai->add(chaiscript::fun(&Cel::getTransparent), "getTransparent");
        g_chai->add(chaiscript::fun(&Cel::drawPixel), "drawPixel");
        g_chai->add(chaiscript::fun(&Cel::drawLine), "drawLine");
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
    catch (const chaiscript::exception::dispatch_error &e)
    {
        AfxMessageBox(e.what(), MB_OK | MB_ICONWARNING);
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

// clamps
void Cel::drawPixel(point16 point, byte color)
{
    int x = max(0, min(point.x, size.cx - 1));
    int y = max(0, min(point.y, size.cy - 1));
    Data[x + y * GetStride()] = color;
}
void Cel::discardDrawPixel(point16 point, byte color)
{
    if (point.x >= 0 && point.x < size.cx && point.y >= 0 && point.y < size.cy)
    {
        Data[point.x + point.y * GetStride()] = color;
    }
}

//#define LINEMACRO(pData, startx, starty, deltalinear, deltanonlinear, linearvar, nonlinearvar, \
  //                linearend, nonlinearstart, linearmod, nonlinearmod) \


void LineMacro(Cel &cel, byte color, int& x, int& y, int deltalinear, int deltanonlinear, int& linearvar, int& nonlinearvar, int linearend, int nonlinearstart, int linearmod, int nonlinearmod)
{
    int incrNE = ((deltalinear) > 0) ? (deltalinear) : -(deltalinear);
    incrNE <<= 1;
    deltanonlinear <<= 1;
    int incrE = ((deltanonlinear) > 0) ? -(deltanonlinear) : (deltanonlinear);
    int d = nonlinearstart - 1;
    while (linearvar != (linearend))
    {
        cel.discardDrawPixel(point16(x, y), color);
        linearvar += linearmod;
        if ((d += incrE) < 0) {
                d += incrNE;
                nonlinearvar += nonlinearmod;
        };
    };
    cel.discardDrawPixel(point16(x, y), color);
}

void Cel::drawLine(point16 start, point16 end, byte color, int percentEnd)
{
    int xStart = start.x;
    int yStart = start.y;
    int xEnd = end.x;
    int yEnd = end.y;
    //DrawCaptureToolHelper(nullptr, *this, start, end, 1, color);
    // Copied from pic commands so we can use a bresenham algorithm, and control how much of a line we draw.
    int dx, dy, finalx, finaly;
    int x = (int)xStart;
    int y = (int)yStart;
    dx = xEnd - x;
    dy = yEnd - y;

    int finalxFull = xStart + dx;
    int finalyFull = yStart + dy;

    finalx = xStart + dx * percentEnd / 100;
    finaly = yStart + dy * percentEnd / 100;

    dx = abs(dx);
    dy = abs(dy);

    if (dx > dy) {
        if (finalxFull < x) {
            if (finalyFull < y) { /* llu == left-left-up */
                LineMacro(*this, color, x, y, dx, dy, x, y, finalx, dx, -1, -1);
            }
            else {         /* lld */
                LineMacro(*this, color, x, y, dx, dy, x, y, finalx, dx, -1, 1);
            }
        }
        else { /* x1 >= x */
            if (finalyFull < y) { /* rru */
                LineMacro(*this, color, x, y, dx, dy, x, y, finalx, dx, 1, -1);
            }
            else {         /* rrd */
                LineMacro(*this, color, x, y, dx, dy, x, y, finalx, dx, 1, 1);
            }
        }
    }
    else { /* dx <= dy */
        if (finalyFull < y) {
            if (finalxFull < x) { /* luu */
                LineMacro(*this, color, x, y, dy, dx, y, x, finaly, dy, -1, -1);
            }
            else {         /* ruu */
                LineMacro(*this, color, x, y, dy, dx, y, x, finaly, dy, -1, 1);
            }
        }
        else { /* y1 >= y */
            if (finalxFull < x) { /* ldd */
                LineMacro(*this, color, x, y, dy, dx, y, x, finaly, dy, 1, -1);
            }
            else {         /* rdd */
                LineMacro(*this, color, x, y, dy, dx, y, x, finaly, dy, 1, 1);
            }
        }
    }
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
    for (int y = 0; y < size.cy; y++)
    {
        for (int x = 0; x < size.cx; x++)
        {
            if (Data[y * GetStride() + x] == color)
            {
                pixels.push_back(Pixel{ point16(x, y), color });
            }
        }
    }
    return pixels;
}
std::vector<Pixel> Cel::selectPixelsNotOfColor(byte badColor)
{
    std::vector<Pixel> pixels;

    for (int y = 0; y < size.cy; y++)
    {
        for (int x = 0; x < size.cx; x++)
        {
            byte color = Data[y * GetStride() + x];
            if (badColor != color)
            {
                pixels.push_back(Pixel{ point16(x, y), color });
            }
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



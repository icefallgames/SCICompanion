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
#include "DrawHelper.h"

struct ColorOverLife
{
    std::map<double, byte> ageToColor;
};

struct SizeOverLife
{
    std::map<double, int> ageToSize;
};


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
    int currentAge; // Retired when this equals lifetime

    bool line;

    float xPrev;
    float yPrev;

    std::unique_ptr<Cel> mask;
    ColorOverLife *colorOverLife;
    SizeOverLife *sizeOverLife;
};

struct Drop
{
    byte color;
};

struct DropSim
{
    void set(int x, int y, Drop drop)
    {
        drops[x + width * y] = drop;
    }
    void set(int index, Drop drop)
    {
        drops[index] = drop;
    }
    Drop get(int x, int y)
    {
        return drops[x + width * y];
    }
    void move(int xFrom, int yFrom, int xTo, int yTo)
    {
        //std::ostringstream ss;
        //ss << "Move: " << xFrom << "," << yFrom << " to " << xTo << "," << yTo;
        //OutputDebugString(ss.str().c_str());

        Drop drop = get(xFrom, yFrom);
        set(xFrom, yFrom, { transparent });
        set(xTo, yTo, drop);

        processedThisFrame[xTo + yTo * width] = true;
        processedThisFrame[xFrom + yFrom * width] = false;
    }
    bool isFilled(int index)
    {
        return drops[index].color != transparent;
    }
    bool isFilled(int x, int y)
    {
        if (x >= 0 && y >= 0 && x < width && y < height)
        {
            return get(x, y).color != transparent;
        }
        return true; // edges are considered filled... boundaries.
    }

    void clearFrame()
    {
        std::fill(processedThisFrame.begin(), processedThisFrame.end(), false);
    }
    bool hasProcessedThisFrame(int x, int y)
    {
        return processedThisFrame[x + width * y];
    }

    int width;
    int height;
    byte transparent;
    std::vector<Drop> drops;
    std::vector<int> randomIndices;
    std::vector<int> randomRowIndices;
    int abandonPerFrame;

    std::vector<bool> processedThisFrame;
};

DropSim g_dropSimulation;

std::random_device g_rd;
std::mt19937 g_mt(g_rd());

bool IsParticleDead(const Particle &p) { return p.currentAge >= p.lifetime; }

void ProcessParticle(Particle &p)
{
    p.xPrev = p.x;
    p.yPrev = p.y;
    p.vx += p.ax;
    p.vy += p.ay;
    p.x += p.vx;
    p.y += p.vy;
    p.currentAge++;
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
        byte color = p.color;
        ColorOverLife *colorOverLife = p.colorOverLife;
        if (colorOverLife && !colorOverLife->ageToColor.empty())
        {
            double age = (double)p.currentAge / (double)p.lifetime;
            color = colorOverLife->ageToColor[0];
            for (auto pair : colorOverLife->ageToColor)
            {
                if (age <= pair.first) // No interpolation for us
                {
                    break;
                }
                color = pair.second;
            }
        }

        int x = (int)roundf(p.x);
        int y = (int)roundf(p.y);
        if (x >= 0 && x < cel.size.cx && y >= 0 && y < cel.size.cy) // I changed this from accumulate on edge to discard... might want it to be an option instead?
        {
            x = max(0, min(cel.size.cx - 1, x));
            y = max(0, min(cel.size.cy - 1, y));

            if (p.line)
            {
                int xPrev = (int)roundf(p.xPrev);
                int yPrev = (int)roundf(p.yPrev);
                xPrev = max(0, min(cel.size.cx - 1, xPrev));
                yPrev = max(0, min(cel.size.cy - 1, yPrev));
                bresenham(cel, color, xPrev, yPrev, x, y);
            }
            else
            {
                cel.Data[y * cel.GetStride() + x] = color;
            }
        }
    }
}

std::map<Cel*, std::vector<Particle>> g_particles;
std::vector<ColorOverLife> g_colorOverLife;
std::vector<SizeOverLife> g_sizeOverLife;

//ColorOverLife *declareColorOverLife(std::map<double, byte> colorOverLife)
//ColorOverLife *declareColorOverLife(std::vector<chaiscript::Boxed_Value> *colorOverLifeBase)
ColorOverLife *declareColorOverLife(double life0, byte color0, double life1, byte color1, double life2, byte color2, double life3, byte color3)
{
    std::map<double, byte> colorOverLife;
    colorOverLife[life0] = color0;
    colorOverLife[life1] = color1;
    colorOverLife[life2] = color2;
    colorOverLife[life3] = color3;

    g_colorOverLife.push_back({ colorOverLife });
    return &g_colorOverLife.back();
}
SizeOverLife *declareSizeOverLife(std::map<double, int> sizeOverLife)
{
    g_sizeOverLife.push_back({ sizeOverLife });
    return &g_sizeOverLife.back();
}

void _addParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, byte color, bool isLine, Cel *brushCelTemplate, Cel *brushSourceCel, ColorOverLife *colorOverLife, SizeOverLife *sizeOverLife)
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
    p.currentAge = 0;
    p.line = isLine;
    p.xPrev = p.x;
    p.yPrev = p.y;
    p.colorOverLife = colorOverLife;
    p.sizeOverLife = sizeOverLife;

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
    _addParticle(cel, point, vx, vy, ax, ay, lifetime, color, false, nullptr, nullptr, nullptr, nullptr);
}

void addParticleColor(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, ColorOverLife *colorOverLife)
{
    _addParticle(cel, point, vx, vy, ax, ay, lifetime, 0x00, false, nullptr, nullptr, colorOverLife, nullptr);
}



void addBrushParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, Cel *brushCel, Cel *sourceCel)
{
    _addParticle(cel, point, vx, vy, ax, ay, lifetime, 0, false, brushCel, sourceCel, nullptr, nullptr);
}

void addLineParticle(Cel *cel, point16 point, float vx, float vy, float ax, float ay, int lifetime, byte color)
{
    _addParticle(cel, point, vx, vy, ax, ay, lifetime, color, true, nullptr, nullptr, nullptr, nullptr);
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


int randomRange(int min, int max)
{
    std::uniform_int_distribution<int32_t> distribution(min, max - 1); // -1 since I want it to be exclusive.
    return distribution(g_mt);
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


void dropSimulation(int width, int height, byte transparent, int abandonPerFrame)
{
    g_dropSimulation.abandonPerFrame = abandonPerFrame;
    g_dropSimulation.width = width;
    g_dropSimulation.height = height;
    g_dropSimulation.transparent = transparent;
    g_dropSimulation.drops.clear();
    g_dropSimulation.drops.resize(width * height);
    g_dropSimulation.processedThisFrame.clear();
    g_dropSimulation.processedThisFrame.resize(width * height);
    g_dropSimulation.randomIndices.clear();
    g_dropSimulation.randomIndices.resize(width * height);
    g_dropSimulation.randomRowIndices.clear();
    g_dropSimulation.randomRowIndices.resize(width);

    for (int i = 0; i < width * height; i++)
    {
        g_dropSimulation.drops[i].color = transparent;
        g_dropSimulation.randomIndices[i] = i;
    }
    std::random_shuffle(g_dropSimulation.randomIndices.begin(), g_dropSimulation.randomIndices.end());
    bool incr = true;
    int boop = 0;
    int center = width / 2;
    for (int i = 0; i < width; i++)
    {
        //g_dropSimulation.randomRowIndices[i] = i;
        // try inward out
        g_dropSimulation.randomRowIndices[i] = center + boop;
        if (incr)
        {
            boop++;
        }
        boop = -boop;
        incr = !incr;
    }
    std::random_shuffle(g_dropSimulation.randomRowIndices.begin(), g_dropSimulation.randomRowIndices.end());
}

void addDrop(int x, int y, byte color)
{
    if (x >= 0 && y >= 0 && x < g_dropSimulation.width && y < g_dropSimulation.height)
    {
        Drop drop = { color };
        g_dropSimulation.drops[g_dropSimulation.width * y + x] = drop;
    }
}

void BlitDropsToCel(Cel &cel)
{
    for (int y = 0; y < g_dropSimulation.height; y++)
    {
        for (int x = 0; x < g_dropSimulation.width; x++)
        {
            cel.drawPixel(point16(x, y), g_dropSimulation.get(x, y).color);
        }
    }
}

void simulateDrops(Loop *loop)
{
    int maxIterations = loop->celCount();

    bool moveMade = true;
    for (int i = 0; moveMade && i < maxIterations; i++)
    {
        // Paint to cel
        BlitDropsToCel(loop->Cels[i]);

        g_dropSimulation.clearFrame();

        moveMade = false;

        int isFirstIteration = true;
        int attemptsRemainingThisFrame = 100;
        bool keepGoing = true;
        bool didAbandonPass = false;
        while (keepGoing && (attemptsRemainingThisFrame > 0))
        {
            attemptsRemainingThisFrame--;
            keepGoing = false;

            bool moveMadeThisFrameIteration = false;

            // Now simulate, but in a random order.
            int dropCount = g_dropSimulation.width * g_dropSimulation.height;
            int randomStartIndex = randomRange(0, dropCount);


            // First though, abandon some
            if (!didAbandonPass)
            {
                int abandonCount = g_dropSimulation.abandonPerFrame;
                for (int i = 0; abandonCount && i < dropCount; i++)
                {
                    int index = (i + randomStartIndex) % dropCount;
                    index = g_dropSimulation.randomIndices[index];
                    if (g_dropSimulation.isFilled(index))
                    {
                        g_dropSimulation.set(index, Drop{ g_dropSimulation.transparent });
                        abandonCount--;
                        moveMadeThisFrameIteration = true;
                    }
                }
                didAbandonPass = true;
            }

            int randomRowIndexStart = 0;
            for (int i = 0; i < dropCount; i++)
            {
                //int y = (g_dropSimulation.height - i / g_dropSimulation.width - 1);
                int y = i / g_dropSimulation.width;
                int x = i % g_dropSimulation.width;

                if (x == 0)
                {
                    randomRowIndexStart = 0;
                    //randomRowIndexStart = randomRange(0, g_dropSimulation.width);
                }
                x = g_dropSimulation.randomRowIndices[(randomRowIndexStart + x) % g_dropSimulation.width];

                /*
                int index = (i + randomStartIndex) % dropCount;
                index = g_dropSimulation.randomIndices[index];
                int y = index / g_dropSimulation.width;
                int x = index % g_dropSimulation.width;
                */

                if (!g_dropSimulation.hasProcessedThisFrame(x, y) && g_dropSimulation.isFilled(x, y))
                {
                    // If below is empty, move there.
                    if (!g_dropSimulation.isFilled(x, y + 1))
                    {
                        g_dropSimulation.move(x, y, x, y + 1);
                        moveMadeThisFrameIteration = true;
                    }
                    else
                    {
                        // Otherwise if someone is above me, maybe move to the side. But only if this is the first iteration
                        if (isFirstIteration)
                        {
                            if (g_dropSimulation.isFilled(x, y - 1))
                            {
                                bool leftFilled = g_dropSimulation.isFilled(x - 1, y);
                                bool rightFilled = g_dropSimulation.isFilled(x + 1, y);
                                if (leftFilled && !rightFilled)
                                {
                                    g_dropSimulation.move(x, y, x + 1, y);
                                    moveMadeThisFrameIteration = true;
                                }
                                else if (!leftFilled && rightFilled)
                                {
                                    g_dropSimulation.move(x, y, x - 1, y);
                                    moveMadeThisFrameIteration = true;
                                }
                            }
                        }
                    }
                }
            }

            if (moveMadeThisFrameIteration)
            {
                keepGoing = true;
                moveMade = true;
            }

            isFirstIteration = false;
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

point16 makePoint(int x, int y)
{
    return point16(x, y);
}


point16 makePointF(double x, double y)
{
    return point16((int16_t)round(x), (int16_t)round(y));
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

double length(double x, double y)
{
    return sqrt(x * x + y * y);
}

void console(const std::string &s, double value)
{
    std::ostringstream ss;
    ss << s << value << "\n";
    OutputDebugString(ss.str().c_str());
}

void RunChaiScript(const std::string &script, RasterComponent &rasterIn, CelIndex selectedCel)
{
    // Give ourselves context.
    g_particles.clear();
    g_colorOverLife.clear();
    g_colorOverLife.reserve(10);    // Doing this allows us up to 10 before resizing, which would invalidate pointers. Dunno if chai has a better way to do things.
    g_sizeOverLife.clear();
    g_sizeOverLife.reserve(10);

    if (!g_chai)
    {
        g_chai = std::make_unique<chaiscript::ChaiScript>();

        g_chai->add(chaiscript::fun(&randomRange), "rand");
        g_chai->add(chaiscript::fun(&randomRangeF), "randf");
        g_chai->add(chaiscript::fun(&seedRandom), "randseed");

        g_chai->add(chaiscript::fun(&simulateParticles), "simulateParticles");
        g_chai->add(chaiscript::fun(&simulateParticlesOneAtATime), "simulateParticlesOneAtATime");
        g_chai->add(chaiscript::fun(&simulateParticlesRoundRobin), "simulateParticlesRoundRobin");
        
        g_chai->add(chaiscript::fun(&dropSimulation), "dropSimulation");
        g_chai->add(chaiscript::fun(&addDrop), "addDrop");
        g_chai->add(chaiscript::fun(&simulateDrops), "simulateDrops");

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
        g_chai->add(chaiscript::fun(&length), "length");
        g_chai->add(chaiscript::fun(&console), "console");
        

        g_chai->add(chaiscript::user_type<point16>(), "point16");
        g_chai->add(chaiscript::fun(&makePoint), "makePoint");
        g_chai->add(chaiscript::fun(&makePointF), "makePoint");

        g_chai->add(chaiscript::user_type<Pixel>(), "Pixel");
        g_chai->add(chaiscript::bootstrap::standard_library::vector_type<std::vector<Pixel> >("PixelVector"));
        g_chai->add(chaiscript::map_conversion<std::map<std::string, byte>>());
        //g_chai->add(chaiscript::bootstrap::standard_library::map_type<std::map<double, byte> >("ColorMap"));

        g_chai->add(chaiscript::fun(&randomizePixelList), "randomizePixelList");
        g_chai->add(chaiscript::fun(&Cel::getPixel), "getPixel");
        g_chai->add(chaiscript::fun(&Cel::getWidth), "getWidth");
        g_chai->add(chaiscript::fun(&Cel::getHeight), "getHeight");
        g_chai->add(chaiscript::fun(&Cel::getTransparent), "getTransparent");
        g_chai->add(chaiscript::fun(&Cel::drawPixel), "drawPixel");
        g_chai->add(chaiscript::fun(&Cel::fill), "fill");
        g_chai->add(chaiscript::fun(&Cel::drawLine), "drawLine");
        g_chai->add(chaiscript::fun(&Cel::getRandomPixel), "getRandomPixel");
        g_chai->add(chaiscript::fun(&Cel::selectPixelsOfColor), "selectPixelsOfColor");
        g_chai->add(chaiscript::fun(&Cel::selectPixelsNotOfColor), "selectPixelsNotOfColor");
        g_chai->add(chaiscript::fun(&Cel::clear), "clear");
        g_chai->add(chaiscript::fun(&Loop::getCel), "getCel");
        g_chai->add(chaiscript::fun(&Loop::celCount), "celCount");
        g_chai->add(chaiscript::fun(&Loop::clear), "clear");
        g_chai->add(chaiscript::fun(&RasterComponent::getCel), "getCel");

        g_chai->add(chaiscript::fun(&addParticle), "addParticle");
        g_chai->add(chaiscript::fun(&addParticleColor), "addParticle");
        g_chai->add(chaiscript::fun(&addLineParticle), "addLineParticle");
        g_chai->add(chaiscript::fun(&addBrushParticle), "addBrushParticle");
        
        g_chai->add(chaiscript::fun(&declareColorOverLife), "declareColorOverLife");
        g_chai->add(chaiscript::fun(&declareSizeOverLife), "declareSizeOverLife");
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

void Cel::fill(point16 point, byte color)
{
    // Use a unique colormapping for palette indices, otherwise the flood-fill may fail when the same RGB colors are
    // represented by multiple different palette indices.
    CSCIDrawHelper helper(nullptr, &this->Data[0], this->size, g_continuousPriorityColors, 256);
    COLORREF mainColor = RGB(0, color, 0);
    CBrush brush;
    HGDIOBJ hOld;
    int crTextOld, crBkOld;
    brush.CreateSolidBrush(mainColor);
    hOld = helper.dc.SelectObject(&brush);
    COLORREF colorRef = helper.dc.GetPixel(PointToCPoint(point));
    // This fails for white/silver mix for some reason.
    BOOL fResult = helper.dc.ExtFloodFill(point.x, point.y, colorRef, FLOODFILLSURFACE);
    helper.dc.SelectObject(hOld);
}


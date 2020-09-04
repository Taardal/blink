#include "World.h"
#include <iostream>

namespace blink
{
    World::World(const char* path)
            : path(path), width(2), height(2)
    {
        std::cout << "Created world [" << path << "]" << " with [width: " << width << ", height: " << height << "]" << std::endl;
    }

    void World::Update() const
    {
        std::cout << "Updated world" << std::endl;
    }

    void World::Draw() const
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int i = x + y * width;
                std::cout << "Drawing tile [" << i << "]" << " at [X: " << x << ", Y: " << y << "]" << std::endl;
            }
        }
    }
}

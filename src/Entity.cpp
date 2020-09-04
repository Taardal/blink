#include "Entity.h"
#include <iostream>

namespace blink
{
    Entity::Entity()
            : x(0), y(0)
    {
        std::cout << "Created entity" << std::endl;
    }

    Entity::~Entity()
    {
        std::cout << "Destroyed entity" << std::endl;
    }

    void Entity::Update(int velocityX, int velocityY)
    {
        x += velocityX;
        y += velocityY;
        std::cout << "Moved entity to " << x << ", " << y << std::endl;
    }

    void Entity::Draw() const
    {
        std::cout << "Drawed entity at " << x << ", " << y << std::endl;
    }
}



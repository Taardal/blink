#pragma once


namespace blink
{
    class Entity
    {
    private:
        int x;
        int y;

    public:
        Entity();

        virtual ~Entity();

        void Update(int velocityX, int velocityY);

        void Draw() const;
    };
}


#pragma once


namespace blink
{
    class World
    {
    private:
        const char* path;
        int width;
        int height;

    public:
        explicit World(const char* path);

        void Update() const;

        void Draw() const;
    };
}



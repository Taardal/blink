#pragma once

#include "system/SystemModule.h"
#include "window/WindowModule.h"
#include "graphics/GraphicsModule.h"
#include "GameModule.h"

namespace Blink {
    class App {
    private:
        SystemModule* systemModule;
        WindowModule* windowModule;
        GraphicsModule* graphicsModule;
        GameModule* gameModule;

    public:
        App();

        ~App();

        void run(const Config& config) const;

    private:
        bool initialize(const Config& config) const;

        void terminate() const;
    };
}



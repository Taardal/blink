package.path = './?.lua;' .. package.path
package.path = './?.out;' .. package.path

local Player = require("player")
--local Camera = require("camera")

function main()
  print("Hello World")
  Player.onUpdate()
  --Camera.onUpdate()
  print("Completed successfully")
end

main()

--function blink.onCreate()
--
--end
--
--function blink.onUpdate()
--
--end
--
--function blink.onDraw()
--    blink.graphics.drawRectangle({
--        width = 200,
--        height = 200,
--        x = 1,
--        y = 1,
--        r = 0.8,
--        g = 0.2,
--        b = 0.8,
--        a = 1.0,
--    });
--end

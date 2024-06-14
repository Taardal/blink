local testCount = 0

local entityOffset = 100

local groundSize = glm.vec3(50, 1, 100)
local fighterSize = glm.vec3(100, 100, 100)
local oxarFreighterSize = glm.vec3(100, 100, 100)
local cameraSize = glm.vec3(100, 100, 100)
local sibenikSize = glm.vec3(1, 1, 1)
local vikingRoomSize = glm.vec3(33, 33, 33)

local function calculateGroundPosition()
    return glm.vec3(0, 0, (testCount * (groundSize.z * 2)) * 1.5)
end

local function calculateEntityPosition()
    return calculateGroundPosition() + glm.vec3(0, 50, 0)
end

local function createEntityOffsetCalculationFunction()
    local xOffset = 0
    return function(entityCount)
        if entityCount % 2 ~= 0 then
            xOffset = xOffset + entityOffset
        end
        local x = xOffset
        if entityCount % 2 == 0 then
            x = x * -1
        end
        return glm.vec3(x, 0, 0)
    end
end

local function yawTest()
    local entityCount = 0
    local calculateEntityOffset = createEntityOffsetCalculationFunction()

    -- --------------------------------------------------------------------------------------------------------------
    -- FIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local fighter = Entity:create()
    Entity:setTagComponent(fighter, {
        tag = "Yaw: Fighter"
    })
    Entity:setLuaComponent(fighter, {
        type = "Yaw",
        path = "lua/scenes/rotation_test/entities/yaw.out",
    })
    Entity:setMeshComponent(fighter, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/cinfa.jpg",
    })
    Entity:setTransformComponent(fighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = fighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- OXAR FREIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local oxarFreighter = Entity:create()
    Entity:setTagComponent(oxarFreighter, {
        tag = "Yaw: Oxar Freighter"
    })
    Entity:setLuaComponent(oxarFreighter, {
        type = "Yaw",
        path = "lua/scenes/rotation_test/entities/yaw.out",
    })
    Entity:setMeshComponent(oxarFreighter, {
        modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj",
        textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png",
    })
    Entity:setTransformComponent(oxarFreighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = oxarFreighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- CAMERA
    -- --------------------------------------------------------------------------------------------------------------

    local camera = Entity:create()
    Entity:setTagComponent(camera, {
        tag = "Yaw: Camera"
    })
    Entity:setLuaComponent(camera, {
        type = "Yaw",
        path = "lua/scenes/rotation_test/entities/yaw.out",
    })
    Entity:setMeshComponent(camera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(camera, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = cameraSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- SIBENIK CATHEDRAL
    -- --------------------------------------------------------------------------------------------------------------

    local sibenik = Entity:create()
    Entity:setTagComponent(sibenik, {
        tag = "Yaw: Sibenik"
    })
    Entity:setLuaComponent(sibenik, {
        type = "Yaw",
        path = "lua/scenes/rotation_test/entities/yaw.out",
    })
    Entity:setMeshComponent(sibenik, {
        modelPath = "models/sibenik/sibenik.obj",
        texturesDirectoryPath = "models/sibenik",
    })
    Entity:setTransformComponent(sibenik, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = sibenikSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- VIKING ROOM
    -- --------------------------------------------------------------------------------------------------------------

    local vikingRoom = Entity:create()
    Entity:setTagComponent(vikingRoom, {
        tag = "Yaw: Viking Room"
    })
    Entity:setLuaComponent(vikingRoom, {
        type = "Yaw",
        path = "lua/scenes/rotation_test/entities/yaw.out",
    })
    Entity:setMeshComponent(vikingRoom, {
        modelPath = "models/viking_room/viking_room.obj",
        textureAtlasPath = "models/viking_room/viking_room.png",
    })
    Entity:setTransformComponent(vikingRoom, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = vikingRoomSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- GROUND
    -- --------------------------------------------------------------------------------------------------------------

    local ground = Entity:create()
    Entity:setTagComponent(ground, {
        tag = "Yaw: Ground"
    })
    Entity:setMeshComponent(ground, {
        modelPath = "models/flat_plain/flat_plain.obj",
        textureAtlasPath = "models/flat_plain/grass_4k.png",
    })
    Entity:setTransformComponent(ground, {
        position = calculateGroundPosition(),
        size = groundSize + glm.vec3(entityCount * groundSize.x, 0, 0)
    })
end


local function pitchTest()
    local entityCount = 0
    local calculateEntityOffset = createEntityOffsetCalculationFunction()

    -- --------------------------------------------------------------------------------------------------------------
    -- FIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local fighter = Entity:create()
    Entity:setTagComponent(fighter, {
        tag = "Pitch: Fighter"
    })
    Entity:setLuaComponent(fighter, {
        type = "Pitch",
        path = "lua/scenes/rotation_test/entities/pitch.out",
    })
    Entity:setMeshComponent(fighter, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/cinfa.jpg",
    })
    Entity:setTransformComponent(fighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = fighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- OXAR FREIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local oxarFreighter = Entity:create()
    Entity:setTagComponent(oxarFreighter, {
        tag = "Pitch: Oxar Freighter"
    })
    Entity:setLuaComponent(oxarFreighter, {
        type = "Pitch",
        path = "lua/scenes/rotation_test/entities/pitch.out",
    })
    Entity:setMeshComponent(oxarFreighter, {
        modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj",
        textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png",
    })
    Entity:setTransformComponent(oxarFreighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = oxarFreighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- CAMERA
    -- --------------------------------------------------------------------------------------------------------------

    local camera = Entity:create()
    Entity:setTagComponent(camera, {
        tag = "Pitch: Camera"
    })
    Entity:setLuaComponent(camera, {
        type = "Pitch",
        path = "lua/scenes/rotation_test/entities/pitch.out",
    })
    Entity:setMeshComponent(camera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(camera, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = cameraSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- SIBENIK CATHEDRAL
    -- --------------------------------------------------------------------------------------------------------------

    local sibenik = Entity:create()
    Entity:setTagComponent(sibenik, {
        tag = "Pitch: Sibenik"
    })
    Entity:setLuaComponent(sibenik, {
        type = "Pitch",
        path = "lua/scenes/rotation_test/entities/pitch.out",
    })
    Entity:setMeshComponent(sibenik, {
        modelPath = "models/sibenik/sibenik.obj",
        texturesDirectoryPath = "models/sibenik",
    })
    Entity:setTransformComponent(sibenik, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = sibenikSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- VIKING ROOM
    -- --------------------------------------------------------------------------------------------------------------

    local vikingRoom = Entity:create()
    Entity:setTagComponent(vikingRoom, {
        tag = "Pitch: Viking Room"
    })
    Entity:setLuaComponent(vikingRoom, {
        type = "Pitch",
        path = "lua/scenes/rotation_test/entities/pitch.out",
    })
    Entity:setMeshComponent(vikingRoom, {
        modelPath = "models/viking_room/viking_room.obj",
        textureAtlasPath = "models/viking_room/viking_room.png",
    })
    Entity:setTransformComponent(vikingRoom, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = vikingRoomSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- GROUND
    -- --------------------------------------------------------------------------------------------------------------

    local ground = Entity:create()
    Entity:setTagComponent(ground, {
        tag = "Pitch: Ground"
    })
    Entity:setMeshComponent(ground, {
        modelPath = "models/flat_plain/flat_plain.obj",
        textureAtlasPath = "models/flat_plain/grass_4k.png",
    })
    Entity:setTransformComponent(ground, {
        position = calculateGroundPosition(),
        size = groundSize + glm.vec3(entityCount * groundSize.x, 0, 0)
    })
end

local function rollTest()
    local entityCount = 0
    local calculateEntityOffset = createEntityOffsetCalculationFunction()

    -- --------------------------------------------------------------------------------------------------------------
    -- FIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local fighter = Entity:create()
    Entity:setTagComponent(fighter, {
        tag = "Roll: Fighter"
    })
    Entity:setLuaComponent(fighter, {
        type = "Roll",
        path = "lua/scenes/rotation_test/entities/roll.out",
    })
    Entity:setMeshComponent(fighter, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/cinfa.jpg",
    })
    Entity:setTransformComponent(fighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = fighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- OXAR FREIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local oxarFreighter = Entity:create()
    Entity:setTagComponent(oxarFreighter, {
        tag = "Roll: Oxar Freighter"
    })
    Entity:setLuaComponent(oxarFreighter, {
        type = "Roll",
        path = "lua/scenes/rotation_test/entities/roll.out",
    })
    Entity:setMeshComponent(oxarFreighter, {
        modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj",
        textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png",
    })
    Entity:setTransformComponent(oxarFreighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = oxarFreighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- CAMERA
    -- --------------------------------------------------------------------------------------------------------------

    local camera = Entity:create()
    Entity:setTagComponent(camera, {
        tag = "Roll: Camera"
    })
    Entity:setLuaComponent(camera, {
        type = "Roll",
        path = "lua/scenes/rotation_test/entities/roll.out",
    })
    Entity:setMeshComponent(camera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(camera, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = cameraSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- SIBENIK CATHEDRAL
    -- --------------------------------------------------------------------------------------------------------------

    local sibenik = Entity:create()
    Entity:setTagComponent(sibenik, {
        tag = "Roll: Sibenik"
    })
    Entity:setLuaComponent(sibenik, {
        type = "Roll",
        path = "lua/scenes/rotation_test/entities/roll.out",
    })
    Entity:setMeshComponent(sibenik, {
        modelPath = "models/sibenik/sibenik.obj",
        texturesDirectoryPath = "models/sibenik",
    })
    Entity:setTransformComponent(sibenik, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = sibenikSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- VIKING ROOM
    -- --------------------------------------------------------------------------------------------------------------

    local vikingRoom = Entity:create()
    Entity:setTagComponent(vikingRoom, {
        tag = "Roll: Viking Room"
    })
    Entity:setLuaComponent(vikingRoom, {
        type = "Roll",
        path = "lua/scenes/rotation_test/entities/roll.out",
    })
    Entity:setMeshComponent(vikingRoom, {
        modelPath = "models/viking_room/viking_room.obj",
        textureAtlasPath = "models/viking_room/viking_room.png",
    })
    Entity:setTransformComponent(vikingRoom, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = vikingRoomSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- GROUND
    -- --------------------------------------------------------------------------------------------------------------

    local ground = Entity:create()
    Entity:setTagComponent(ground, {
        tag = "Roll: Ground"
    })
    Entity:setMeshComponent(ground, {
        modelPath = "models/flat_plain/flat_plain.obj",
        textureAtlasPath = "models/flat_plain/grass_4k.png",
    })
    Entity:setTransformComponent(ground, {
        position = calculateGroundPosition(),
        size = groundSize + glm.vec3(entityCount * groundSize.x, 0, 0)
    })
end

local function yawAndPitchTest()
    local entityCount = 0
    local calculateEntityOffset = createEntityOffsetCalculationFunction()

    -- --------------------------------------------------------------------------------------------------------------
    -- FIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local fighter = Entity:create()
    Entity:setTagComponent(fighter, {
        tag = "Yaw and Pitch: Fighter"
    })
    Entity:setLuaComponent(fighter, {
        type = "YawAndPitch",
        path = "lua/scenes/rotation_test/entities/yaw_and_pitch.out",
    })
    Entity:setMeshComponent(fighter, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/cinfa.jpg",
    })
    Entity:setTransformComponent(fighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = fighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- OXAR FREIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local oxarFreighter = Entity:create()
    Entity:setTagComponent(oxarFreighter, {
        tag = "Yaw and Pitch: Oxar Freighter"
    })
    Entity:setLuaComponent(oxarFreighter, {
        type = "YawAndPitch",
        path = "lua/scenes/rotation_test/entities/yaw_and_pitch.out",
    })
    Entity:setMeshComponent(oxarFreighter, {
        modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj",
        textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png",
    })
    Entity:setTransformComponent(oxarFreighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = oxarFreighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- CAMERA
    -- --------------------------------------------------------------------------------------------------------------

    local camera = Entity:create()
    Entity:setTagComponent(camera, {
        tag = "Yaw and Pitch: Camera"
    })
    Entity:setLuaComponent(camera, {
        type = "YawAndPitch",
        path = "lua/scenes/rotation_test/entities/yaw_and_pitch.out",
    })
    Entity:setMeshComponent(camera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(camera, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = cameraSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- SIBENIK CATHEDRAL
    -- --------------------------------------------------------------------------------------------------------------

    local sibenik = Entity:create()
    Entity:setTagComponent(sibenik, {
        tag = "Yaw and Pitch: Sibenik"
    })
    Entity:setLuaComponent(sibenik, {
        type = "YawAndPitch",
        path = "lua/scenes/rotation_test/entities/yaw_and_pitch.out",
    })
    Entity:setMeshComponent(sibenik, {
        modelPath = "models/sibenik/sibenik.obj",
        texturesDirectoryPath = "models/sibenik",
    })
    Entity:setTransformComponent(sibenik, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = sibenikSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- VIKING ROOM
    -- --------------------------------------------------------------------------------------------------------------

    local vikingRoom = Entity:create()
    Entity:setTagComponent(vikingRoom, {
        tag = "Yaw and Pitch: Viking Room"
    })
    Entity:setLuaComponent(vikingRoom, {
        type = "YawAndPitch",
        path = "lua/scenes/rotation_test/entities/yaw_and_pitch.out",
    })
    Entity:setMeshComponent(vikingRoom, {
        modelPath = "models/viking_room/viking_room.obj",
        textureAtlasPath = "models/viking_room/viking_room.png",
    })
    Entity:setTransformComponent(vikingRoom, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = vikingRoomSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- GROUND
    -- --------------------------------------------------------------------------------------------------------------

    local ground = Entity:create()
    Entity:setTagComponent(ground, {
        tag = "Yaw and Pitch: Ground"
    })
    Entity:setMeshComponent(ground, {
        modelPath = "models/flat_plain/flat_plain.obj",
        textureAtlasPath = "models/flat_plain/grass_4k.png",
    })
    Entity:setTransformComponent(ground, {
        position = calculateGroundPosition(),
        size = groundSize + glm.vec3(entityCount * groundSize.x, 0, 0)
    })
end

local function yawAndRollTest()
    local entityCount = 0
    local calculateEntityOffset = createEntityOffsetCalculationFunction()

    -- --------------------------------------------------------------------------------------------------------------
    -- FIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local fighter = Entity:create()
    Entity:setTagComponent(fighter, {
        tag = "Yaw and Roll: Fighter"
    })
    Entity:setLuaComponent(fighter, {
        type = "YawAndRoll",
        path = "lua/scenes/rotation_test/entities/yaw_and_roll.out",
    })
    Entity:setMeshComponent(fighter, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/cinfa.jpg",
    })
    Entity:setTransformComponent(fighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = fighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- OXAR FREIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local oxarFreighter = Entity:create()
    Entity:setTagComponent(oxarFreighter, {
        tag = "Yaw and Roll: Oxar Freighter"
    })
    Entity:setLuaComponent(oxarFreighter, {
        type = "YawAndRoll",
        path = "lua/scenes/rotation_test/entities/yaw_and_roll.out",
    })
    Entity:setMeshComponent(oxarFreighter, {
        modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj",
        textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png",
    })
    Entity:setTransformComponent(oxarFreighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = oxarFreighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- CAMERA
    -- --------------------------------------------------------------------------------------------------------------

    local camera = Entity:create()
    Entity:setTagComponent(camera, {
        tag = "Yaw and Roll: Camera"
    })
    Entity:setLuaComponent(camera, {
        type = "YawAndRoll",
        path = "lua/scenes/rotation_test/entities/yaw_and_roll.out",
    })
    Entity:setMeshComponent(camera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(camera, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = cameraSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- SIBENIK CATHEDRAL
    -- --------------------------------------------------------------------------------------------------------------

    local sibenik = Entity:create()
    Entity:setTagComponent(sibenik, {
        tag = "Yaw and Roll: Sibenik"
    })
    Entity:setLuaComponent(sibenik, {
        type = "YawAndRoll",
        path = "lua/scenes/rotation_test/entities/yaw_and_roll.out",
    })
    Entity:setMeshComponent(sibenik, {
        modelPath = "models/sibenik/sibenik.obj",
        texturesDirectoryPath = "models/sibenik",
    })
    Entity:setTransformComponent(sibenik, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = sibenikSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- VIKING ROOM
    -- --------------------------------------------------------------------------------------------------------------

    local vikingRoom = Entity:create()
    Entity:setTagComponent(vikingRoom, {
        tag = "Yaw and Roll: Viking Room"
    })
    Entity:setLuaComponent(vikingRoom, {
        type = "YawAndRoll",
        path = "lua/scenes/rotation_test/entities/yaw_and_roll.out",
    })
    Entity:setMeshComponent(vikingRoom, {
        modelPath = "models/viking_room/viking_room.obj",
        textureAtlasPath = "models/viking_room/viking_room.png",
    })
    Entity:setTransformComponent(vikingRoom, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = vikingRoomSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- GROUND
    -- --------------------------------------------------------------------------------------------------------------

    local ground = Entity:create()
    Entity:setTagComponent(ground, {
        tag = "Yaw and Roll: Ground"
    })
    Entity:setMeshComponent(ground, {
        modelPath = "models/flat_plain/flat_plain.obj",
        textureAtlasPath = "models/flat_plain/grass_4k.png",
    })
    Entity:setTransformComponent(ground, {
        position = calculateGroundPosition(),
        size = groundSize + glm.vec3(entityCount * groundSize.x, 0, 0)
    })
end

local function pitchAndRollTest()
    local entityCount = 0
    local calculateEntityOffset = createEntityOffsetCalculationFunction()

    -- --------------------------------------------------------------------------------------------------------------
    -- FIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local fighter = Entity:create()
    Entity:setTagComponent(fighter, {
        tag = "Pitch and Roll: Fighter"
    })
    Entity:setLuaComponent(fighter, {
        type = "PitchAndRoll",
        path = "lua/scenes/rotation_test/entities/pitch_and_roll.out",
    })
    Entity:setMeshComponent(fighter, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/cinfa.jpg",
    })
    Entity:setTransformComponent(fighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = fighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- OXAR FREIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local oxarFreighter = Entity:create()
    Entity:setTagComponent(oxarFreighter, {
        tag = "Pitch and Roll: Oxar Freighter"
    })
    Entity:setLuaComponent(oxarFreighter, {
        type = "PitchAndRoll",
        path = "lua/scenes/rotation_test/entities/pitch_and_roll.out",
    })
    Entity:setMeshComponent(oxarFreighter, {
        modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj",
        textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png",
    })
    Entity:setTransformComponent(oxarFreighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = oxarFreighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- CAMERA
    -- --------------------------------------------------------------------------------------------------------------

    local camera = Entity:create()
    Entity:setTagComponent(camera, {
        tag = "Pitch and Roll: Camera"
    })
    Entity:setLuaComponent(camera, {
        type = "PitchAndRoll",
        path = "lua/scenes/rotation_test/entities/pitch_and_roll.out",
    })
    Entity:setMeshComponent(camera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(camera, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = cameraSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- SIBENIK CATHEDRAL
    -- --------------------------------------------------------------------------------------------------------------

    local sibenik = Entity:create()
    Entity:setTagComponent(sibenik, {
        tag = "Pitch and Roll: Sibenik"
    })
    Entity:setLuaComponent(sibenik, {
        type = "PitchAndRoll",
        path = "lua/scenes/rotation_test/entities/pitch_and_roll.out",
    })
    Entity:setMeshComponent(sibenik, {
        modelPath = "models/sibenik/sibenik.obj",
        texturesDirectoryPath = "models/sibenik",
    })
    Entity:setTransformComponent(sibenik, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = sibenikSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- VIKING ROOM
    -- --------------------------------------------------------------------------------------------------------------

    local vikingRoom = Entity:create()
    Entity:setTagComponent(vikingRoom, {
        tag = "Pitch and Roll: Viking Room"
    })
    Entity:setLuaComponent(vikingRoom, {
        type = "PitchAndRoll",
        path = "lua/scenes/rotation_test/entities/pitch_and_roll.out",
    })
    Entity:setMeshComponent(vikingRoom, {
        modelPath = "models/viking_room/viking_room.obj",
        textureAtlasPath = "models/viking_room/viking_room.png",
    })
    Entity:setTransformComponent(vikingRoom, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = vikingRoomSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- GROUND
    -- --------------------------------------------------------------------------------------------------------------

    local ground = Entity:create()
    Entity:setTagComponent(ground, {
        tag = "Pitch and Roll: Ground"
    })
    Entity:setMeshComponent(ground, {
        modelPath = "models/flat_plain/flat_plain.obj",
        textureAtlasPath = "models/flat_plain/grass_4k.png",
    })
    Entity:setTransformComponent(ground, {
        position = calculateGroundPosition(),
        size = groundSize + glm.vec3(entityCount * groundSize.x, 0, 0)
    })
end

local function yawPitchAndRollTest()
    local entityCount = 0
    local calculateEntityOffset = createEntityOffsetCalculationFunction()

    -- --------------------------------------------------------------------------------------------------------------
    -- FIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local fighter = Entity:create()
    Entity:setTagComponent(fighter, {
        tag = "Yaw, Pitch and Roll: Fighter"
    })
    Entity:setLuaComponent(fighter, {
        type = "YawPitchAndRoll",
        path = "lua/scenes/rotation_test/entities/yaw_pitch_and_roll.out",
    })
    Entity:setMeshComponent(fighter, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/cinfa.jpg",
    })
    Entity:setTransformComponent(fighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = fighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- OXAR FREIGHTER
    -- --------------------------------------------------------------------------------------------------------------

    local oxarFreighter = Entity:create()
    Entity:setTagComponent(oxarFreighter, {
        tag = "Yaw, Pitch and Roll: Oxar Freighter"
    })
    Entity:setLuaComponent(oxarFreighter, {
        type = "YawPitchAndRoll",
        path = "lua/scenes/rotation_test/entities/yaw_pitch_and_roll.out",
    })
    Entity:setMeshComponent(oxarFreighter, {
        modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj",
        textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png",
    })
    Entity:setTransformComponent(oxarFreighter, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = oxarFreighterSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- CAMERA
    -- --------------------------------------------------------------------------------------------------------------

    local camera = Entity:create()
    Entity:setTagComponent(camera, {
        tag = "Yaw, Pitch and Roll: Camera"
    })
    Entity:setLuaComponent(camera, {
        type = "YawPitchAndRoll",
        path = "lua/scenes/rotation_test/entities/yaw_pitch_and_roll.out",
    })
    Entity:setMeshComponent(camera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(camera, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = cameraSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- SIBENIK CATHEDRAL
    -- --------------------------------------------------------------------------------------------------------------

    local sibenik = Entity:create()
    Entity:setTagComponent(sibenik, {
        tag = "Yaw, Pitch and Roll: Sibenik"
    })
    Entity:setLuaComponent(sibenik, {
        type = "YawPitchAndRoll",
        path = "lua/scenes/rotation_test/entities/yaw_pitch_and_roll.out",
    })
    Entity:setMeshComponent(sibenik, {
        modelPath = "models/sibenik/sibenik.obj",
        texturesDirectoryPath = "models/sibenik",
    })
    Entity:setTransformComponent(sibenik, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = sibenikSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- VIKING ROOM
    -- --------------------------------------------------------------------------------------------------------------

    local vikingRoom = Entity:create()
    Entity:setTagComponent(vikingRoom, {
        tag = "Yaw, Pitch and Roll: Viking Room"
    })
    Entity:setLuaComponent(vikingRoom, {
        type = "YawPitchAndRoll",
        path = "lua/scenes/rotation_test/entities/yaw_pitch_and_roll.out",
    })
    Entity:setMeshComponent(vikingRoom, {
        modelPath = "models/viking_room/viking_room.obj",
        textureAtlasPath = "models/viking_room/viking_room.png",
    })
    Entity:setTransformComponent(vikingRoom, {
        position = calculateEntityPosition() + calculateEntityOffset(entityCount),
        size = vikingRoomSize,
    })
    entityCount = entityCount + 1

    -- --------------------------------------------------------------------------------------------------------------
    -- GROUND
    -- --------------------------------------------------------------------------------------------------------------

    local ground = Entity:create()
    Entity:setTagComponent(ground, {
        tag = "Yaw, Pitch and Roll: Ground"
    })
    Entity:setMeshComponent(ground, {
        modelPath = "models/flat_plain/flat_plain.obj",
        textureAtlasPath = "models/flat_plain/grass_4k.png",
    })
    Entity:setTransformComponent(ground, {
        position = calculateGroundPosition(),
        size = groundSize + glm.vec3(entityCount * groundSize.x, 0, 0)
    })
end

function Scene.onCreateEntities()
    yawTest()
    testCount = testCount + 1
    pitchTest()
    testCount = testCount + 1
    rollTest()
    testCount = testCount + 1
    yawAndPitchTest()
    testCount = testCount + 1
    yawAndRollTest()
    testCount = testCount + 1
    pitchAndRollTest()
    testCount = testCount + 1
    yawPitchAndRollTest()
    testCount = testCount + 1
end

function Scene.onConfigureCamera()
    SceneCamera:setMoveSpeed(200)
    SceneCamera:setNearClip(0.1)
    SceneCamera:setFarClip(10000)
    SceneCamera:setPosition(-347.88528, 254.04382, -295.1399)
    SceneCamera:setPitch(-36)
    SceneCamera:setYaw(-132)
end

function Scene.onConfigureSkybox()
    local name = "space-blue"
    local directoryPath = "skyboxes/" .. name
    local fileType = "png"
	Skybox:setSkybox({
        directoryPath .. "/" .. "right" .. "." .. fileType,
        directoryPath .. "/" .. "left" .. "." .. fileType,
        directoryPath .. "/" .. "top" .. "." .. fileType,
        directoryPath .. "/" .. "bottom" .. "." .. fileType,
        directoryPath .. "/" .. "front" .. "." .. fileType,
        directoryPath .. "/" .. "back" .. "." .. fileType,
    })
end
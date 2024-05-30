function Scene.onConfigureCamera()
    SceneCamera:setMoveSpeed(100)
    SceneCamera:setNearClip(0.1)
    SceneCamera:setFarClip(10000)
    SceneCamera:setPosition(-347.88528, 254.04382, -295.1399)
    SceneCamera:setPitch(-36)
    SceneCamera:setYaw(-132)
end

local plainSize = glm.vec3(200, 1, 200)
local fighterJetSize = glm.vec3(100, 100, 100)

function Scene.onCreateEntities()
    local ground = Entity:create()
    Entity:setTagComponent(ground, {
        tag = "Flat plain"
    })
    Entity:setMeshComponent(ground, {
        modelPath = "models/flat_plain/flat_plain.obj",
        textureAtlasPath = "models/flat_plain/grass_4k.png",
    })
    Entity:setTransformComponent(ground, {
        position = glm.vec3(0, 0, 0),
        size = plainSize,
    })

    local fighterJets = {}

    local fighterJetYaw = Entity:create()

    Entity:setTagComponent(fighterJetYaw, {
        tag = "Fighter jet: Yaw"
    })
    Entity:setLuaComponent(fighterJetYaw, {
        type = "FighterJetYaw",
        path = "lua/scenes/rotation_test/entities/fighter_jet_yaw.out",
    })
    Entity:setMeshComponent(fighterJetYaw, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/cinfa.jpg",
    })
    Entity:setTransformComponent(fighterJetYaw, {
        position = glm.vec3(-100, 50, 0),
        size = fighterJetSize,
    })

    local fighterJetPitch = Entity:create()

    Entity:setTagComponent(fighterJetPitch, {
        tag = "Fighter jet: Pitch"
    })
    Entity:setLuaComponent(fighterJetPitch, {
        type = "FighterJetPitch",
        path = "lua/scenes/rotation_test/entities/fighter_jet_pitch.out",
    })
    Entity:setMeshComponent(fighterJetPitch, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/crono782.jpg",
    })
    Entity:setTransformComponent(fighterJetPitch, {
        position = glm.vec3(-100, 50, -100),
        size = fighterJetSize,
    })

    local fighterJetRoll = Entity:create()

    Entity:setTagComponent(fighterJetRoll, {
        tag = "Fighter jet: Roll"
    })
    Entity:setLuaComponent(fighterJetRoll, {
        type = "FighterJetRoll",
        path = "lua/scenes/rotation_test/entities/fighter_jet_roll.out",
    })
    Entity:setMeshComponent(fighterJetRoll, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/idolknight.jpg",
    })
    Entity:setTransformComponent(fighterJetRoll, {
        position = glm.vec3(-100, 50, 100),
        size = fighterJetSize,
    })

    local fighterJetYawAndPitch = Entity:create()

    Entity:setTagComponent(fighterJetYawAndPitch, {
        tag = "Fighter jet: YawAndPitch"
    })
    Entity:setLuaComponent(fighterJetYawAndPitch, {
        type = "FighterJetYawAndPitch",
        path = "lua/scenes/rotation_test/entities/fighter_jet_yaw_and_pitch.out",
    })
    Entity:setMeshComponent(fighterJetYawAndPitch, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/jodomatis.jpg",
    })
    Entity:setTransformComponent(fighterJetYawAndPitch, {
        position = glm.vec3(100, 50, -100),
        size = fighterJetSize,
    })

    local fighterJetYawAndRoll = Entity:create()

    Entity:setTagComponent(fighterJetYawAndRoll, {
        tag = "Fighter jet: YawAndRoll"
    })
    Entity:setLuaComponent(fighterJetYawAndRoll, {
        type = "FighterJetYawAndRoll",
        path = "lua/scenes/rotation_test/entities/fighter_jet_yaw_and_roll.out",
    })
    Entity:setMeshComponent(fighterJetYawAndRoll, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/kaoskiwi.jpg",
    })
    Entity:setTransformComponent(fighterJetYawAndRoll, {
        position = glm.vec3(100, 50, 0),
        size = fighterJetSize,
    })

    local fighterJetPitchAndRoll = Entity:create()

    Entity:setTagComponent(fighterJetPitchAndRoll, {
        tag = "Fighter jet: PitchAndRoll"
    })
    Entity:setLuaComponent(fighterJetPitchAndRoll, {
        type = "FighterJetPitchAndRoll",
        path = "lua/scenes/rotation_test/entities/fighter_jet_pitch_and_roll.out",
    })
    Entity:setMeshComponent(fighterJetPitchAndRoll, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/NMT3000.jpg",
    })
    Entity:setTransformComponent(fighterJetPitchAndRoll, {
        position = glm.vec3(100, 50, 100),
        size = fighterJetSize,
    })

    local fighterJetYawPitchAndRoll = Entity:create()

    Entity:setTagComponent(fighterJetYawPitchAndRoll, {
        tag = "Fighter jet: YawPitchAndRoll"
    })
    Entity:setLuaComponent(fighterJetYawPitchAndRoll, {
        type = "FighterJetYawPitchAndRoll",
        path = "lua/scenes/rotation_test/entities/fighter_jet_yaw_pitch_and_roll.out",
    })
    Entity:setMeshComponent(fighterJetYawPitchAndRoll, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/psionic.jpg",
    })
    Entity:setTransformComponent(fighterJetYawPitchAndRoll, {
        position = glm.vec3(0, 50, 0),
        size = fighterJetSize,
    })

    --for x = 1, #fighterJets, 1 do
    --    for y = 1, 3, 1 do
    --        Entity:setTransformComponent(fighterJets[x * y], {
    --            position = glm.vec3((y - 1) * 100, 50, (x - 1) * 100),
    --        })
    --    end
    --end
end

function Scene.onConfigureSkybox()
    local name = "lake"
    local directoryPath = "skyboxes/" .. name
    local fileType = "jpg"
	Skybox:setSkybox({
        directoryPath .. "/" .. "right" .. "." .. fileType,
        directoryPath .. "/" .. "left" .. "." .. fileType,
        directoryPath .. "/" .. "top" .. "." .. fileType,
        directoryPath .. "/" .. "bottom" .. "." .. fileType,
        directoryPath .. "/" .. "front" .. "." .. fileType,
        directoryPath .. "/" .. "back" .. "." .. fileType,
    })
end

function Scene.onConfigureCamera()
    SceneCamera:setPosition(glm.vec3(282.69107, 189.53714, 316.0191))
    SceneCamera:setYaw(39.5)
    SceneCamera:setPitch(-9)
    SceneCamera:setMoveSpeed(100)
    SceneCamera:setRotationSpeed(0.5)
    SceneCamera:setNearClip(0.1)
    SceneCamera:setFarClip(10000)
end

function Scene.onCreateEntities()
    -- --------------------------------------------------------------------------------------------------------------
    -- TERRAIN
    -- --------------------------------------------------------------------------------------------------------------

    local terrain = Entity:create()

    Entity:setTagComponent(terrain, {
        tag = "Terrain"
    })
    Entity:setMeshComponent(terrain, {
        modelPath = "models/terrain/terrain.obj",
        textureAtlasPath = "models/terrain/diff2.jpg",
    })
    Entity:setTransformComponent(terrain, {
        position = glm.vec3(0, -50, 0),
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- CHURCH
    -- --------------------------------------------------------------------------------------------------------------

    local churchRoofCamera = Entity:create()

    Entity:setTagComponent(churchRoofCamera, {
        tag = "Church roof camera"
    })
    Entity:setLuaComponent(churchRoofCamera, {
        type = "ChurchRoofCamera",
        path = "lua/scenes/sandbox/entities/church_roof_camera.out",
    })
    Entity:setMeshComponent(churchRoofCamera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(churchRoofCamera, {
        position = glm.vec3(-447, 110, -579),
        size = glm.vec3(100, 100, 100)
    })
    Entity:setCameraComponent(churchRoofCamera, {
        aspectRatio = Window:getAspectRatio(),
        fieldOfView = 45,
        nearClip = 0.1,
        farClip = 10000,
    })

    local churchIndoorCamera = Entity:create()

    Entity:setTagComponent(churchIndoorCamera, {
        tag = "Church indoor camera"
    })
    Entity:setLuaComponent(churchIndoorCamera, {
        type = "ChurchIndoorCamera",
        path = "lua/scenes/sandbox/entities/church_indoor_camera.out",
    })
    Entity:setMeshComponent(churchIndoorCamera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(churchIndoorCamera, {
        position = glm.vec3(-479, 0, -590),
        size = glm.vec3(50, 50, 50),
        yaw = 110,
    })
    Entity:setCameraComponent(churchIndoorCamera, {
        aspectRatio = Window:getAspectRatio(),
        fieldOfView = 46,
        nearClip = 0.1,
        farClip = 10000,
    })

    local church = Entity:create()

    Entity:setTagComponent(church, {
        tag = "Church"
    })
    Entity:setMeshComponent(church, {
        modelPath = "models/sibenik/sibenik.obj",
        texturesDirectoryPath = "models/sibenik",
    })
    Entity:setTransformComponent(church, {
        position = glm.vec3(-410, -45, -570),
        size = glm.vec3(5, 5, 5),
        yaw = 75,
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- CHURCH SPINNER
    -- --------------------------------------------------------------------------------------------------------------

    local churchSpinner = Entity:create()

    Entity:setTagComponent(churchSpinner, {
        tag = "Church spinner fighter jet"
    })
    Entity:setLuaComponent(churchSpinner, {
        type = "ChurchSpinner",
        path = "lua/scenes/sandbox/entities/church_spinner.out",
    })
    Entity:setMeshComponent(churchSpinner, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/uvmap.jpg",
    })
    Entity:setTransformComponent(churchSpinner, {
        position = glm.vec3(-410, 150, -570),
        pitch = -90,
        size = glm.vec3(200, 200, 200)
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- MOTHERSHIP
    -- --------------------------------------------------------------------------------------------------------------

    local mothership = Entity:create()

    Entity:setTagComponent(mothership, {
        tag = "Mothership"
    })
    Entity:setLuaComponent(mothership, {
        type = "Mothership",
        path = "lua/scenes/sandbox/entities/mothership.out",
    })
    Entity:setMeshComponent(mothership, {
        modelPath = "models/oxar_freighter/Meshes/oxar_freighter.obj",
        textureAtlasPath = "models/oxar_freighter/Textures/Oxar_Diffuse.png",
    })
    Entity:setTransformComponent(mothership, {
        position = glm.vec3(-410, 260, -570),
        size = glm.vec3(1000, 1000, 1000),
        yaw = 255,
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- ROLL PATROL 1
    -- --------------------------------------------------------------------------------------------------------------

    local rollPatrol1Camera = Entity:create()

    Entity:setTagComponent(rollPatrol1Camera, {
        tag = "Roll patrol 1 camera"
    })
    Entity:setLuaComponent(rollPatrol1Camera, {
        type = "RollPatrolCamera",
        path = "lua/scenes/sandbox/entities/roll_patrol_camera.out",
    })
    Entity:setMeshComponent(rollPatrol1Camera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(rollPatrol1Camera, {
        size = glm.vec3(50, 50, 50)
    })
    Entity:setCameraComponent(rollPatrol1Camera, {
        aspectRatio = Window:getAspectRatio(),
        fieldOfView = 45,
        nearClip = 0.1,
        farClip = 10000,
    })

    local rollPatrol1 = Entity:create()

    Entity:setTagComponent(rollPatrol1, {
        tag = "Roll patrol fighter jet 1"
    })
    Entity:setLuaComponent(rollPatrol1, {
        type = "RollPatrol",
        path = "lua/scenes/sandbox/entities/roll_patrol.out",
    })
    Entity:setMeshComponent(rollPatrol1, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/psionic.jpg",
    })
    Entity:setTransformComponent(rollPatrol1, {
        position = glm.vec3(-410, 175, -400),
        yaw = 255,
        size = glm.vec3(50, 50, 50)
    })

    local rollPatrol2 = Entity:create()

    Entity:setTagComponent(rollPatrol2, {
        tag = "Roll patrol fighter jet 2"
    })
    Entity:setLuaComponent(rollPatrol2, {
        type = "RollPatrol",
        path = "lua/scenes/sandbox/entities/roll_patrol.out",
    })
    Entity:setMeshComponent(rollPatrol2, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/pyschicparrot.jpg",
    })
    Entity:setTransformComponent(rollPatrol2, {
        size = glm.vec3(50, 50, 50),
        position = glm.vec3(-410, 175, -325),
        yaw = 255,
    })

    local rollPatrol3 = Entity:create()

    Entity:setTagComponent(rollPatrol3, {
        tag = "Roll patrol fighter jet 3"
    })
    Entity:setLuaComponent(rollPatrol3, {
        type = "RollPatrol",
        path = "lua/scenes/sandbox/entities/roll_patrol.out",
    })
    Entity:setMeshComponent(rollPatrol3, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/thor.jpg",
    })
    Entity:setTransformComponent(rollPatrol3, {
        size = glm.vec3(50, 50, 50),
        position = glm.vec3(-410, 225, -350),
        yaw = 255,
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- ROLL PATROL 2
    -- --------------------------------------------------------------------------------------------------------------

    local rollPatrol4 = Entity:create()

    Entity:setTagComponent(rollPatrol4, {
        tag = "Roll patrol fighter jet 4"
    })
    Entity:setLuaComponent(rollPatrol4, {
        type = "RollPatrol",
        path = "lua/scenes/sandbox/entities/roll_patrol.out",
    })
    Entity:setMeshComponent(rollPatrol4, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/robin.jpg",
    })
    Entity:setTransformComponent(rollPatrol4, {
        position = glm.vec3(-410, 175, -775),
        yaw = 75,
        size = glm.vec3(50, 50, 50)
    })

    local rollPatrol5 = Entity:create()

    Entity:setTagComponent(rollPatrol5, {
        tag = "Roll patrol fighter jet 5"
    })
    Entity:setLuaComponent(rollPatrol5, {
        type = "RollPatrol",
        path = "lua/scenes/sandbox/entities/roll_patrol.out",
    })
    Entity:setMeshComponent(rollPatrol5, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/snaz12.jpg",
    })
    Entity:setTransformComponent(rollPatrol5, {
        size = glm.vec3(50, 50, 50),
        position = glm.vec3(-410, 175, -700),
        yaw = 75,
    })

    local rollPatrol6 = Entity:create()

    Entity:setTagComponent(rollPatrol6, {
        tag = "Roll patrol fighter jet 6"
    })
    Entity:setLuaComponent(rollPatrol6, {
        type = "RollPatrol",
        path = "lua/scenes/sandbox/entities/roll_patrol.out",
    })
    Entity:setMeshComponent(rollPatrol6, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/wilko.jpg",
    })
    Entity:setTransformComponent(rollPatrol6, {
        size = glm.vec3(50, 50, 50),
        position = glm.vec3(-410, 225, -750),
        yaw = 75,
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- LINE PATROL
    -- --------------------------------------------------------------------------------------------------------------

    local linePatrol1Camera = Entity:create()

    Entity:setTagComponent(linePatrol1Camera, {
        tag = "Line Patrol Camera"
    })
    Entity:setLuaComponent(linePatrol1Camera, {
        type = "LinePatrolCamera",
        path = "lua/scenes/sandbox/entities/line_patrol_camera.out",
    })
    Entity:setMeshComponent(linePatrol1Camera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(linePatrol1Camera, {
        size = glm.vec3(100, 100, 100),
    })
    Entity:setCameraComponent(linePatrol1Camera, {
        aspectRatio = Window:getAspectRatio(),
        fieldOfView = 45,
        nearClip = 0.1,
        farClip = 10000,
    })

    local linePatrol1 = Entity:create()

    Entity:setTagComponent(linePatrol1, {
        tag = "Line patrol fighter jet 1"
    })
    Entity:setLuaComponent(linePatrol1, {
        type = "LinePatrol",
        path = "lua/scenes/sandbox/entities/line_patrol.out",
    })
    Entity:setMeshComponent(linePatrol1, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/krulspeld4.jpg",
    })
    Entity:setTransformComponent(linePatrol1, {
        position = glm.vec3(0, 50, 0),
        size = glm.vec3(100, 100, 100),
        yaw = 90,
    })

    local linePatrol2 = Entity:create()

    Entity:setTagComponent(linePatrol2, {
        tag = "Line patrol fighter jet 2"
    })
    Entity:setLuaComponent(linePatrol2, {
        type = "LinePatrol",
        path = "lua/scenes/sandbox/entities/line_patrol.out",
    })
    Entity:setMeshComponent(linePatrol2, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/krulspeld5.jpg",
    })
    Entity:setTransformComponent(linePatrol2, {
        position = glm.vec3(0, 50, -100),
        size = glm.vec3(100, 100, 100),
        yaw = 90,
    })

    -- --------------------------------------------------------------------------------------------------------------
    -- PLAYER
    -- --------------------------------------------------------------------------------------------------------------

    local playerCamera = Entity:create()

    Entity:setTagComponent(playerCamera, {
        tag = "Camera"
    })
    Entity:setLuaComponent(playerCamera, {
        type = "Camera",
        path = "lua/scenes/sandbox/entities/camera.out",
    })
    Entity:setMeshComponent(playerCamera, {
        modelPath = "models/camera/11673_camera_v1_L3.obj",
        texturesDirectoryPath = "models/camera",
    })
    Entity:setTransformComponent(playerCamera, {
        position = glm.vec3(0, 0, 0),
        size = glm.vec3(25, 25, 25),
    })
    Entity:setCameraComponent(playerCamera, {
        aspectRatio = Window:getAspectRatio(),
        fieldOfView = 45,
        nearClip = 0.1,
        farClip = 10000,
    })

    local player = Entity:create()

    Entity:setTagComponent(player, {
        tag = "Player"
    })
    Entity:setLuaComponent(player, {
        type = "Player",
        path = "lua/scenes/sandbox/entities/player.out",
    })
    Entity:setMeshComponent(player, {
        modelPath = "models/fighter/fighter.obj",
        textureAtlasPath = "models/fighter/krulspeld1.jpg",
    })
    Entity:setTransformComponent(player, {
        size = glm.vec3(25, 25, 25),
    })
end

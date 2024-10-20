#pragma once

// --------------------------------------------------------------------------------------------------------------
// Standard library
// --------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// --------------------------------------------------------------------------------------------------------------
// 3rd-party
// --------------------------------------------------------------------------------------------------------------

// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Lua
#include <lua.hpp>

// Vulkan
#include <vulkan/vulkan.h>

// --------------------------------------------------------------------------------------------------------------
// App
// --------------------------------------------------------------------------------------------------------------

#include "system/Assert.h"
#include "system/Environment.h"
#include "system/Error.h"
#include "system/ErrorSignal.h"
#include "system/Log.h"
#include "utils/utils.h"
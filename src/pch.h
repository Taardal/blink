#pragma once

// Standard library
#include <array>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// 3rd-party
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <lua.hpp>

#include <vulkan/vulkan.h>

// App
#include "system/Environment.h"
#include "system/Log.h"
#include "system/Assert.h"
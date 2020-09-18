#!/bin/bash

cat <<"EOF"


BBBBBBBBBBBBBBBBB   lllllll   iiii                   kkkkkkkk
B::::::::::::::::B  l:::::l  i::::i                  k::::::k
B::::::BBBBBB:::::B l:::::l   iiii                   k::::::k
BB:::::B     B:::::Bl:::::l                          k::::::k
  B::::B     B:::::B l::::l iiiiiiinnnn  nnnnnnnn     k:::::k    kkkkkkk
  B::::B     B:::::B l::::l i:::::in:::nn::::::::nn   k:::::k   k:::::k
  B::::BBBBBB:::::B  l::::l  i::::in::::::::::::::nn  k:::::k  k:::::k
  B:::::::::::::BB   l::::l  i::::inn:::::::::::::::n k:::::k k:::::k
  B::::BBBBBB:::::B  l::::l  i::::i  n:::::nnnn:::::n k::::::k:::::k
  B::::B     B:::::B l::::l  i::::i  n::::n    n::::n k:::::::::::k
  B::::B     B:::::B l::::l  i::::i  n::::n    n::::n k:::::::::::k
  B::::B     B:::::B l::::l  i::::i  n::::n    n::::n k::::::k:::::k
BB:::::BBBBBB::::::Bl::::::li::::::i n::::n    n::::nk::::::k k:::::k
B:::::::::::::::::B l::::::li::::::i n::::n    n::::nk::::::k  k:::::k
B::::::::::::::::B  l::::::li::::::i n::::n    n::::nk::::::k   k:::::k
BBBBBBBBBBBBBBBBB   lllllllliiiiiiii nnnnnn    nnnnnnkkkkkkkk    kkkkkkk


EOF

readonly DEBUG_BUILD_TYPE="Debug"
readonly RELEASE_BUILD_TYPE="Release"

echo "
#####################
#   Setting up...   #
#####################
"
if [[ -n "${1}" ]]; then
  buildType="${1}"
else
  buildType="${DEBUG_BUILD_TYPE}"
fi

if [[ ${buildType} != "${DEBUG_BUILD_TYPE}" && ${buildType} != "${RELEASE_BUILD_TYPE}" ]]; then
  echo "Invalid build type [${buildType}], expected [${DEBUG_BUILD_TYPE},${RELEASE_BUILD_TYPE}]"
  exit 1
fi

echo "-- Using build type [${buildType}]"

buildFolder="cmake-build-$(echo ${buildType} | awk '{print tolower($0)}')"
echo "-- Using build folder [${buildFolder}]"

resourcesFolder="res"
echo "-- Using resources folder [${resourcesFolder}]"

buildResourcesFolder="${buildFolder}/${resourcesFolder}"
echo "-- Using build resources folder [${buildResourcesFolder}]"

cd ..
echo "-- Working from directory [$(pwd)]"

echo "
#############################
#   Updating resources...   #
#############################
"
rm -rf "${buildResourcesFolder}"
mkdir "${buildResourcesFolder}"
echo "-- Cleaned resources from [$(pwd)/${buildResourcesFolder}]"

cp -r "${resourcesFolder}" "${buildFolder}"
echo "-- Copied resources from [$(pwd)/${resourcesFolder}] to [$(pwd)/${buildFolder}]"

echo "
##########################
#   Running program...   #
##########################
"
cd "${buildFolder}" || exit
./blink "$(pwd)/${resourcesFolder}/main.lua"

echo "
############
#   DONE   #
############
"
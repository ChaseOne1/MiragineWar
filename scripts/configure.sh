echo 'initializing the submodules...'
ls

git submodule update --init --depth 1 3rdlibs/entt
git submodule update --init --depth 1 3rdlibs/mathfu
git submodule update --init --depth 1 3rdlibs/MiragineWarProtocol
git submodule update --init --depth 1 3rdlibs/mirrow
git submodule update --init --depth 1 3rdlibs/SDL3
git submodule update --init --depth 1 3rdlibs/SDL3_image
git submodule update --init --depth 1 3rdlibs/SDL3_ttf
git submodule update --init --depth 1 3rdlibs/SLikeNet
git submodule update --init --depth 1 3rdlibs/sol2
git submodule update --init --depth 1 3rdlibs/toml++

cd 3rdlibs

cd mathfu
git submodule update --init --depth 1 dependencies/fplutil
git submodule update --init --depth 1 dependencies/vectorial

cd ../MiragineWarProtocol
git submodule update --init --depth 1 3rdlibs/protobuf

cd ../SDL3_image
git submodule update --init --depth 1 external/libpng
git submodule update --init --depth 1 external/jpeg
git submodule update --init --depth 1 external/zlib

cd ../SDL3_ttf
git submodule update --init --depth 1 external/freetype
git submodule update --init --depth 1 external/harfbuzz

cd ../../
echo 'initialize done'
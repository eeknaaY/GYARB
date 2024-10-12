
#pragma once

#include <vector>
#include "FastNoiseLite.h"

struct biome{
    FastNoiseLite noise;
    int waterLevel;
    int averageHeightValue;
    int heightOffsetValue;
};

class Biomes{
    public:
        enum biomes{
            Forest
        };

        static std::vector<biome> _biomes;

        static biome getBiome(biomes biomeVal){
            if (_biomes.size() == 0){
                buildBiomeVector();
                return _biomes[biomeVal];
            } else {
                return _biomes[biomeVal];
            }
        }

        static void buildBiomeVector(){
            _biomes.push_back(biome());
        }
};


#pragma once

#include <vector>
#include "FastNoiseLite.h"

struct Biome{
    FastNoiseLite noise;
    int waterLevel = 0;
    int averageHeightValue = 0;
    int heightOffsetValue = 0;

    Biome(FastNoiseLite noise, int waterLevel, int averageHeightValue, int heightOffsetValue){
        this->noise = noise;
        this->waterLevel = waterLevel;
        this->averageHeightValue = averageHeightValue;
        this->heightOffsetValue = heightOffsetValue;
    }
};

class Biomes{
    public:
        enum biomes{
            Forest
        };

        static Biome getBiome(biomes biomeVal){
            if (biomesVector.size() == 0){
                buildBiomeVector();
                return biomesVector[biomeVal];
            } else {
                return biomesVector[biomeVal];
            }
        }

        static void buildBiomeVector(){
            FastNoiseLite plainNoise;
            plainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            plainNoise.SetFractalType(FastNoiseLite::FractalType_FBm);
            plainNoise.SetFrequency(0.002);
            plainNoise.SetFractalOctaves(3);
            plainNoise.SetFractalWeightedStrength(8);

            { // Forest
                biomesVector.push_back(Biome(plainNoise, 8, 16, 16));
            }

            { // ?

            }
        }
    private:
        inline static std::vector<Biome> biomesVector = std::vector<Biome>{};
};
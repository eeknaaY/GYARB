
#pragma once

#include <vector>
#include "FastNoiseLite.h"

enum class Biomes{
    Forest,
    Mountain
};

struct Biome{
    Biomes biome;
    FastNoiseLite noise;
    int waterLevel = 0;
    int averageHeightValue = 0;
    int heightOffsetValue = 0;

    Biome(FastNoiseLite noise, int waterLevel, int averageHeightValue, int heightOffsetValue){
        this->biome = biome;
        this->noise = noise;
        this->waterLevel = waterLevel;
        this->averageHeightValue = averageHeightValue;
        this->heightOffsetValue = heightOffsetValue;
    }

    virtual float getNoiseValue(float x, float z) {return 0;};   
};

struct ForestBiome : Biome{
    using Biome::Biome;

    float getNoiseValue(float x, float z) {
        return noise.GetNoise(x, z);
    }   
};

struct MountainBiome : Biome{
    using Biome::Biome;

    float getNoiseValue(float x, float z) {
        float noiseValue = noise.GetNoise(x, z);

        if (noiseValue > 0.55){
            noiseValue = pow(3, noiseValue + 0.3) - 2;
        }

        return noiseValue;
    } 
};


class BiomeHandler{
    public:
        static Biome* getBiome(Biomes biomeVal){
            int index = static_cast<int>(biomeVal);

            if (biomesVector.size() == 0){
                buildBiomeVector();
                return biomesVector[index];
            } else {
                return biomesVector[index];
            }
        }

        static void buildBiomeVector(){
            FastNoiseLite noise;
            noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            noise.SetFractalType(FastNoiseLite::FractalType_FBm);
            noise.SetFrequency(0.002);
            noise.SetFractalOctaves(3);
            noise.SetFractalWeightedStrength(8);

            { // Forest
                int waterLevel = 8;
                int averageHeight = 16;
                int averageOffset = 16;

                biomesVector.push_back(new ForestBiome(noise, waterLevel, averageHeight, averageOffset));
            }

            { // Mountain
                int waterLevel = 8;
                int averageHeight = 16;
                int averageOffset = 16;

                noise.SetFrequency(0.001);
                biomesVector.push_back(new MountainBiome(noise, waterLevel, averageHeight, averageOffset));
            }
        }
    private:
        inline static std::vector<Biome*> biomesVector = std::vector<Biome*>{};
};
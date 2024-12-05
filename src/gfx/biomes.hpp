
#pragma once

#include <vector>
#include "FastNoiseLite.h"
#include <limits>
#include "glm/glm.hpp"

enum class Biomes{
    Forest,
    Mountain
};

struct Biome{
    FastNoiseLite noise;
    int waterLevel = 0;
    int averageHeightValue = 0;
    int heightOffsetValue = 0;
    float treeProbability = 0;

    Biome(FastNoiseLite noise, int waterLevel, int averageHeightValue, int heightOffsetValue, float treeProbability){
        this->noise = noise;
        this->waterLevel = waterLevel;
        this->averageHeightValue = averageHeightValue;
        this->heightOffsetValue = heightOffsetValue;
        this->treeProbability = treeProbability;
    }

    virtual int getHeightValue(float x, float z) {return 0;};   
};

struct ForestBiome : Biome{
    using Biome::Biome;

    int getHeightValue(float x, float z) {
        return this->averageHeightValue + noise.GetNoise(x, z) * this->heightOffsetValue;
    }   
};

struct MountainBiome : Biome{
    using Biome::Biome;

    // float getHeightValue(float x, float z) {
    //     float noiseValue = noise.GetNoise(x, z);

    //     if (noiseValue > 0.55){
    //         noiseValue = pow(3, noiseValue + 0.3) - 2;
    //     }
        
    //     return noiseValue;
    // }

    int getHeightValue(float x, float z) {
        return this->averageHeightValue + noise.GetNoise(x, z) * this->heightOffsetValue;
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

        static Biome* getBiome(float x, float z){
            FastNoiseLite biomeNoise;
            float downFall = biomeNoise.GetNoise(x, z) + 0.5f;
            float temperature = biomeNoise.GetNoise(x * 2, z * 2) + 0.5f;
            downFall = downFall < 0 ? 0 : downFall;
            temperature = temperature < 0 ? 0 : temperature;
            float combinedValues = downFall * temperature;
            if (combinedValues > 0.5f) return getBiome(Biomes::Forest);
            if (combinedValues <= 0.5f) return getBiome(Biomes::Mountain);

            return getBiome(Biomes::Forest);
        }

        static int getHeightValue(int x, int z){
            int chunkSize = 32;
            int chunkPosX = (int)((round(x) - (int)round(x) % chunkSize) / chunkSize);
            int chunkPosZ = (int)((round(z) - (int)round(z) % chunkSize) / chunkSize);
            if (x < 0) chunkPosX--;
            if (z < 0) chunkPosZ--;

            Biome* currentBiome = getBiome(chunkPosX, chunkPosZ);
            glm::bvec4 cornerManipulated = glm::bvec4(false);
            glm::vec4 cornerValues = glm::vec4(currentBiome->getHeightValue(chunkSize * chunkPosX, chunkSize * chunkPosZ),
                                                currentBiome->getHeightValue(chunkSize * (chunkPosX + 1), chunkSize * chunkPosZ),
                                                currentBiome->getHeightValue(chunkSize * chunkPosX, chunkSize * (chunkPosZ + 1)),
                                                currentBiome->getHeightValue(chunkSize * (chunkPosX + 1), chunkSize * (chunkPosZ + 1))
                                                );

            for (int dx = 0; dx <= 1; dx++){
                for (int dz = 0; dz <= 1; dz++){
                    Biome* targetBiome = getBiome(chunkPosX + dx, chunkPosZ + dz);
                    if (targetBiome == currentBiome) continue;
                    // Chunk is next to a different chunk so we need to interpolate.
                    cornerManipulated.x = true;

                    if (dx <= 0){
                        if (dz <= 0){
                            //if (!cornerManipulated.x) cornerValues.x = targetBiome->getHeightValue(chunkSize * chunkPosX, chunkSize * chunkPosZ);
                            //cornerManipulated.x = true;
                        } 
                        
                        if (dz >= 0) {
                            if (!cornerManipulated.z) cornerValues.z = targetBiome->getHeightValue(chunkSize * chunkPosX, chunkSize * (chunkPosZ + 1));
                            cornerManipulated.z = true;
                        }
                    }

                    if (dx >= 0){
                        if (dz <= 0){
                            if (!cornerManipulated.y) cornerValues.y = targetBiome->getHeightValue(chunkSize * (chunkPosX + 1), chunkSize * chunkPosZ);
                            cornerManipulated.y = true;
                        } 
                        
                        if (dz >= 0) {
                            if (!cornerManipulated.w) cornerValues.w = targetBiome->getHeightValue(chunkSize * (chunkPosX + 1), chunkSize * (chunkPosZ + 1));
                            cornerManipulated.w = true;
                        }
                    }
                    
                }
            }

            for (int i = 0; i < 4; i++){
                if (cornerManipulated[i] == true) return bilinearInterpolation(glm::vec2(x, z), cornerValues);
            }

            return currentBiome->getHeightValue(x, z);
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

                biomesVector.push_back(new ForestBiome(noise, waterLevel, averageHeight, averageOffset, 0.02f));
            }

            { // Mountain
                int waterLevel = 8;
                int averageHeight = 50;
                int averageOffset = 45;

                //noise.SetFrequency(0.001);
                biomesVector.push_back(new MountainBiome(noise, waterLevel, averageHeight, averageOffset, 0.0f));
            }
        }
    private:
        inline static std::vector<Biome*> biomesVector = std::vector<Biome*>{};

        int static bilinearInterpolation(glm::vec2 position, glm::vec4 cornerValues){
            int chunkSize = 32;
            int chunkPosX = (int)((round(position.x) - (int)round(position.x) % chunkSize) / chunkSize);
            int chunkPosZ = (int)((round(position.y) - (int)round(position.y) % chunkSize) / chunkSize);
            if (position.x < 0) chunkPosX--;
            if (position.y < 0) chunkPosZ--;
            
            position.x -= 32 * chunkPosX;
            position.y -= 32 * chunkPosZ;

            int denom = chunkSize * chunkSize;
            float value = 1.0 / denom * (
                cornerValues.x * (32 - position.x) * (32 - position.y) +
                cornerValues.y * position.x * (32 - position.y) +
                cornerValues.z * (32 - position.x) * position.y +
                cornerValues.w * position.x * position.y
            );

            return value;
        }
};
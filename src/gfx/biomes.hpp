
#pragma once

#include <vector>
#include "FastNoiseLite.h"
#include <limits>
#include "glm/glm.hpp"
#include <ctime>
#include <iostream>

enum class Biomes{
    Forest,
    Plains
};

struct Biome{
    FastNoiseLite noise;
    FastNoiseLite waterNoise;
    int waterLevel = 0;
    int averageHeightValue = 0;
    int heightOffsetValue = 0;
    float treeProbability = 0;

    Biome(FastNoiseLite noise, int waterLevel, int averageHeightValue, int heightOffsetValue, float treeProbability, FastNoiseLite waterNoise = FastNoiseLite()){
        this->noise = noise;
        this->waterNoise = waterNoise;
        this->waterLevel = waterLevel;
        this->averageHeightValue = averageHeightValue;
        this->heightOffsetValue = heightOffsetValue;
        this->treeProbability = treeProbability;
    }

    virtual int getBlockValue(int globalY, int globalMaxHeight) {return 0;}
    virtual int getHeightValue(float x, float z) {return 0;}
    
    bool shouldGenerateTree(int globalY, int globalMaxHeight){
        int yChunk = (int)((globalY - globalY % 32) / 32.f);

        return std::rand() % 100 < treeProbability * 100 && (globalY) > waterLevel && globalY == globalMaxHeight;
    }
};

struct ForestBiome : Biome{
    using Biome::Biome;

    int getHeightValue(float x, float z) {
        return this->averageHeightValue + noise.GetNoise(x, z) * this->heightOffsetValue;
    }  

    int getBlockValue(int globalY, int globalMaxHeight){
        int yChunk = (int)((globalY - globalY % 32) / 32.f);
        int localY = globalY % 32;
        int localMaxHeight = globalMaxHeight - 32 * yChunk;

        if (globalY == 0) return 2;
        if (localY > localMaxHeight && globalY < waterLevel) return 17; // Water
        if (localY > localMaxHeight) return 0; // Air

        if (localY > (localMaxHeight - 1)){ // Grass
            if (globalY < (waterLevel - 1)) return 3; // Make dirt under water
            return 1; // Grass
        }

        if (localY > (localMaxHeight - 2)) return 3; // Dirt
        return 2; // Stone
    }
};

struct PlainsBiome : Biome{
    using Biome::Biome;

    int getHeightValue(float x, float z) {
        float waterN = waterNoise.GetNoise(x, z) + 0.5 < 0 ? 0 : waterNoise.GetNoise(x, z) + 0.5;
        float normalN = noise.GetNoise(x, z);
        return this->averageHeightValue + normalN * this->heightOffsetValue - waterN * 40;
    }

    int getBlockValue(int globalY, int globalMaxHeight){
        int yChunk = (int)((globalY - globalY % 32) / 32.f);
        int localY = globalY % 32;
        int localMaxHeight = globalMaxHeight - 32 * yChunk;

        if (globalY == 0) return 2;
        if (localY > localMaxHeight && globalY < waterLevel) return 17; // Water
        if (localY > localMaxHeight) return 0; // Air

        if (localY > (localMaxHeight - 1)){ // Grass
            if (globalY < (waterLevel - 1)) return 3; // Make dirt under water
            return 1; // Grass
        }

        if (localY > (localMaxHeight - 2)) return 3; // Dirt
        return 2; // Stone
    }
};

class BiomeHandler{
    public:
        static Biome* getBiome(Biomes biomeVal){
            int index = (int)biomeVal;

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
            if (combinedValues <= 0.5f) return getBiome(Biomes::Plains);

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

            for (int dx = -1; dx <= 1; dx++){
                for (int dz = -1; dz <= 1; dz++){
                    Biome* targetBiome = getBiome(chunkPosX + dx, chunkPosZ + dz);
                    if (targetBiome == currentBiome) continue;
                    
                    // Chunk is next to a different chunk so we need to interpolate.

                    if (dx <= 0){
                        if (dz <= 0){
                            if (!cornerManipulated.x){
                                cornerValues.x += targetBiome->getHeightValue(chunkSize * chunkPosX, chunkSize * chunkPosZ);
                                cornerValues.x /= 2;
                            } 
                            cornerManipulated.x = true;
                        } 
                        
                        if (dz >= 0) {
                            if (!cornerManipulated.z){
                                cornerValues.z += targetBiome->getHeightValue(chunkSize * chunkPosX, chunkSize * (chunkPosZ + 1));
                                cornerValues.z /= 2;
                            } 
                            cornerManipulated.z = true;
                        }
                    }

                    if (dx >= 0){
                        if (dz <= 0){
                            if (!cornerManipulated.y){
                                cornerValues.y += targetBiome->getHeightValue(chunkSize * (chunkPosX + 1), chunkSize * chunkPosZ);
                                cornerValues.y /= 2;
                            } 
                            cornerManipulated.y = true;
                        } 
                        
                        if (dz >= 0) {
                            if (!cornerManipulated.w){
                                cornerValues.w += targetBiome->getHeightValue(chunkSize * (chunkPosX + 1), chunkSize * (chunkPosZ + 1));
                                cornerValues.w /= 2;
                            } 
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
            noise.SetSeed(0);
            noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
            noise.SetFractalType(FastNoiseLite::FractalType_FBm);
            noise.SetFrequency(0.002);
            noise.SetFractalOctaves(3);
            noise.SetFractalWeightedStrength(8);

            { // Forest
                int waterLevel = 8;
                int averageHeight = 20;
                int averageOffset = 16;

                biomesVector.push_back(new ForestBiome(noise, waterLevel, averageHeight, averageOffset, 0.02f));
            }

            { // Plains
                int waterLevel = 8;
                int averageHeight = 15;
                int averageOffset = 10;

                FastNoiseLite waterNoise;
                waterNoise.SetSeed(0);
                waterNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
                waterNoise.SetCellularReturnType(FastNoiseLite::CellularReturnType::CellularReturnType_Distance);
                waterNoise.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq);
                waterNoise.SetFrequency(0.02);

                biomesVector.push_back(new PlainsBiome(noise, waterLevel, averageHeight, averageOffset, 0.0f, waterNoise));
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
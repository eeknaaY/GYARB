#pragma once

#include <iostream>
#include "glad/glad.h"
#include <vector>
#include "stb_image.h"

class Textures{
    // Yes I know these are static, I didn't ask.
    inline static const std::string faces[] = {
        "C:/Users/Johannes/Desktop/GYARB/src/textures/right.png",
        "C:/Users/Johannes/Desktop/GYARB/src/textures/left.png",
        "C:/Users/Johannes/Desktop/GYARB/src/textures/top.png",
        "C:/Users/Johannes/Desktop/GYARB/src/textures/bottom.png",
        "C:/Users/Johannes/Desktop/GYARB/src/textures/front.png",
        "C:/Users/Johannes/Desktop/GYARB/src/textures/back.png"
    };


    public:
        static unsigned int loadCubemap(){
            unsigned int textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 

            int width, height, nrChannels;
            for (unsigned int i = 0; i < 6; i++)
            {
                unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
                if (data)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                    );
                    stbi_image_free(data);
                }
                else
                {
                    std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
                    stbi_image_free(data);
                }
            }

            return textureID;
        } 


        static unsigned int getTextureIndex(){
            static unsigned int textureAtlas;
            static bool hasInitialized;

            // Don't know if this is needed, but I'll have it here for safety
            if (hasInitialized){
                return textureAtlas;
            }

            hasInitialized = true;

            glGenTextures(1, &textureAtlas);
            glBindTexture(GL_TEXTURE_2D, textureAtlas);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            int textureWidth, textureHeight, nrChannels;
            unsigned char *data = stbi_load("C:/Users/Johannes/Desktop/GYARB/src/textures/TextureAtlas.png", &textureWidth, &textureHeight, &nrChannels, 0);

            if (data)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            }
            else
            {
                std::cout << "Failed to load texture" << std::endl;
            }

            stbi_image_free(data);
            return textureAtlas;
        };
};
#pragma once

#include <iostream>
#include "glad/glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Textures{
    public:
        Textures();
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
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            }
            else
            {
                std::cout << "Failed to load texture" << std::endl;
            }

            stbi_image_free(data);
            return textureAtlas;
        };
};
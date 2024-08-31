#include "glad/glad.h"
#include <iostream>
#include "../shaders/shaders.hpp"


// enum blockTypes{
//     dirt,
//     stone
// };

// class Textures{
//     public:
//         Textures(Shader shader);
//         unsigned int dirt, stone;
// };

// Textures::Textures(Shader shader){
//     glGenTextures(1, &dirt);
//     glBindTexture(GL_TEXTURE_2D, dirt);  
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     int textureWidth, textureHeight, nrChannels;
//     unsigned char *data = stbi_load("C:/Users/Johannes/Desktop/GYARB/src/textures/grass_block.png", &textureWidth, &textureHeight, &nrChannels, 0);

//     if (data)
//     {
//         stbi_set_flip_vertically_on_load(true); 
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
//     else
//     {
//         std::cout << "Failed to load texture" << std::endl;
//     }

//     stbi_image_free(data);

//     shader.setInt("texture2", 1);



//     glGenTextures(1, &stone);
//     glBindTexture(GL_TEXTURE_2D, stone);  
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     unsigned char *datat = stbi_load("C:/Users/Johannes/Desktop/GYARB/src/textures/stone_block.png", &textureWidth, &textureHeight, &nrChannels, 0);

//     if (datat)
//     {
//         stbi_set_flip_vertically_on_load(true); 
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, datat);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
//     else
//     {
//         std::cout << "Failed to load texture" << std::endl;
//     }

//     stbi_image_free(datat);
// }

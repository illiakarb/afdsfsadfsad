#include "tga.h"
#include <iostream>
#include <algorithm>

int main() {
    Image layer1 = ReadTGA("layer1");
    Image layer2 = ReadTGA("layer2");
    Image pattern1 = ReadTGA("pattern1");
    Image pattern2 = ReadTGA("pattern2");
    Image car = ReadTGA("car");
    Image circles = ReadTGA("circles");
    Image text = ReadTGA("text");
    Image text2 = ReadTGA("text2");
    Image layerBlue = ReadTGA("layer_blue");
    Image layerGreen = ReadTGA("layer_green");
    Image layerRed = ReadTGA("layer_red");

    WriteFile(layer1 * pattern1, "part1");

    
    WriteFile(car - layer2, "part2");


    WriteFile(screen(layer1 * pattern2, text), "part3");


    WriteFile((layer2 * circles) - pattern2, "part4");


    WriteFile(overlay(layer1, pattern1), "part5");


    Image part6 = ReadTGA("car");
    for (int i = 0; i < part6.pixelData.size(); i++) {
        part6.pixelData[i].GREEN = std::min(255, part6.pixelData[i].GREEN + 200);
    }
    WriteFile(part6, "part6");


    Image part7 = ReadTGA("car");
    for (int i = 0; i < part7.pixelData.size(); i++) {
        part7.pixelData[i].RED = std::min(255, part7.pixelData[i].RED * 4);
        part7.pixelData[i].BLUE = 0;
    }
    WriteFile(part7, "part7");


    Image part8Blue = Image(car.header, std::vector<Pixel>());
    Image part8Green = Image(car.header, std::vector<Pixel>());
    Image part8Red = Image(car.header, std::vector<Pixel>());
    for (int i = 0; i < car.pixelData.size(); i++) {
        part8Blue.pixelData.push_back(Pixel(car.pixelData[i].BLUE, car.pixelData[i].BLUE, car.pixelData[i].BLUE));
        part8Green.pixelData.push_back(Pixel(car.pixelData[i].GREEN, car.pixelData[i].GREEN, car.pixelData[i].GREEN));
        part8Red.pixelData.push_back(Pixel(car.pixelData[i].RED, car.pixelData[i].RED, car.pixelData[i].RED));
    }
    WriteFile(part8Blue, "part8_b");
    WriteFile(part8Green, "part8_g");
    WriteFile(part8Red, "part8_r");


    Image part9 = Image(layerBlue.header, std::vector<Pixel>());
    for (int i = 0; i < layerBlue.pixelData.size(); i++) {
        part9.pixelData.push_back(Pixel(layerBlue.pixelData[i].BLUE, layerGreen.pixelData[i].GREEN, layerRed.pixelData[i].RED));
    }
    WriteFile(part9, "part9");


    std::reverse(text2.pixelData.begin(), text2.pixelData.end());
    WriteFile(text2, "part10");
    return 0;
}

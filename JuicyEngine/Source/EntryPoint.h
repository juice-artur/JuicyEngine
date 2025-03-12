#pragma once


#include "Application.h"

extern JuicyEngine::Application* ::JuicyEngine::CreateApplication();

int main(int argc, char** argv)
{
    auto App = JuicyEngine::CreateApplication();
    delete App;
}

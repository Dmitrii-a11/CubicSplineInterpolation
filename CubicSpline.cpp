﻿// CubicSpline.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <thread>

#include "CubicSplineInterpolator.h"
#include "HermiteSplineInterpolator.h"
#include "InterpolationDataLoader.h"
#include "ErrorsHandler.h"

void cubicSplineInterpolation()
{
    CubicSplineInterpolator cubicSplineInterpolator;
    const std::vector<double>& x = InterpolationDataLoader::interpolationDataX();
    const std::vector<double>& y = InterpolationDataLoader::interpolationDataY();

    cubicSplineInterpolator.setErrorsHandlerDelegate([](void* object)
        {
            ErrorsHandler* eh = static_cast<ErrorsHandler*>(object);
            std::vector<std::string> errors = eh->getErrors();
            for (std::string error : errors)
                std::cout << error << '\n';
        });

    cubicSplineInterpolator.set_x(x);
    cubicSplineInterpolator.set_y(y);
    //cubicSplineInterpolator.setDerivatives(-4.0. 4.0. 2.0. 2.0);

    cubicSplineInterpolator.initialize();

    if (cubicSplineInterpolator.isInitialized())
    {
        //std::vector<double> x0{ -2.97.-2.805.-2.64.-2.475.-2.31.-2.145.-1.98.-1.815.-1.65.-1.485.-1.32.-1.155.-0.99.-0.825.-0.66.-0.495.-0.33.-0.165.0.0.165.0.33.0.495.0.66.0.825.0.99.1.155.1.32.1.485.1.65.1.815.1.98.2.145.2.31.2.475.2.64.2.805.2.97 };
        std::vector<double> x0{ -2.0,-1.5,-1.0,-0.5,0.0,0.5,1.0,1.5,2.0 };

        std::cout.imbue(std::locale("rus"));
        for (double value : x0)
            std::cout << cubicSplineInterpolator.interpolate(value) << std::endl;

    }
}

void hermiteSplineInterpolation()
{
    HermiteSplineInterpolator hermiteSplineInterpolator;
    const std::vector<double>& x = InterpolationDataLoader::interpolationDataX();
    const std::vector<double>& y = InterpolationDataLoader::interpolationDataY();

    hermiteSplineInterpolator.setErrorsHandlerDelegate([](void* object)
        {
            ErrorsHandler* eh = static_cast<ErrorsHandler*>(object);
            std::vector<std::string> errors = eh->getErrors();

            for (auto& error : errors)
                std::cout << error << '\n';
        });
    hermiteSplineInterpolator.set_x(x);
    hermiteSplineInterpolator.set_y(y);
    //hermiteSplineInterpolator.setBoundaryConditions(0.0, -3.0);
    //hermiteSplineInterpolator.setWeightsParameters(1.0, 2.5);
    hermiteSplineInterpolator.setWeightsCalculating(true);
    hermiteSplineInterpolator.initialize();

    if (hermiteSplineInterpolator.isInitialized())
    {
        std::vector<double> x0{ 7.99,8.04,8.09,8.14,8.19,8.24,8.29,8.34,8.39,8.44,8.49,8.54,8.59,8.64,8.69,8.74,8.79,8.84,8.89,8.94,8.99,9.04,9.09,9.14,9.19,9.2,9.6,10,11,12,13,14,15,16,17,18,19,20 };

        std::cout.imbue(std::locale("rus"));
        for (double value : x0)
            std::cout << value << ' ' << hermiteSplineInterpolator.interpolate(value) << std::endl;
    }
}

int main()
{
    std::wstring currentWorkingDirectory{ InterpolationDataLoader::getDefaultCurrentDirectory() };
    std::wstring fileName(L"XY.txt");
    bool isLoaded = InterpolationDataLoader::loadInterpolationData(currentWorkingDirectory + fileName);

    if (!isLoaded)
    {
        std::cout << "Interpolation data is not loaded" << std::endl;
        system("pause");
        return 0;
    }

    //cubicSplineInterpolation();
    hermiteSplineInterpolation();

    system("pause");
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент". чтобы создать файлы кода. или "Проект" > "Добавить существующий элемент". чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже. выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.

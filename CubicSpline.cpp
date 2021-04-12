// CubicSpline.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
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

    cubicSplineInterpolator.initialize();

    if (cubicSplineInterpolator.isInitialized())
    {
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
    hermiteSplineInterpolator.setWeightsCalculating(true);
    hermiteSplineInterpolator.initialize();

    if (hermiteSplineInterpolator.isInitialized())
    {
        std::vector<double> x0{ 0,0.25,0.5,0.75,1,1.25,1.5,1.75,2,2.25,2.5,2.75,3,3.25,3.5,3.75,4,4.25,4.5,4.75,5,5.25,5.5,5.75,6,6.25,6.5,6.75,7,7.25,7.5,7.75,8,8.25,8.5,8.75,9,9.25,9.5,9.75,10 };

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

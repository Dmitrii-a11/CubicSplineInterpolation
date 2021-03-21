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
        //std::vector<double> x0{ 7.99,8.04,8.09,8.14,8.19,8.24,8.29,8.34,8.39,8.44,8.49,8.54,8.59,8.64,8.69,8.74,8.79,8.84,8.89,8.94,8.99,9.04,9.09,9.14,9.19,9.2,9.6,10,11,12,13,14,15,16,17,18,19,20 };
        //std::vector<double> x0{ 0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,10.5,11,11.5,12,12.5,13,13.5,14,14.5,15 };
        std::vector<double> x0{ 0,0.25,0.5,0.75,1,1.25,1.5,1.75,2,2.25,2.5,2.75,3,3.25,3.5,3.75,4,4.25,4.5,4.75,5,5.25,5.5,5.75,6,6.25,6.5,6.75,7,7.25,7.5,7.75,8,8.25,8.5,8.75,9,9.25,9.5,9.75,10 };

        std::cout.imbue(std::locale("rus"));
        for (double value : x0)
            std::cout << value << ' ' << hermiteSplineInterpolator.interpolate(value) << std::endl;
    }
}

void testing(/*const std::wstring& fileName*/)
{
    std::wstring currentWorkingDirectory{ InterpolationDataLoader::getDefaultCurrentDirectory() };
    std::wstring fileName;
    HermiteSplineInterpolator hermiteSplineInterpolator;

    //Радиохимичесаие данные:
    {
        fileName = L"XY_1.txt";
        bool isLoaded{ InterpolationDataLoader::loadInterpolationData(currentWorkingDirectory + fileName) };
        std::vector<double> x0{ 7.99,8.04,8.09,8.14,8.19,8.24,8.29,8.34,8.39,8.44,8.49,8.54,8.59,8.64,8.69,8.74,8.79,8.84,8.89,8.94,8.99,9.04,9.09,9.14,9.19,9.2,9.6,10,11,12,13,14,15,16,17,18,19,20 };

        if (!isLoaded)
        {
            std::cout << "first test failed:\n";
            std::cout << "Interpolation data is not loaded" << std::endl;

            return;
        }

        const std::vector<double>& x = InterpolationDataLoader::interpolationDataX();
        const std::vector<double>& y = InterpolationDataLoader::interpolationDataY();

        hermiteSplineInterpolator.setErrorsHandlerDelegate([](void* object)
            {
                ErrorsHandler* eh = static_cast<ErrorsHandler*>(object);
                std::vector<std::string> errors = eh->getErrors();

                for (auto& error : errors)
                    std::cout << error << '\n';
            });
        hermiteSplineInterpolator.reset();
        hermiteSplineInterpolator.set_x(x);
        hermiteSplineInterpolator.set_y(y);
        hermiteSplineInterpolator.setWeightsCalculating(true);
        hermiteSplineInterpolator.initialize();

        std::wstring result_1(L"/Results_1.txt");
        std::ofstream fout(currentWorkingDirectory + result_1);

        if (!fout.is_open())
        {
            std::wcout << L"fail to save to file: " << result_1 << '\n';
            return;
        }
        if (hermiteSplineInterpolator.isInitialized())
        {
            std::cout.imbue(std::locale("rus"));
            for (double value : x0)
                fout << value << ' ' << hermiteSplineInterpolator.interpolate(value) << std::endl;
        }
        fout.close();
    }
    //Данные из [21]:
    {
        fileName = L"XY_2.txt";
        bool isLoaded{ InterpolationDataLoader::loadInterpolationData(currentWorkingDirectory + fileName) };
        std::vector<double> x0{ 0,0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,10.5,11,11.5,12,12.5,13,13.5,14,14.5,15};

        if (!isLoaded)
        {
            std::cout << "second test failed:\n";
            std::cout << "Interpolation data is not loaded" << std::endl;

            return;
        }

        const std::vector<double>& x = InterpolationDataLoader::interpolationDataX();
        const std::vector<double>& y = InterpolationDataLoader::interpolationDataY();

        hermiteSplineInterpolator.setErrorsHandlerDelegate([](void* object)
            {
                ErrorsHandler* eh = static_cast<ErrorsHandler*>(object);
                std::vector<std::string> errors = eh->getErrors();

                for (auto& error : errors)
                    std::cout << error << '\n';
            });
        hermiteSplineInterpolator.reset();
        hermiteSplineInterpolator.set_x(x);
        hermiteSplineInterpolator.set_y(y);
        hermiteSplineInterpolator.setWeightsCalculating(true);
        hermiteSplineInterpolator.initialize();

        std::wstring result_1(L"/Results_2.txt");
        std::ofstream fout(currentWorkingDirectory + result_1);

        if (!fout.is_open())
        {
            std::wcout << L"fail to save to file: " << result_1 << '\n';
            return;
        }
        if (hermiteSplineInterpolator.isInitialized())
        {
            std::cout.imbue(std::locale("rus"));
            for (double value : x0)
                fout << value << ' ' << hermiteSplineInterpolator.interpolate(value) << std::endl;
        }
        fout.close();
    }
    //Данные из [22]:
    {       
        fileName = L"XY_3.txt";
        bool isLoaded{ InterpolationDataLoader::loadInterpolationData(currentWorkingDirectory + fileName) };
        std::vector<double> x0{ 0,0.25,0.5,0.75,1,1.25,1.5,1.75,2,2.25,2.5,2.75,3,3.25,3.5,3.75,4,4.25,4.5,4.75,5,5.25,5.5,5.75,6,6.25,6.5,6.75,7,7.25,7.5,7.75,8,8.25,8.5,8.75,9,9.25,9.5,9.75,10};

        if (!isLoaded)
        {
            std::cout << "third test failed:\n";
            std::cout << "Interpolation data is not loaded" << std::endl;

            return;
        }

        const std::vector<double>& x = InterpolationDataLoader::interpolationDataX();
        const std::vector<double>& y = InterpolationDataLoader::interpolationDataY();

        hermiteSplineInterpolator.setErrorsHandlerDelegate([](void* object)
            {
                ErrorsHandler* eh = static_cast<ErrorsHandler*>(object);
                std::vector<std::string> errors = eh->getErrors();

                for (auto& error : errors)
                    std::cout << error << '\n';
            });
        hermiteSplineInterpolator.reset();
        hermiteSplineInterpolator.set_x(x);
        hermiteSplineInterpolator.set_y(y);
        hermiteSplineInterpolator.setWeightsCalculating(true);
        hermiteSplineInterpolator.initialize();

        std::wstring result_1(L"/Results_3.txt");
        std::ofstream fout(currentWorkingDirectory + result_1);

        if (!fout.is_open())
        {
            std::wcout << L"fail to save to file: " << result_1 << '\n';
            return;
        }
        if (hermiteSplineInterpolator.isInitialized())
        {
            std::cout.imbue(std::locale("rus"));
            for (double value : x0)
                fout << value << ' ' << hermiteSplineInterpolator.interpolate(value) << std::endl;
        }
        fout.close();
    }
}

int main()
{
    /*std::wstring currentWorkingDirectory{ InterpolationDataLoader::getDefaultCurrentDirectory() };
    std::wstring fileName(L"XY.txt");
    bool isLoaded = InterpolationDataLoader::loadInterpolationData(currentWorkingDirectory + fileName);

    if (!isLoaded)
    {
        std::cout << "Interpolation data is not loaded" << std::endl;
        system("pause");
        return 0;
    }*/

    //cubicSplineInterpolation();
    //hermiteSplineInterpolation();
    testing();

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

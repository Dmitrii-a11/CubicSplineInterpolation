#include "InterpolationDataLoader.h"

std::wstring InterpolationDataLoader::currentDirectory = L"";
std::vector<std::vector<double>> InterpolationDataLoader::interpolationData
{
    std::vector<double>(),
    std::vector<double>()
};

bool InterpolationDataLoader::loadInterpolationData(const std::wstring& fileName)
{
    std::wifstream fin(fileName);
    std::vector<double> xy(2);

    std::vector<double> xVector, yVector;

    if (fin.is_open())
    {
        while (!fin.eof())
        {
            double x, y;

            //fin >> x >> y;
            //if (!fin.fail())
            //{
            //    xVector.push_back(x);
            //    yVector.push_back(y);
            //}

            fin >> x;
            if (!fin.fail())
                xVector.push_back(x);
            fin >> y;
            if (!fin.fail())
                yVector.push_back(y);
        }

        interpolationData[0] = std::move(xVector);
        interpolationData[1] = std::move(yVector);

        fin.close();
        return true;
    }
    else
        return false;
}
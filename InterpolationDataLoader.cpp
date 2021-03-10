#include "InterpolationDataLoader.h"

#include <algorithm>
#include <direct.h>

std::wstring InterpolationDataLoader::currentDirectory = L"";
std::vector<std::vector<double>> InterpolationDataLoader::interpolationData
{
    std::vector<double>(),
    std::vector<double>()
};

std::wstring InterpolationDataLoader::getDefaultCurrentDirectory()
{
    wchar_t* buffer{ _wgetcwd(nullptr, 0) };
    std::wstring path;

    if (buffer)
        path.assign(buffer);
    else
        path.assign(L"");

    path = path + L"\\";

    return path;
}

bool InterpolationDataLoader::loadInterpolationData(const std::wstring& fileName)
{
    std::wifstream fin(getCurrenDirectory() + fileName);
    std::vector<double> xVector, yVector;

    if (fin.is_open())
    {
        while (!fin.eof())
        {
            double x, y;

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
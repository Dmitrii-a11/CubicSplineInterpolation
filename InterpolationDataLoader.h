#ifndef INTERPOLATIONDATALOADER_H
#define INTERPOLATIONDATALOADER_H

#include <vector>
#include <fstream>
#include <string>


struct InterpolationDataLoader
{
    static void setCurrentDirectory(const std::wstring& path)
    {
        currentDirectory = path;
    }

    static std::wstring getCurrenDirectory()
    {
        return currentDirectory;
    }

    static std::wstring getDefaultCurrentDirectory();

    static bool loadInterpolationData(const std::wstring& fileName);

    static const std::vector<double>& interpolationDataX()
    {
        return interpolationData[0];
    }

    static const std::vector<double>& interpolationDataY()
    {
        return interpolationData[1];
    }

private:
    static std::wstring currentDirectory;
    static std::vector<std::vector<double>> interpolationData;
};

#endif 

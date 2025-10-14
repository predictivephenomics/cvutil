/*
Copyright (C) 2025, Oak Ridge National Laboratory
Copyright (C) 2021, Anand Seethepalli and Larry York
Copyright (C) 2020, Courtesy of Noble Research Institute, LLC

File: helper_functions.cpp

Authors:
Anand Seethepalli (seethepallia@ornl.gov)
Larry York (yorklm@ornl.gov)

This file is part of Computer Vision UTILity toolkit (cvutil)

cvutil is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

cvutil is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cvutil; see the file COPYING.  If not, see
<https://www.gnu.org/licenses/>.
*/

#include "helper_functions.h"

#include <QtCore/QString>

// Helper function to split a string into numeric and non-numeric parts
static std::vector<QString> splitString(const QString& str) {
    std::vector<QString> parts;
    QString currentPart;
    bool isDigit = false;

    for (int i = 0; i < str.size(); ++i) {
        if (str[i].isDigit() != isDigit) {
            if (!currentPart.isEmpty()) {
                parts.push_back(currentPart);
                currentPart.clear();
            }
            isDigit = str[i].isDigit();
        }
        currentPart.append(str[i]);
    }
    if (!currentPart.isEmpty()) {
        parts.push_back(currentPart);
    }
    return parts;
}

// Helper function to compare two strings naturally
static bool naturalCompare(const QString& a, const QString& b) {
    std::vector<QString> partsA = splitString(a);
    std::vector<QString> partsB = splitString(b);

    auto itA = partsA.begin();
    auto itB = partsB.begin();

    while (itA != partsA.end() && itB != partsB.end()) {
        bool isDigitA = (*itA)[0].isDigit();
        bool isDigitB = (*itB)[0].isDigit();

        if (isDigitA && isDigitB) {
            // Compare numeric values
            bool okA, okB;
            int numA = itA->toInt(&okA);
            int numB = itB->toInt(&okB);

            if (okA && okB) {
                if (numA != numB) return numA < numB;
            }
        }
        else {
            // Compare as strings
            if (*itA != *itB) return *itA < *itB;
        }
        ++itA;
        ++itB;
    }
    return partsA.size() < partsB.size();
}

void natsort(QStringList& vec)
{
    std::sort(vec.begin(), vec.end(), naturalCompare);
}

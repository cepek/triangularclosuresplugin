/*  Extension of Gama-q2 for excluding outlying angular measurements in triangles
    based on Normal distribution test.
    Copyright (C) 2016 Dominik Hladík

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef TRIANGLELIST_H
#define TRIANGLELIST_H

#include "triangle.h"
#include "datalist.h"

#include <map>
#include <set>
#include <string>
#include <QString>
#include <QList>
#include <QDebug>

#include <gnu_gama/local/network.h>
#include <gnu_gama/local/observation.h>
#include <gnu_gama/obsdata.h>

namespace GNU_gama {
    namespace local {
        class LocalNetwork;
    }
}

using namespace GNU_gama::local;

class TriangleList
{
    std::set<Triangle*> _triangleList;
    LocalNetwork* _lnet {nullptr};

public:    
    TriangleList(LocalNetwork *ln) : _lnet(ln){
        addData(new DataList(_lnet));
    }
    QList<Triangle*> data();
    QList<Triangle*> onlyDirections();
    QList<Triangle*> onlyAngles();
    void addData(Triangle *triangle);
    void addData(DataList *dataList);
    void clearData();
    int count();
    Triangle* operator[](int i);    
    void setAllActive();
    void setAllPassive();

};

#endif // TRIANGLELIST_H

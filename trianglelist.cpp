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

#include "trianglelist.h"

QList<Triangle*> TriangleList::data()
{
    QList<Triangle*> list;
    for (std::set<Triangle*>::iterator
         it = _triangleList.begin(); it!=_triangleList.end(); ++it) {
        list << (*it);
    }
    return list;
}

QList<Triangle*> TriangleList::onlyDirections()
{
    QList<Triangle*> list;
    for (std::set<Triangle*>::iterator
        it = _triangleList.begin(); it!=_triangleList.end(); ++it){
        if((*it)->isOnlyDir()) list << (*it);
    }
    return list;
}

QList<Triangle*> TriangleList::onlyAngles()
{
    QList<Triangle*> list;
    for (std::set<Triangle*>::iterator
        it = _triangleList.begin(); it!=_triangleList.end(); ++it){
        if((*it)->isOnlyAngle()) list << (*it);
    }
    return list;
}

void TriangleList::addData(Triangle *triangle)
{
    _triangleList.insert(triangle);
}

void TriangleList::addData(DataList *dataList)
{   // 1) vyhledani trojuhelniku bez kontroly obousmerneho mereni
    _triangleList.clear();
    int N = dataList->count();
    for (int i=0; i<N-2; i++) {
        Data A = *(dataList->operator [](i));
        for (int j=i+1; j<N-1; j++) {
            Data B = *(dataList->operator [](j));
            for (int k=j+1; k<N; k++) {
                Data C = *(dataList->operator [](k));
                if (A.pid() == B.pid() || A.pid() == C.pid() || B.pid() == C.pid()){
                    continue;
                }
                if(A.targets().find(B.pid()) == A.targets().end() ||
                   A.targets().find(C.pid()) == A.targets().end() ||
                   B.targets().find(A.pid()) == B.targets().end() ||
                   B.targets().find(C.pid()) == B.targets().end() ||
                   C.targets().find(A.pid()) == C.targets().end() ||
                   C.targets().find(B.pid()) == C.targets().end()) {
                        continue;
                }
                addData(new Triangle(A.pid(),B.pid(),C.pid(),_lnet));
            }
        }
    }
    // 2) odstraneni nevalidnich trojuhelniku (chybi obousmerne mereni)
    std::set<Triangle*> list;
    for (std::set<Triangle*>::iterator
         it = _triangleList.begin(); it!=_triangleList.end(); ++it) {
        if((*it)->isValid()) list.insert(*it);
    }
    _triangleList = list;
}

void TriangleList::clearData()
{
    _triangleList.clear();
}

int TriangleList::count()
{
    return _triangleList.size();
}

Triangle* TriangleList::operator[](int i)
{
    int index = 0;
    for (std::set<Triangle*>::iterator it = _triangleList.begin();it!=_triangleList.end(); ++it) {
        if(i == index) return (*it);
        index++;
    }
    return nullptr;
}

void TriangleList::setAllActive()
{
    for (std::set<Triangle*>::iterator
        it = _triangleList.begin(); it!=_triangleList.end(); ++it){
        (*it)->setActive_A();
        (*it)->setActive_B();
        (*it)->setActive_C();

    }
}

void TriangleList::setAllPassive()
{
    for (std::set<Triangle*>::iterator
        it = _triangleList.begin(); it!=_triangleList.end(); ++it){
        (*it)->setPassive_A();
        (*it)->setPassive_B();
        (*it)->setPassive_C();

    }
}

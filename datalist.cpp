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

#include "datalist.h"

QList<Data*> DataList::data()
{
    return _dataList;
}

void DataList::addData()
{
    _dataList.clear();
    // 1) najdi vsechna stanoviska
    std::set<std::string> pids;
    for (auto t : _lnet->OD)
    {
        if(dynamic_cast<Direction*> (t) || dynamic_cast<Angle*> (t)){
            Observation* ob = dynamic_cast<Observation*> (t);
            pids.insert(ob->from().str());
        }
    }
    // 2) najdi cíle
    int index = 0;
    for(std::set<std::string>::iterator it = pids.begin(); it != pids.end(); it++){
        _dataList.push_back(new Data(*it));
        for (auto t : _lnet->OD){
            if(dynamic_cast<Direction*> (t)){
                Observation* ob = dynamic_cast<Observation*> (t);
                if(ob->from().str()==*it){
                    _dataList[index]->addTarget(ob->to().str());
                }
            }else if(dynamic_cast<Angle*> (t)){
                Angle* angle = dynamic_cast<Angle*> (t);
                Observation* ob = dynamic_cast<Observation*> (t);
                if(ob->from().str()==*it){
                    _dataList[index]->addTarget(angle->bs().str());
                    _dataList[index]->addTarget(angle->fs().str());

                }
            }
        }
        index++;
    }
}

void DataList::clearData()
{
    _dataList.clear();
}

int DataList::count()
{
    return _dataList.size();
}

Data* DataList::operator[](int i)
{
    if(i >= _dataList.size()) return nullptr;
    else return _dataList[i];
}


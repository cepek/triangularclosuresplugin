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

#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <string>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <cfloat>

#include <gnu_gama/local/network.h>
#include <gnu_gama/local/observation.h>
#include <gnu_gama/obsdata.h>

namespace GNU_gama {
    namespace local {
        class LocalNetwork;
    }
}

using namespace GNU_gama::local;

class Triangle
{    
    std::string _A;
    std::string _B;
    std::string _C;

    LocalNetwork* _lnet {nullptr};

    bool _isAngle_A;
    bool _isAngle_B;
    bool _isAngle_C;

    std::pair<Observation*,Observation*> _observation_A;
    std::pair<Observation*,Observation*> _observation_B;
    std::pair<Observation*,Observation*> _observation_C;

public:
    Triangle(const std::string &A,const std::string &B,const std::string &C, LocalNetwork *ln) :
        _A(A), _B(B), _C(C), _lnet(ln),
        _isAngle_A(false), _isAngle_B(false), _isAngle_C(false),
        _observation_A(std::pair<Observation*,Observation*>(nullptr,nullptr)),
        _observation_B(std::pair<Observation*,Observation*>(nullptr,nullptr)),
        _observation_C(std::pair<Observation*,Observation*>(nullptr,nullptr))
    {
        find();
    }

    std::string A();
    std::string B();
    std::string C();

    QString type_A();
    QString type_B();
    QString type_C();

    bool isAngle_A();
    bool isAngle_B();
    bool isAngle_C();

    bool isValid();
    bool isUnused();
    bool isOnlyDir();
    bool isOnlyAngle();

    void setAngle_A(Observation *obs1);
    void setAngle_B(Observation *obs1);
    void setAngle_C(Observation *obs1);

    void setDir_A(Observation *obs1,Observation *obs2);
    void setDir_B(Observation *obs1,Observation *obs2);
    void setDir_C(Observation *obs1,Observation *obs2);

    double angle_A();
    double angle_B();
    double angle_C();

    Observation* dir_A_I()
    {
        return _observation_A.first;
    }
    Observation* dir_A_II()
    {
        return _observation_A.second;
    }

    Observation* dir_B_I()
    {
        return _observation_B.first;
    }
    Observation* dir_B_II()
    {
        return _observation_B.second;
    }

    Observation* dir_C_I()
    {
        return _observation_C.first;
    }
    Observation* dir_C_II()
    {
        return _observation_C.second;
    }


    double sigma_A();
    double sigma_B();
    double sigma_C();

    bool isActive_A();
    bool isActive_B();
    bool isActive_C();

    void setActive_A();
    void setActive_B();
    void setActive_C();

    void setPassive_A();
    void setPassive_B();
    void setPassive_C();

    double closer();
    double test();

private:

    QStringList _types{"directions","angle"};
    enum Type{TypeDirections,TypeAngle};

    static double validateAngle(double angle);
    void find();

};

#endif // TRIANGLE_H

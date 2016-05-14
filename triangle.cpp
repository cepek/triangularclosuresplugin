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

/*  This file contains some statistical functions which are available here:
    http://git.savannah.gnu.org/cgit/gama.git/tree/lib/gnu_gama/statan.cpp

    Copyright (C) 1999, 2012  Ales Cepek <cepek@gnu.org>

    That file is part of the GNU Gama C++ library.
*/

#include "triangle.h"

namespace {

    /** \brief Function computes in the given point x value D(x) of
     * distributive normalized normal distribution and the value of its
     * density function.
     *
     * \param x  argument
     * \param D  cumulative distribution
     * \param f  probability density function
     */

    void NormalDistribution(double x, double& D, double& f)
    {
       const double maxd = 1e30;
       const double mind = 1e-30;
       double s;

       f=0.3989422804014327;
       if (x == 0)
       {
          D = 0.5;
          return;
       }

       bool typv = (x <= 0);
       double b = x; if (b < 0) b = -b;

       double x2 = x*x;
       f *= exp(-0.5*x2);
       double r = f/b;

       if (r <= 0)
       {
          if (typv)
             D = 0;
          else
             D = 1;
          return;
       }

       if( typv)
          r = 2.32;
       else
          r = 3.5;

       if (b - r <= 0)
       {
          double y = f*b;
          D = y;
          s = y;
          r = 3;
          for (;;)
          {
             y *= x2/r;
             D += y;
             if (D-s <= 0) break;
             s = D;
             r += 2;
          }
          if (typv)
             D = 0.5-D;
          else
             D += 0.5;

          return;
       }

       double a1 = 2;
       double a2 = 0;
       double t  = x2+3;
       double p1 = f;
       double q1 = b;
       double p2 = (t-1)*f;
       double q2 = t*b;
       r  = p1/q1;
       D  = p2/q2;
       if (!typv)
       {
          r = 1 - r;
          D = 1 - D;
       }

       do
       {
          t  += 4;
          a1 -= 8;
          a2 += a1;
          s  = a2*p1+t*p2;
          p1 = p2;
          p2 = s;
          s  = a2*q1+t*q2;
          q1 = q2;
          q2 = s;
          if (q2 > maxd)
          {
             q1 *= mind;
             q2 *= mind;
             p1 *= mind;
             p2 *= mind;
          }
          s = r;
          r = D;
          D = p2/q2;
          if (!typv)
             D = 1 - D;
       } while (std::abs(r - D) > DBL_EPSILON);

       if (s - D) return;
       if (typv) D = 0; else D = 1;
    }

    /** \brief For given probability computes critical value of normalized
     *  normal distribution N(0,1).
     *
     * \param alfa probability for which critical values of Normal
     *        distribution is computed; value of parameter alfa must be in
     *        interval (0, 1); function doesn't check the value of the
     *        parameter
     */

    double Normal(double alfa)
    {
       double a = alfa;
       if (a > 0.5)
          a = 1 - a;

       double z = sqrt(-2*log(a));
       z -= ((7.47395*z+494.877)*z+1637.72)/(((z+117.9407)*z+908.401)*z+659.935);

       double g, f;
       NormalDistribution(z, f, g);
       f = 1 - f;
       f = (f-a)/g;
       double norm_  = (((((0.75*z*z+0.875)*f+z)*z+0.5)*f/3+0.5*z)*f+1)*f+z;
       if (alfa > 0.5) norm_ = -norm_;
       return norm_;
    }

    /** \brief For the given probability and number of degrees of fredom
     * computes critical value of Student's distribution
     *
     * \param alfa probability for which critical values of Student's
     *       distribution is computed; value of parameter alfa must be
     *       in interval (0, 1); function doesn;t check the value of the
     *       parameter
     *
     * \param N degrees of freedom
     */

    float Student(float palfa, int N)
    {
       float alfa = palfa;
       if(alfa > 0.5) alfa=1.0-alfa;
       alfa *= 2;

       if(palfa == 0.5) return 0;

       if (N <= 1)
       {
          float a = M_PI/2*alfa;
          float stu_ = cos(a)/sin(a);
          if (palfa > 0.5) stu_ = -stu_;
          return stu_;
       }

       if (N <= 2)
       {
          float stu_ = sqrt(2.0/(alfa*(2.0-alfa))-2.0);
          if (palfa > 0.5) stu_ = -stu_;
          return stu_;
        }

       float r = N;
       float a = 1.0/(r-0.5);
       float b = 48.0/(a*a);
       float c = ((20700.0*a/b-98.0)*a-16.0)*a+96.36;
       float d = ((94.5/(b+c)-3.0)/b+1.0)*sqrt(M_PI/2*a)*r;
       float x = d*alfa, xx = 2.0/r;
       float y = pow(x,xx);

       if (y > a+0.05)
       {
          x = -Normal(0.5*alfa);
          y = x*x;
          if (N < 5) c += 0.3*(r-4.5)*(x+0.6);
          c = (((0.05*d*x-5.0)*x-7.0)*x-2.0)*x+b+c;
          y = (((((0.4*y+6.3)*y+36.0)*y+94.5)/c-y-3.0)/b+1.0)*x;
          y = a*y*y;
          if (y <= 0.002)
             y = 0.5*y*y+y;
          else
             y = exp(y)-1.0;
          float stu_ = sqrt(r*y);
          if (palfa > 0.5) stu_ = -stu_;
          return stu_;
       }

       y = ((1.0/(((r+6.0)/(r*y)-0.089*d-0.822)*(r+2.0)*3.0)+0.5/(r+4.0))
            *y-1.0)*(r+1.0)/(r+2.0)+1.0/y;
       float stu_ = sqrt(r*y);
       if (palfa > 0.5) stu_ = -stu_;
       return stu_;
    }

    /** \brief For the given probability and number of degrees of fredom
     * computes Grubbs's critical value
     *
     * \param alfa probability for which Grubbs's critical values
     *       is computed; value of parameter alfa must be
     *       in interval (0, 1); function doesn't check the value of the
     *       parameter
     *
     * \param N degrees of freedom
     */

    float Grubbs(float palfa, int N)
    {
        float st = Student(palfa/(2*N),N-2); // two-sided
//        float st = Student(palfa/N,N-2); // one-sided
        return (N-1)*sqrt(st*st/(N-2+st*st))/sqrt(N);
    }
}


std::string Triangle::A()
{
    return _A;
}

std::string Triangle::B()
{
    return _B;
}

std::string Triangle::C()
{
    return _C;
}

QString Triangle::type_A()
{
    if(isAngle_A()) return _types[TypeAngle];
    else return _types[TypeDirections];
}

QString Triangle::type_B()
{
    if(isAngle_B()) return _types[TypeAngle];
    else return _types[TypeDirections];
}

QString Triangle::type_C()
{
    if(isAngle_C()) return _types[TypeAngle];
    else return _types[TypeDirections];
}

bool Triangle::isAngle_A()
{
    return _isAngle_A;
}

bool Triangle::isAngle_B()
{
    return _isAngle_B;
}

bool Triangle::isAngle_C()
{
    return _isAngle_C;
}

bool Triangle::isValid()
{
    if(_observation_A.first == nullptr || _observation_A.second == nullptr ||
       _observation_B.first == nullptr || _observation_B.second == nullptr ||
       _observation_C.first == nullptr || _observation_C.second == nullptr){
        return false;
    }else return true;
}

bool Triangle::isUnused()
{
    if(!isValid()) return false;

    if(!_observation_A.first->active() || !_observation_A.second->active() ||
       !_observation_B.first->active() || !_observation_B.second->active() ||
       !_observation_C.first->active() || !_observation_C.second->active()){
        return true;
    }else return false;
}

bool Triangle::isOnlyDir()
{
    if(!_isAngle_A && !_isAngle_B && !_isAngle_C) return true;
    else return false;
}

bool Triangle::isOnlyAngle()
{
    if(_isAngle_A && _isAngle_B && _isAngle_C) return true;
    else return false;
}

void Triangle::setAngle_A(Observation *obs1)
{
    _observation_A = std::pair<Observation*,Observation*>(obs1,obs1);
    _isAngle_A = true;
}
void Triangle::setAngle_B(Observation *obs1)
{
    _observation_B = std::pair<Observation*,Observation*>(obs1,obs1);
    _isAngle_B = true;
}

void Triangle::setAngle_C(Observation *obs1)
{
    _observation_C = std::pair<Observation*,Observation*>(obs1,obs1);
    _isAngle_C = true;
}

void Triangle::setDir_A(Observation *obs1,Observation *obs2)
{
    _observation_A = std::pair<Observation*,Observation*>(obs1,obs2);
    _isAngle_A = false;
}

void Triangle::setDir_B(Observation *obs1,Observation *obs2)
{
    _observation_B = std::pair<Observation*,Observation*>(obs1,obs2);
    _isAngle_B = false;
}

void Triangle::setDir_C(Observation *obs1,Observation *obs2)
{
    _observation_C = std::pair<Observation*,Observation*>(obs1,obs2);
    _isAngle_C = false;
}

double Triangle::angle_A()
{
    if(isAngle_A()){
        return _observation_A.first->value();
    }else{
        double value1 = validateAngle(_observation_A.first->value() - _observation_A.second->value());
        double value2 = validateAngle(_observation_A.second->value() - _observation_A.first->value());
        return std::min(value1,value2);
    }
}

double Triangle::angle_B()
{
    if(isAngle_B()){
        return _observation_B.first->value();
    }else{
        double value1 = validateAngle(_observation_B.first->value() - _observation_B.second->value());
        double value2 = validateAngle(_observation_B.second->value() - _observation_B.first->value());
        return std::min(value1,value2);
    }
}

double Triangle::angle_C()
{
    if(isAngle_C()){
        return _observation_C.first->value();
    }else{
        double value1 = validateAngle(_observation_C.first->value() - _observation_C.second->value());
        double value2 = validateAngle(_observation_C.second->value() - _observation_C.first->value());
        return std::min(value1,value2);
    }
}

double Triangle::sigma_A()
{
    if(isAngle_A()){
        return _observation_A.first->stdDev()*M_PI/2000000; // mgon -> rad
    }else{
        double value1 = _observation_A.first->stdDev();
        double value2 = _observation_A.second->stdDev();
        return std::sqrt(value1*value1 + value2*value2)*M_PI/2000000; // mgon -> rad
    }
}

double Triangle::sigma_B()
{
    if(isAngle_B()){
        return _observation_B.first->stdDev()*M_PI/2000000; // mgon -> rad
    }else{
        double value1 = _observation_B.first->stdDev();
        double value2 = _observation_B.second->stdDev();
        return std::sqrt(value1*value1 + value2*value2)*M_PI/2000000; // mgon -> rad
    }
}

double Triangle::sigma_C()
{
    if(isAngle_C()){
        return _observation_C.first->stdDev()*M_PI/2000000; // mgon -> rad
    }else{
        double value1 = _observation_C.first->stdDev();
        double value2 = _observation_C.second->stdDev();
        return std::sqrt(value1*value1 + value2*value2)*M_PI/2000000; // mgon -> rad
    }
}

double Triangle::validateAngle(double angle)
{
    while(angle >= 2*M_PI) angle -= 2*M_PI;
    while(angle <  0)      angle += 2*M_PI;
    return angle;
}

void Triangle::find()
{
    std::pair<Observation*,Observation*> observation_A(nullptr,nullptr);
    std::pair<Observation*,Observation*> observation_B(nullptr,nullptr);
    std::pair<Observation*,Observation*> observation_C(nullptr,nullptr);

    for(auto t : _lnet->OD)
    {
        if(dynamic_cast<Direction*> (t)){
            Observation* ob = dynamic_cast<Observation*> (t);
            if(A()==ob->from().str() && B()==ob->to().str()){ // A - B
                observation_A = std::pair<Observation*,Observation*>(ob,observation_A.second);
                _isAngle_A = false;
            }else if(A()==ob->from().str() && C()==ob->to().str()){ // A - C
                observation_A = std::pair<Observation*,Observation*>(observation_A.first,ob);
                _isAngle_A = false;
            }else if(B()==ob->from().str() && A()==ob->to().str()){ // B - A
                observation_B = std::pair<Observation*,Observation*>(ob,observation_B.second);
                _isAngle_B = false;
            }else if(B()==ob->from().str() && C()==ob->to().str()){ // B - C
                observation_B = std::pair<Observation*,Observation*>(observation_B.first,ob);
                _isAngle_B = false;
            }else if(C()==ob->from().str() && A()==ob->to().str()){ // C - A
                observation_C = std::pair<Observation*,Observation*>(ob,observation_C.second);
                _isAngle_C = false;
            }else if(C()==ob->from().str() && B()==ob->to().str()){ // C - B
                observation_C = std::pair<Observation*,Observation*>(observation_C.first,ob);
                _isAngle_C = false;
            }
        }else if(dynamic_cast<Angle*> (t)){
            Angle* angle = dynamic_cast<Angle*> (t);
            if(A()==angle->from().str() &&
              (B()==angle->bs().str() || B()==angle->fs().str()) &&
              (C()==angle->bs().str() || C()==angle->fs().str())){ // A
                observation_A = std::pair<Observation*,Observation*>(angle,angle);
                _isAngle_A = true;
            }else if(B()==angle->from().str() &&
                    (C()==angle->bs().str() || C()==angle->fs().str()) &&
                    (A()==angle->bs().str() || A()==angle->fs().str())){ // B
                observation_B = std::pair<Observation*,Observation*>(angle,angle);
                _isAngle_B = true;
            }else if(C()==angle->from().str() &&
                    (B()==angle->bs().str() || B()==angle->fs().str()) &&
                    (A()==angle->bs().str() || A()==angle->fs().str())){ // C
                observation_C = std::pair<Observation*,Observation*>(angle,angle);
                _isAngle_C = true;
            }
        }
    }
    _observation_A = observation_A;
    _observation_B = observation_B;
    _observation_C = observation_C;
}

bool Triangle::isActive_A()
{
    if(_observation_A.first->active() && _observation_A.second->active()) return true;
    else return false;
}
bool Triangle::isActive_B()
{
    if(_observation_B.first->active() && _observation_B.second->active()) return true;
    else return false;
}
bool Triangle::isActive_C()
{
    if(_observation_C.first->active() && _observation_C.second->active()) return true;
    else return false;
}

void Triangle::setActive_A()
{
    _observation_A.first->set_active();
    _observation_A.second->set_active();
}
void Triangle::setActive_B()
{
    _observation_B.first->set_active();
    _observation_B.second->set_active();
}
void Triangle::setActive_C()
{
    _observation_C.first->set_active();
    _observation_C.second->set_active();
}

void Triangle::setPassive_A()
{
    _observation_A.first->set_passive();
    _observation_A.second->set_passive();
}
void Triangle::setPassive_B()
{
    _observation_B.first->set_passive();
    _observation_B.second->set_passive();
}
void Triangle::setPassive_C()
{
    _observation_C.first->set_passive();
    _observation_C.second->set_passive();
}

double Triangle::closer()
{ // radians
    return M_PI - (angle_A() + angle_B() + angle_C());
}

double Triangle::test()
{ // radians
    double alfa = 1-_lnet->conf_pr();
    double confCoefficient = Normal(alfa/2);
    return confCoefficient*std::sqrt(sigma_A()*sigma_A()+sigma_B()*sigma_B()+sigma_C()*sigma_C());
}

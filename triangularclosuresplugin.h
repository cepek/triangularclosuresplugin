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

#ifndef TRIANGULARCLOSURESPLUGIN_H
#define TRIANGULARCLOSURESPLUGIN_H

#include <QObject>
#include <QString>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QStatusBar>
#include <QTableWidget>
#include <QCheckBox>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QPushButton>

#include "gamaq2interfaces.h"
#include "mytablewidgetitem.h"
#include "datalist.h"
#include "trianglelist.h"
#include "triangle.h"
#include "help.h"

#include "networkadjustmentpanel.h"
#include <gnu_gama/local/network.h>

namespace GNU_gama {
namespace local {
    class LocalNetwork;
}
}

using LocalNetwork = GNU_gama::local::LocalNetwork;

class QFrame;

class TriangularClosuresPlugin : public QObject, AdjustmentInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.gnu.gama.gama-q2.AdjustmentInterface/1.0")
    Q_INTERFACES(AdjustmentInterface)

public:
    TriangularClosuresPlugin();

    QString getName();
    QWidget* create(NetworkAdjustmentPanel* adjp);

private:
    QAction *actionClose, *actionReset, *actionSetActive, *actionSetPassive,
            *actionSortByQualityAsc, *actionSortByQualityDesc,
            *actionDirections, *actionAngles, *actionUnused,
            *actionStdev, *actionType, *actionTest, *actionSettings,
            *actionAbout, *actionHelp;

    QStringList headerData{"Id","Sort",
                           "A","B","C",
                           "Angle","Stdev","","",
                           "Angle","Stdev","","",
                           "Angle","Stdev","","",
                           "Closure","△U","Quality"};

    enum columns{Id,Sort,
                 PointA,PointB,PointC,
                 ValueA,SigmaA,TypeA,UseA,
                 ValueB,SigmaB,TypeB,UseB,
                 ValueC,SigmaC,TypeC,UseC,
                 Closure,Testing,Quality};

    enum vertex {VertexA,VertexB,VertexC};

    QString title = "Triangular Closures plugin";

    QColor color1 = QColor(255,255,255);
    QColor color2 = QColor(240,240,240);

    int statusBarTime = 4000;
    int precission = 5;

    double units = 200;

    NetworkAdjustmentPanel* nap {nullptr};
    QString confName;
    LocalNetwork* lnet {nullptr};
    QMainWindow* mainWindow;
    QTableWidget* tableWidget;

    TriangleList *triangleList;

    QMainWindow* getMainWindow();
    void createMenuBar();

    QTableWidget* getTable();

    void setCheckBoxState(const QList<Triangle*> &data);
    void calculate(const int &row,const QList<Triangle*> &data);
    QCheckBox* setCheckBox(const int &row, const int &column,const QColor &color);
    QCheckBox* setCheckBox(const int &row, const int &column,const QColor &color, QTableWidget* table);

    void setTableData();
    void getUnits();

    void filter();
    void filter(const int &row);
    void reset();

    QDialog* getSettings();

    QString toUnits(double value);
    QString toSmallUnits(double value);

    QMainWindow* getDirections(const int &row, const QList<Triangle*> &data, const int &type);

    static double round(const double &value, const int &decimals);
    static void degrees2dms(const double &deg, int &d, int &m, double &s);
    static double dms2degrees(const int &d,const int &m,const double &s);

private slots:
    void about();
};

#endif // TRIANGULARCLOSURESPLUGIN_H

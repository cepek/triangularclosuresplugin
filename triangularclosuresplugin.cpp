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

#include <QtPlugin>
#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QTableView>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QMessageBox>

#include "triangularclosuresplugin.h"
#include <gnu_gama/local/network.h>

#include <QGridLayout>
#include <QLabel>

#include <QDebug>

TriangularClosuresPlugin::TriangularClosuresPlugin()
{
}

QString TriangularClosuresPlugin::getName()
{
    return title;
}

QWidget* TriangularClosuresPlugin::create(NetworkAdjustmentPanel *adjp)
{

    NetworkAdjustmentPanel* nap = adjp;
    confName = nap->configurationName();
    lnet = nap->adjustment()->get_local_network();
    triangleList = new TriangleList(lnet);
    if(lnet->gons()) units = 200.0;
    else             units = 180.0;
    return getMainWindow();
}

QMainWindow* TriangularClosuresPlugin::getMainWindow()
{    
    mainWindow = new QMainWindow;
    if (mainWindow) {

        mainWindow->setWindowTitle(title + " '" + confName + "'");
        mainWindow->setCentralWidget(getTable());
        getUnits();

        mainWindow->resize(800,400);

        mainWindow->setStatusBar(new QStatusBar);
        mainWindow->statusBar()->showMessage("Ready!",statusBarTime);

        createMenuBar();
    }

    return mainWindow;
}

QTableWidget* TriangularClosuresPlugin::getTable()
{
    tableWidget = new QTableWidget;

    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setColumnCount(headerData.size());
    tableWidget->setHorizontalHeaderLabels(headerData);

    tableWidget->hideColumn(Id);
    tableWidget->hideColumn(Sort);
    tableWidget->hideColumn(Quality);

    tableWidget->hideColumn(SigmaA);
    tableWidget->hideColumn(SigmaB);
    tableWidget->hideColumn(SigmaC);

    tableWidget->hideColumn(Testing);

    setTableData();

    tableWidget->setSortingEnabled(true);
    tableWidget->sortByColumn(Quality,Qt::AscendingOrder);

    return tableWidget;
}

void TriangularClosuresPlugin::setTableData()
{
    QList<QColor> colors{color1,color1,
                         color2,color2,color2,
                         color1,color1,color1,color1,
                         color2,color2,color2,color2,
                         color1,color1,color1,color1,
                         color2,color2,color2};

    QList<Triangle*> data = triangleList->data();
    tableWidget->setRowCount(data.size());
    for(int i = 0;i < data.size();i++){

        tableWidget->setItem(i,Id, new MyTableWidgetItem);
        tableWidget->item(i,Id)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,Id)->setData(Qt::BackgroundRole,QVariant(colors[Id]));
        tableWidget->item(i,Id)->setText(QString::number(i));

        tableWidget->setItem(i,Sort, new MyTableWidgetItem);
        tableWidget->item(i,Sort)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,Sort)->setData(Qt::BackgroundRole,QVariant(colors[Sort]));
        tableWidget->item(i,Sort)->setText(QString::number(i));

        tableWidget->setItem(i,PointA, new MyTableWidgetItem);
        tableWidget->item(i,PointA)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,PointA)->setData(Qt::BackgroundRole,QVariant(colors[PointA]));
        tableWidget->item(i,PointA)->setText(QString::fromStdString(data[i]->A()));

        tableWidget->setItem(i,PointB, new MyTableWidgetItem);
        tableWidget->item(i,PointB)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,PointB)->setData(Qt::BackgroundRole,QVariant(colors[PointB]));
        tableWidget->item(i,PointB)->setText(QString::fromStdString(data[i]->B()));

        tableWidget->setItem(i,PointC, new MyTableWidgetItem);
        tableWidget->item(i,PointC)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,PointC)->setData(Qt::BackgroundRole,QVariant(colors[PointB]));
        tableWidget->item(i,PointC)->setText(QString::fromStdString(data[i]->C()));

        tableWidget->setItem(i,ValueA, new MyTableWidgetItem);
        tableWidget->item(i,ValueA)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,ValueA)->setData(Qt::BackgroundRole,QVariant(colors[ValueA]));
        tableWidget->item(i,ValueA)->setText(toUnits(data[i]->angle_A()));

        tableWidget->setItem(i,SigmaA, new MyTableWidgetItem);
        tableWidget->item(i,SigmaA)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,SigmaA)->setData(Qt::BackgroundRole,QVariant(colors[SigmaA]));
        tableWidget->item(i,SigmaA)->setText(toSmallUnits(data[i]->sigma_A()));

        tableWidget->setItem(i,TypeA, new MyTableWidgetItem);
        tableWidget->item(i,TypeA)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,TypeA)->setData(Qt::BackgroundRole,QVariant(colors[TypeA]));
        tableWidget->item(i,TypeA)->setText(data[i]->type_A());

        tableWidget->setItem(i,ValueB, new MyTableWidgetItem);
        tableWidget->item(i,ValueB)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,ValueB)->setData(Qt::BackgroundRole,QVariant(colors[ValueB]));
        tableWidget->item(i,ValueB)->setText(toUnits(data[i]->angle_B()));

        tableWidget->setItem(i,SigmaB, new MyTableWidgetItem);
        tableWidget->item(i,SigmaB)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,SigmaB)->setData(Qt::BackgroundRole,QVariant(colors[SigmaB]));
        tableWidget->item(i,SigmaB)->setText(toSmallUnits(data[i]->sigma_B()));

        tableWidget->setItem(i,TypeB, new MyTableWidgetItem);
        tableWidget->item(i,TypeB)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,TypeB)->setData(Qt::BackgroundRole,QVariant(colors[TypeB]));
        tableWidget->item(i,TypeB)->setText(data[i]->type_B());

        tableWidget->setItem(i,ValueC, new MyTableWidgetItem);
        tableWidget->item(i,ValueC)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,ValueC)->setData(Qt::BackgroundRole,QVariant(colors[ValueC]));
        tableWidget->item(i,ValueC)->setText(toUnits(data[i]->angle_C()));

        tableWidget->setItem(i,SigmaC, new MyTableWidgetItem);
        tableWidget->item(i,SigmaC)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,SigmaC)->setData(Qt::BackgroundRole,QVariant(colors[SigmaC]));
        tableWidget->item(i,SigmaC)->setText(toSmallUnits(data[i]->sigma_C()));

        tableWidget->setItem(i,TypeC, new MyTableWidgetItem);
        tableWidget->item(i,TypeC)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,TypeC)->setData(Qt::BackgroundRole,QVariant(colors[TypeC]));
        tableWidget->item(i,TypeC)->setText(data[i]->type_C());

        tableWidget->setItem(i,Closure, new MyTableWidgetItem);
        tableWidget->item(i,Closure)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,Closure)->setData(Qt::BackgroundRole,QVariant(colors[Closure]));
        tableWidget->item(i,Closure)->setText("");

        tableWidget->setItem(i,Testing, new MyTableWidgetItem);
        tableWidget->item(i,Testing)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,Testing)->setData(Qt::BackgroundRole,QVariant(colors[Testing]));
        tableWidget->item(i,Testing)->setText("");

        tableWidget->setItem(i,Quality, new MyTableWidgetItem);
        tableWidget->item(i,Quality)->setTextAlignment(Qt::AlignCenter);
        tableWidget->item(i,Quality)->setData(Qt::BackgroundRole,QVariant(colors[Quality]));
        tableWidget->item(i,Quality)->setText("");

        calculate(i,data);

        QCheckBox *checkBoxA = setCheckBox(i,UseA,colors[UseA]);
        if(data[i]->isActive_A()) checkBoxA->setChecked(true);
        else                      checkBoxA->setChecked(false);

        QCheckBox *checkBoxB = setCheckBox(i,UseB,colors[UseB]);
        if(data[i]->isActive_B()) checkBoxB->setChecked(true);
        else                      checkBoxB->setChecked(false);

        QCheckBox *checkBoxC = setCheckBox(i,UseC,colors[UseC]);
        if(data[i]->isActive_C()) checkBoxC->setChecked(true);
        else                      checkBoxC->setChecked(false);

        connect(checkBoxA,&QCheckBox::clicked,[this,data,i](bool isChecked){
            for(int j = 0;j < tableWidget->rowCount();j++) tableWidget->item(j,Sort)->setText(QString::number(j));
            tableWidget->sortByColumn(Id,Qt::AscendingOrder);

            if(data[i]->isAngle_A()){
                if(isChecked) data[i]->setActive_A();
                else          data[i]->setPassive_A();

            }else{
                getDirections(i,data,VertexA)->show();
            }

            setCheckBoxState(data);
            calculate(i,data);
            filter();
            tableWidget->sortByColumn(Sort,Qt::AscendingOrder);
        });

        connect(checkBoxB,&QCheckBox::clicked,[this,data,i](bool isChecked){
            for(int j = 0;j < tableWidget->rowCount();j++) tableWidget->item(j,Sort)->setText(QString::number(j));
            tableWidget->sortByColumn(Id,Qt::AscendingOrder);

            if(data[i]->isAngle_B()){
                if(isChecked) data[i]->setActive_B();
                else          data[i]->setPassive_B();

            }else{
                getDirections(i,data,VertexB)->show();
            }

            setCheckBoxState(data);
            calculate(i,data);
            filter();
            tableWidget->sortByColumn(Sort,Qt::AscendingOrder);
        });

        connect(checkBoxC,&QCheckBox::clicked,[this,data,i](bool isChecked){
            for(int j = 0;j < tableWidget->rowCount();j++) tableWidget->item(j,Sort)->setText(QString::number(j));
            tableWidget->sortByColumn(Id,Qt::AscendingOrder);

            if(data[i]->isAngle_C()){
                if(isChecked) data[i]->setActive_C();
                else          data[i]->setPassive_C();

            }else{
                getDirections(i,data,VertexC)->show();
            }

            setCheckBoxState(data);
            calculate(i,data);
            filter();
            tableWidget->sortByColumn(Sort,Qt::AscendingOrder);
        });
    }
}

void TriangularClosuresPlugin::setCheckBoxState(const QList<Triangle*> &data)
{
    for(int i = 0;i < data.size();i++)
    {
        calculate(i,data);

        QCheckBox* boxA = tableWidget->cellWidget(i,UseA)->findChildren<QCheckBox *>().first();
        if(data[i]->isActive_A()) boxA->setChecked(true);
        else                      boxA->setChecked(false);

        QCheckBox* boxB = tableWidget->cellWidget(i,UseB)->findChildren<QCheckBox *>().first();
        if(data[i]->isActive_B()) boxB->setChecked(true);
        else                      boxB->setChecked(false);

        QCheckBox* boxC = tableWidget->cellWidget(i,UseC)->findChildren<QCheckBox *>().first();
        if(data[i]->isActive_C()) boxC->setChecked(true);
        else                      boxC->setChecked(false);
    }
}

void TriangularClosuresPlugin::calculate(const int &row,const QList<Triangle*> &data)
{
    if(data[row]->isUnused()){
        tableWidget->item(row,Closure)->setText("");
        tableWidget->item(row,Testing)->setText("");
        tableWidget->item(row,Quality)->setText("");
    }else{
        double closer = data[row]->closer();
        double test = data[row]->test();
        double sort;
        if(closer!=0) sort = test/std::abs(closer);
        else          sort = 1e16;
        tableWidget->item(row,Closure)->setText(toSmallUnits(closer));
        tableWidget->item(row,Testing)->setText(toSmallUnits(test));
        tableWidget->item(row,Quality)->setText(QString::number(sort,'h',16));
        if(std::abs(toSmallUnits(closer).toDouble()) > toSmallUnits(test).toDouble()){
            tableWidget->item(row,Testing)->setTextColor(QColor(Qt::red));
        }else{
            tableWidget->item(row,Testing)->setTextColor(QColor(Qt::black));
        }
    }
}

QCheckBox* TriangularClosuresPlugin::setCheckBox(const int &row, const int &column,const QColor &color)
{
    QCheckBox *checkBox = new QCheckBox;
    QWidget *widget = new QWidget;
    widget->setStyleSheet("background-color: rgb(" +
                          QString::number(color.red()) + ", " +
                          QString::number(color.green())+ "," +
                          QString::number(color.blue())+ ");");
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(checkBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);
    widget->setLayout(layout);
    tableWidget->setCellWidget(row,column,widget);
    return checkBox;
}

QCheckBox* TriangularClosuresPlugin::setCheckBox(const int &row, const int &column,const QColor &color, QTableWidget* table)
{
    QCheckBox *checkBox = new QCheckBox;
    QWidget *widget = new QWidget;
    widget->setStyleSheet("background-color: rgb(" +
                          QString::number(color.red()) + ", " +
                          QString::number(color.green())+ "," +
                          QString::number(color.blue())+ ");");
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(checkBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);
    widget->setLayout(layout);
    table->setCellWidget(row,column,widget);
    return checkBox;
}

void TriangularClosuresPlugin::createMenuBar()
{
    QMenu* menuFile = new QMenu(tr("&File"),mainWindow);
    actionClose = menuFile->addAction(tr("&Close"));
    connect(actionClose, SIGNAL(triggered()),mainWindow,SLOT(close()));

    QMenu* menuSettings = new QMenu(tr("&Settings"), mainWindow);
    actionReset = menuSettings->addAction(tr("&Reset"));
    connect(actionReset, &QAction::triggered,[this](){
        reset();
        mainWindow->statusBar()->showMessage("Data were reloaded!",statusBarTime);
    });
    menuSettings->addSeparator();
    actionSetActive = menuSettings->addAction(tr("&Set All Active"));
    connect(actionSetActive, &QAction::triggered,[this](){

        for(int j = 0;j < tableWidget->rowCount();j++) tableWidget->item(j,Sort)->setText(QString::number(j));
        tableWidget->sortByColumn(Id,Qt::AscendingOrder);
        triangleList->setAllActive();
        setCheckBoxState(triangleList->data());
        filter();
        tableWidget->sortByColumn(Sort,Qt::AscendingOrder);
        mainWindow->statusBar()->showMessage("All measurements were activated!",statusBarTime);
    });
    actionSetPassive = menuSettings->addAction(tr("&Set All Passive"));
    connect(actionSetPassive, &QAction::triggered,[this](){

        for(int j = 0;j < tableWidget->rowCount();j++) tableWidget->item(j,Sort)->setText(QString::number(j));
        tableWidget->sortByColumn(Id,Qt::AscendingOrder);
        triangleList->setAllPassive();
        setCheckBoxState(triangleList->data());
        filter();
        tableWidget->sortByColumn(Sort,Qt::AscendingOrder);
        mainWindow->statusBar()->showMessage("All measurements were deactivated!",statusBarTime);
    });
    menuSettings->addSeparator();
    actionSortByQualityAsc = menuSettings->addAction(tr("&Sort By Quality Asc"));
    connect(actionSortByQualityAsc, &QAction::triggered,[this](){
        tableWidget->sortByColumn(Quality,Qt::AscendingOrder);
        mainWindow->statusBar()->showMessage("Sorting done!",statusBarTime);
    });
    actionSortByQualityDesc = menuSettings->addAction(tr("&Sort By Quality Desc"));
    connect(actionSortByQualityDesc, &QAction::triggered,[this](){
        tableWidget->sortByColumn(Quality,Qt::DescendingOrder);
        mainWindow->statusBar()->showMessage("Sorting done!",statusBarTime);
    });
    menuSettings->addSeparator();
    actionAngles = menuSettings->addAction(tr("&Angles"));
    actionAngles->setCheckable(true);
    actionAngles->setChecked(true);
    connect(actionAngles, &QAction::toggled,[this](){
        filter();
        mainWindow->statusBar()->showMessage("Filtering done!",statusBarTime);
    });
    actionDirections = menuSettings->addAction(tr("&Directions"));
    actionDirections->setCheckable(true);
    actionDirections->setChecked(true);
    connect(actionDirections, &QAction::toggled,[this](){
        filter();
        mainWindow->statusBar()->showMessage("Filtering done!",statusBarTime);
    });
    actionUnused = menuSettings->addAction(tr("&Unused"));
    actionUnused->setCheckable(true);
    actionUnused->setChecked(true);
    connect(actionUnused, &QAction::toggled,[this](){
        filter();
        mainWindow->statusBar()->showMessage("Filtering done!",statusBarTime);
    });
    menuSettings->addSeparator();    
    actionType = menuSettings->addAction(tr("&Type of Measurements"));
    actionType->setCheckable(true);
    actionType->setChecked(true);
    connect(actionType, &QAction::toggled, [this](){
        if(actionType->isChecked()){
            tableWidget->showColumn(TypeA);
            tableWidget->showColumn(TypeB);
            tableWidget->showColumn(TypeC);
            mainWindow->statusBar()->showMessage("Type of measurements were shown!",statusBarTime);
        }else{
            tableWidget->hideColumn(TypeA);
            tableWidget->hideColumn(TypeB);
            tableWidget->hideColumn(TypeC);
            mainWindow->statusBar()->showMessage("Type of measurements were hidden!",statusBarTime);
        }
    });
    actionStdev = menuSettings->addAction(tr("&Standard Deviations"));
    actionStdev->setCheckable(true);
    actionStdev->setChecked(false);
    connect(actionStdev, &QAction::toggled,[this](){
        if(actionStdev->isChecked()){
            tableWidget->showColumn(SigmaA);
            tableWidget->showColumn(SigmaB);
            tableWidget->showColumn(SigmaC);
            mainWindow->statusBar()->showMessage("Standard deviations were shown!",statusBarTime);
        }else{
            tableWidget->hideColumn(SigmaA);
            tableWidget->hideColumn(SigmaB);
            tableWidget->hideColumn(SigmaC);
            mainWindow->statusBar()->showMessage("Standard deviations were hidden!",statusBarTime);
        }
    });
    menuSettings->addSeparator();
    actionTest = menuSettings->addAction(tr("&Normal Distribution Test"));
    actionTest->setCheckable(true);
    actionTest->setChecked(false);
    connect(actionTest, &QAction::toggled, [this](){
        if(actionTest->isChecked()){
            tableWidget->showColumn(Testing);
            mainWindow->statusBar()->showMessage("Normal distribution test was shown!",statusBarTime);
        }else{
            tableWidget->hideColumn(Testing);
            mainWindow->statusBar()->showMessage("Normal distribution test was hidden!",statusBarTime);
        }
    });
    menuSettings->addSeparator();
    actionSettings = menuSettings->addAction(tr("&Settings"));
    connect(actionSettings, &QAction::triggered,[this](){
        getSettings()->show();
        mainWindow->statusBar()->showMessage("Settings shown!",statusBarTime);
    });

    QMenu* menuHelp = new QMenu(tr("&Help"),mainWindow);
    actionHelp = menuHelp->addAction(title + " Help");
    connect(actionHelp, &QAction::triggered,[this](){
        Help::get()->show();
        mainWindow->statusBar()->showMessage("Help shown!",statusBarTime);
    });
    actionAbout = menuHelp->addAction("About " + title);
    connect(actionAbout, &QAction::triggered,[this](){
        about();
        mainWindow->statusBar()->showMessage("About shown!",statusBarTime);
    });

    mainWindow->menuBar()->addMenu(menuFile);
    mainWindow->menuBar()->addMenu(menuSettings);
    mainWindow->menuBar()->addMenu(menuHelp);
}

void TriangularClosuresPlugin::about()
{
    QString version = "Alpha";
    QString year = "2016";

    QMessageBox about(mainWindow);
    about.setWindowTitle("About " + title);
    about.setTextFormat(Qt::RichText);
    about.setText(tr("<p>Extension of Gama-q2 for excluding outlying angular measurements in triangles "
                     "based on Normal distribution test.</p>") +

                  "<p><b>" + title + "&nbsp;" + version + "</b>"
                  " Copyright (C) " + year + " Dominik Hladík</p>"

                  "<p>This program is free software: you can redistribute it and/or modify "
                  "it under the terms of the GNU General Public License as published by "
                  "the Free Software Foundation, either version 3 of the License, or "
                  "(at your option) any later version.</p>"

                  "<p>This program is distributed in the hope that it will be useful, "
                  "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                  "GNU General Public License for more details.</p>"

                  "<p>You should have received a copy of the GNU General Public License "
                  " along with this program.  If not, see <a "
                  "href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/</a>.</p>"
                  );
    about.exec();
}

double TriangularClosuresPlugin::round(const double &value, const int &decimals)
{
    return std::round(std::pow(10,decimals)*value)/std::pow(10,decimals);
}

void TriangularClosuresPlugin::getUnits()
{
    if(lnet->gons()){
//        units = 200.0;
        tableWidget->horizontalHeaderItem(ValueA)->setText("Angle [gon]");
        tableWidget->horizontalHeaderItem(ValueB)->setText("Angle [gon]");
        tableWidget->horizontalHeaderItem(ValueC)->setText("Angle [gon]");
        tableWidget->horizontalHeaderItem(SigmaA)->setText("Stdev [cc]");
        tableWidget->horizontalHeaderItem(SigmaB)->setText("Stdev [cc]");
        tableWidget->horizontalHeaderItem(SigmaC)->setText("Stdev [cc]");
        tableWidget->horizontalHeaderItem(Closure)->setText("Closure [cc]");
        tableWidget->horizontalHeaderItem(Testing)->setText("△U [cc]");
    }else{
//        units = 180.0;
        tableWidget->horizontalHeaderItem(ValueA)->setText("Angle [dms]");
        tableWidget->horizontalHeaderItem(ValueB)->setText("Angle [dms]");
        tableWidget->horizontalHeaderItem(ValueC)->setText("Angle [dms]");
        tableWidget->horizontalHeaderItem(SigmaA)->setText("Stdev [s]");
        tableWidget->horizontalHeaderItem(SigmaB)->setText("Stdev [s]");
        tableWidget->horizontalHeaderItem(SigmaC)->setText("Stdev [s]");
        tableWidget->horizontalHeaderItem(Closure)->setText("Closure [s]");
        tableWidget->horizontalHeaderItem(Testing)->setText("△U [s]");
    }
}

QString TriangularClosuresPlugin::toUnits(double value)
{
    if(units==180){
        int d,m;
        double s;
        degrees2dms(value*units/M_PI,d,m,s);        
        QString text = QString::number(d) + "-";
        if(m < 10) text += "0";
        text += QString::number(m) + "-";
        if(s < 10) text += "0";
        text += QString::number(round(s,precission-4),'h',precission-4);
        return text;
    }else{
        return QString::number(round(value*units/M_PI,precission),'h',precission);
    }
}

QString TriangularClosuresPlugin::toSmallUnits(double value)
{
    if(units==180){
        double s = value*units*3600/M_PI;
        return QString::number(round(s,precission-4),'h',precission-4);
    }else{
        return QString::number(round(value*units*10000/M_PI,precission-4),'h',precission-4);
    }
}

void TriangularClosuresPlugin::degrees2dms(const double &deg, int &d, int &m, double &s)
{
    double value = std::abs(deg); // nefunguje pro zaporne uhly
    d = std::floor(value);
    value -= d;
    m = std::floor(value*60.0);
    value -= m/60.0;
    s = value*3600.0;
    if(value < 0) d *= -1;
}

double TriangularClosuresPlugin::dms2degrees(const int &d,const int &m,const double &s)
{
    return d*1.0 + m/60.0 + s/3600.0;
}

void TriangularClosuresPlugin::filter()
{
    for(int i = 0;i < tableWidget->rowCount();i++){
        filter(i);
    }
}

void TriangularClosuresPlugin::filter(const int &row)
{
    if(tableWidget->item(row,Closure)->text().isEmpty() && !actionUnused->isChecked()){
        tableWidget->hideRow(row);
        return;
    }else tableWidget->showRow(row);

    if((!actionAngles->isChecked() &&
       (tableWidget->item(row,TypeA)->text()=="angle" ||
        tableWidget->item(row,TypeB)->text()=="angle" ||
        tableWidget->item(row,TypeC)->text()=="angle")) ||
       (!actionDirections->isChecked() &&
       (tableWidget->item(row,TypeA)->text()=="directions" ||
        tableWidget->item(row,TypeB)->text()=="directions" ||
        tableWidget->item(row,TypeC)->text()=="directions"))) tableWidget->hideRow(row);
    else                                                      tableWidget->showRow(row);    
}

QDialog *TriangularClosuresPlugin::getSettings()
{
    QDialog *dialog = new QDialog;
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->setWindowTitle("Settings");

    QSpinBox *spinBoxPrimary = new QSpinBox;
    spinBoxPrimary->setValue(precission);
    spinBoxPrimary->setMinimum(4);
    spinBoxPrimary->setMaximum(12);

    QSpinBox *spinBoxSecondary = new QSpinBox;
    spinBoxSecondary->setValue(precission-4);
    spinBoxSecondary->setMinimum(spinBoxPrimary->minimum()-4);
    spinBoxSecondary->setMaximum(spinBoxPrimary->maximum()-4);

    QGridLayout *gridLayout = new QGridLayout;
    QLabel* labelPrimary = new QLabel("Observation Precission");
    gridLayout->addWidget(labelPrimary,0,0);
    gridLayout->addWidget(spinBoxPrimary,0,1);
    gridLayout->addWidget(new QLabel("Stdev Precission"),1,0);
    gridLayout->addWidget(spinBoxSecondary,1,1);

    if(!lnet->gons()){ // if the units are degrees
        spinBoxPrimary->hide();
        labelPrimary->hide();
    }

    QPushButton *pushButtonOK = new QPushButton("OK");
    QPushButton *pushButtonClose = new QPushButton("Close");

    QDialogButtonBox* box = new QDialogButtonBox;
    box->addButton(pushButtonOK,QDialogButtonBox::AcceptRole);
    box->addButton(pushButtonClose,QDialogButtonBox::RejectRole);

    QVBoxLayout *verticalLayout = new QVBoxLayout;
    verticalLayout->addLayout(gridLayout);
    verticalLayout->addItem(new QSpacerItem(1,1,QSizePolicy::Minimum,QSizePolicy::Expanding));
    verticalLayout->addWidget(box,0,Qt::AlignCenter);

    dialog->setLayout(verticalLayout);

    connect(pushButtonOK, &QPushButton::clicked, [this,dialog,spinBoxPrimary](){
        dialog->close();
        precission = spinBoxPrimary->value();

        for(int j = 0;j < tableWidget->rowCount();j++) tableWidget->item(j,Sort)->setText(QString::number(j));
        tableWidget->sortByColumn(Id,Qt::AscendingOrder);

        QList<Triangle*> data = triangleList->data();
        for(int i = 0;i < tableWidget->rowCount();i++){

            tableWidget->item(i,ValueA)->setText(toUnits(data[i]->angle_A()));
            tableWidget->item(i,SigmaA)->setText(toSmallUnits(data[i]->sigma_A()));

            tableWidget->item(i,ValueB)->setText(toUnits(data[i]->angle_B()));
            tableWidget->item(i,SigmaB)->setText(toSmallUnits(data[i]->sigma_B()));

            tableWidget->item(i,ValueC)->setText(toUnits(data[i]->angle_C()));
            tableWidget->item(i,SigmaC)->setText(toSmallUnits(data[i]->sigma_C()));

            calculate(i,data);
            filter(i);
        }
        tableWidget->sortByColumn(Sort,Qt::AscendingOrder);
    });
    connect(pushButtonClose,SIGNAL(clicked(bool)),dialog,SLOT(close()));

    connect(spinBoxPrimary,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),[spinBoxPrimary,spinBoxSecondary](){
        spinBoxSecondary->setValue(spinBoxPrimary->value()-4);
    });
    connect(spinBoxSecondary,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),[spinBoxPrimary,spinBoxSecondary](){
        spinBoxPrimary->setValue(spinBoxSecondary->value()+4);
    });

    return dialog;
}

void TriangularClosuresPlugin::reset()
{
    precission = 5;

    if(lnet->gons()) units = 200.0;
    else             units = 180.0;

    getUnits();

    // 1) load data
    triangleList->clearData();
    triangleList->addData(new DataList(lnet));

    // 2) delete data from table
    tableWidget->sortByColumn(Id,Qt::AscendingOrder);
    tableWidget->setSortingEnabled(false);
    while (tableWidget->rowCount()!=0)  tableWidget->removeRow(0);

    // 3) add data into table
    setTableData();
    tableWidget->setSortingEnabled(true);
    tableWidget->sortByColumn(Quality,Qt::AscendingOrder);
}


QMainWindow* TriangularClosuresPlugin::getDirections(const int &row,const QList<Triangle*> &data, const int& type)
{

    QStringList subHeaderData{"Observation","Target","Value []","Use"};
    QList<QColor> subColors{color2,color2,color1,color1};
    enum subTable{SubA,SubB,SubValue,SubUse};

    QTableWidget* table = new QTableWidget;
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setColumnCount(subHeaderData.size());
    table->setHorizontalHeaderLabels(subHeaderData);

    if(units==200) table->horizontalHeaderItem(SubValue)->setText("Value [gon]");
    else           table->horizontalHeaderItem(SubValue)->setText("Value [dms]");

    table->setSortingEnabled(true);
    table->sortByColumn(Quality,Qt::AscendingOrder);
    table->setRowCount(2);

    Observation* obsI, *obsII;
    if(type == VertexA){ // bod A
        obsI  = data[row]->dir_A_I();
        obsII = data[row]->dir_A_II();
    }else if(type == VertexB){ // bod B
        obsI  = data[row]->dir_B_I();
        obsII = data[row]->dir_B_II();
    }else{ // bod C
        obsI  = data[row]->dir_C_I();
        obsII = data[row]->dir_C_II();
    }

    table->setItem(0,SubA, new MyTableWidgetItem);
    table->item(0,SubA)->setTextAlignment(Qt::AlignCenter);
    table->item(0,SubA)->setData(Qt::BackgroundRole,QVariant(subColors[SubA]));
    table->item(0,SubA)->setText(QString::fromStdString(obsI->from().str()));

    table->setItem(0,SubB, new MyTableWidgetItem);
    table->item(0,SubB)->setTextAlignment(Qt::AlignCenter);
    table->item(0,SubB)->setData(Qt::BackgroundRole,QVariant(subColors[SubB]));
    table->item(0,SubB)->setText(QString::fromStdString(obsI->to().str()));

    table->setItem(0,SubValue, new MyTableWidgetItem);
    table->item(0,SubValue)->setTextAlignment(Qt::AlignCenter);
    table->item(0,SubValue)->setData(Qt::BackgroundRole,QVariant(subColors[SubValue]));
    table->item(0,SubValue)->setText(toUnits(obsI->value()));

    table->setItem(1,SubA, new MyTableWidgetItem);
    table->item(1,SubA)->setTextAlignment(Qt::AlignCenter);
    table->item(1,SubA)->setData(Qt::BackgroundRole,QVariant(subColors[SubA]));
    table->item(1,SubA)->setText(QString::fromStdString(obsI->from().str()));

    table->setItem(1,SubB, new MyTableWidgetItem);
    table->item(1,SubB)->setTextAlignment(Qt::AlignCenter);
    table->item(1,SubB)->setData(Qt::BackgroundRole,QVariant(subColors[SubB]));
    table->item(1,SubB)->setText(QString::fromStdString(obsII->to().str()));

    table->setItem(1,SubValue, new MyTableWidgetItem);
    table->item(1,SubValue)->setTextAlignment(Qt::AlignCenter);
    table->item(1,SubValue)->setData(Qt::BackgroundRole,QVariant(subColors[SubValue]));
    table->item(1,SubValue)->setText(toUnits(obsII->value()));

    QCheckBox *checkBoxI = setCheckBox(0,SubUse,subColors[SubUse],table);
    if(obsI->active()) checkBoxI->setChecked(true);
    else               checkBoxI->setChecked(false);
    connect(checkBoxI,&QCheckBox::toggled,[this,obsI,row,data](bool isChecked){
        for(int j = 0;j < tableWidget->rowCount();j++) tableWidget->item(j,Sort)->setText(QString::number(j));
        tableWidget->sortByColumn(Id,Qt::AscendingOrder);
        if(isChecked) obsI->set_active();
        else          obsI->set_passive();
        setCheckBoxState(data);
        calculate(row,data);
        filter();
        tableWidget->sortByColumn(Sort,Qt::AscendingOrder);
    });

    QCheckBox *checkBoxII = setCheckBox(1,SubUse,subColors[SubUse],table);
    if(obsII->active()) checkBoxII->setChecked(true);
    else                checkBoxII->setChecked(false);
    connect(checkBoxII,&QCheckBox::toggled,[this,obsII,row,data](bool isChecked){
        for(int j = 0;j < tableWidget->rowCount();j++) tableWidget->item(j,Sort)->setText(QString::number(j));
        tableWidget->sortByColumn(Id,Qt::AscendingOrder);
        if(isChecked) obsII->set_active();
        else          obsII->set_passive();
        setCheckBoxState(data);
        calculate(row,data);
        filter();
        tableWidget->sortByColumn(Sort,Qt::AscendingOrder);
    });

    QMainWindow* window = new QMainWindow;
    window->setWindowModality(Qt::ApplicationModal);
    window->setWindowTitle("Directions");
    window->setCentralWidget(table);
    window->resize(450,100);

    return  window;
}


/**
 *  @file    QTVcmTrajectorydialog.cpp
 *  @author  Emily Low
 *  @date    24th July 2015
 *  @version 1.0.0
 *
 *  @brief Init tab dialog layout.
 *
 *  @section DESCRIPTION
 *
 */


#include "QTVcmTrajectorydialog.h"

namespace vcmtrajdemo
{

QTVcmTrajectorydialog:: QTVcmTrajectorydialog(QWidget *parent)
     : QDialog(parent)
 {

     tabWidget = new QTabWidget;
     tabWidget->addTab(new QTVcmTrajectoryTab(), tr("VCMTraj"));

     //
     buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                      | QDialogButtonBox::Cancel);
     connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
     connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

     //
     QVBoxLayout *mainLayout = new QVBoxLayout;
     mainLayout->addWidget(tabWidget);
     mainLayout->addWidget(buttonBox);
     setLayout(mainLayout);
     setWindowTitle(tr("VCM Traj Demo"));
 }

QTVcmTrajectorydialog::~QTVcmTrajectorydialog()
{

}

}//namespace

/**
 *  @file    testVCMdialog.cpp
 *  @author  Emily Low
 *  @date    24th July 2015
 *  @version 1.0.0
 *
 *  @brief Init tab dialog layout.
 *
 *  @section DESCRIPTION
 *
 */


#include "testVCMdialog.h"

namespace QtTestVCM
{

testVCMdialog:: testVCMdialog(QWidget *parent)
     : QDialog(parent)
 {

     tabWidget = new QTabWidget;
     tabWidget->addTab(new testVCMviewerTab(), tr("VCM Test"));

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
     setWindowTitle(tr("VCM Test Emulator"));
 }

testVCMdialog::~testVCMdialog()
{

}

}//namespace

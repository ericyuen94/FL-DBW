/**
 *  @file    testVCMdialog.h
 *  @author  Emily Low
 *  @date    24th July 2015
 *  @version 1.0.0
 *
 *  @brief Init tab dialog layout.
 *
 *  @section DESCRIPTION
 *
 */

#ifndef testVCMdialog_H
#define testVCMdialog_H

#include <QtWidgets>
#include <QDialog>
#include "testVCMviewerTab.h"

namespace QtTestVCM
{

class testVCMdialog : public QDialog
{
     Q_OBJECT

 public:
     testVCMdialog(QWidget *parent = 0);
     ~testVCMdialog();

 private:
     //
     QTabWidget *tabWidget;
     QDialogButtonBox *buttonBox;

};

}
 #endif

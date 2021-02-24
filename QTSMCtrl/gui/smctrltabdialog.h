/**
 *  @file    smctrltabdialog.h
 *  @author  Emily Low
 *  @date    24th July 2015
 *  @version 1.0.0
 *
 *  @brief Init tab dialog layout.
 *
 *  @section DESCRIPTION
 *
 */

#ifndef smctrltabdialog_H
#define smctrltabdialog_H

#include <QtWidgets>
#include <QDialog>
#include "smctrlmapviewerTab.h"

namespace smctrlEmulator
{

class smctrltabdialog : public QDialog
{
     Q_OBJECT

 public:
     smctrltabdialog(QWidget *parent = 0);
     ~smctrltabdialog();

 private:
     //
     QTabWidget *tabWidget;
     QDialogButtonBox *buttonBox;

};

}
 #endif

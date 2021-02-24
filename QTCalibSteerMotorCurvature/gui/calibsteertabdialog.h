/**
 *  @file    calibsteertabdialog.h
 *  @author  Emily Low
 *  @date    24th July 2015
 *  @version 1.0.0
 *
 *  @brief Init tab dialog layout.
 *
 *  @section DESCRIPTION
 *
 */

#ifndef calibsteertabdialog_H
#define calibsteertabdialog_H

#include <QtWidgets>
#include <QDialog>
#include "calibsteermapviewerTab.h"

namespace calibsteerEmulator
{

class calibsteertabdialog : public QDialog
{
     Q_OBJECT

 public:
     calibsteertabdialog(QWidget *parent = 0);
     ~calibsteertabdialog();

 private:
     //
     QTabWidget *tabWidget;
     QDialogButtonBox *buttonBox;

};

}
 #endif

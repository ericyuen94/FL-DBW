/**
 *  @file    QTVcmTrajectorydialog.h
 *  @author  Emily Low
 *  @date    24th July 2015
 *  @version 1.0.0
 *
 *  @brief Init tab dialog layout.
 *
 *  @section DESCRIPTION
 *
 */

#ifndef QTVcmTrajectorydialog_H
#define QTVcmTrajectorydialog_H

#include <QtWidgets>
#include <QDialog>
#include "QTVcmTrajectoryTab.h"

namespace vcmtrajdemo
{

class QTVcmTrajectorydialog : public QDialog
{
     Q_OBJECT

 public:
     QTVcmTrajectorydialog(QWidget *parent = 0);
     ~QTVcmTrajectorydialog();

 private:
     //
     QTabWidget *tabWidget;
     QDialogButtonBox *buttonBox;

};

}
 #endif

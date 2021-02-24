/**
 *  @file    main.cpp
 *  @author  Emily Low
 *  @date    25th July 2015
 *  @version 1.0.0
 *
 *  @brief Main for testing stub for PLM
 *
 *  @section DESCRIPTION
 *
 */

#include "main.h"

int main(int argc, char *argv[])
{
     QApplication app(argc, argv);
  
     QtTestVCM::testVCMdialog auxtabdialog;
     return auxtabdialog.exec();

}

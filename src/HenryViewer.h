//
// C++ Interface: HenryViewer
//
// Description: 
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef HENRY_VIEWER_H
#define HENRY_VIEWER_H

#include <Qooliv/qoolivmainwin.h>
#include "augmentedreality.h"
#include <Inventor/nodes/SoSeparator.h>


class HenryViewer : public QoolMainWin
{
  Q_OBJECT
public:
  HenryViewer();
  SoSeparator* getScene() {return view->getScene();}

private:
  
};

#endif //HENRY_VIEWER_H
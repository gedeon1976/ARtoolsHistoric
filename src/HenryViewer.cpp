//
// C++ Implementation: HenryViewer
//
// Description: 
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "HenryViewer.h"

#include <HenryViewer.moc>

HenryViewer::HenryViewer() : QoolMainWin()
{
  //	start connection
  SoSeparator* root = new SoSeparator;
  view->setScene(root);
//   const char *URL = "rtsp://antlia:7070/cam3";
//   Video = AugmentedReality(root, URL);
}
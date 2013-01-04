/*
 * 
 * 	H264Server bluecherry preview video class
 * 
 * 	brief@	this class serves to show a video preview
		from a Bluecherry H.264/AVC 16 ports card 
 * 
 * 	date		January/03/2012
 * 	author		Henry Portilla
 *
 * 	
 *	Notes:	The code is based on a web example
 *
 */

#include "glwidget.h"

GLwidget::GLwidget(QWidget *parent)
    :QGLWidget(parent)
{

}
GLwidget::~GLwidget()
{

}
void GLwidget::setImage(const QImage &image)
{
    imagetoRender = image;
}
void GLwidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    
    //Set the painter to use a smooth scaling algorithm.
    painter.setRenderHint(QPainter::SmoothPixmapTransform,1);
    painter.drawImage(this->rect(),imagetoRender);
      
}

 // include extra qt moc files
 #include "glwidget.moc"



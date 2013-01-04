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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

class GLwidget :public QGLWidget
{
    Q_OBJECT
    public:
	GLwidget(QWidget* parent = NULL);
	virtual ~GLwidget();
    public Q_SLOTS:
	void setImage(const QImage &image);
    protected:
	void paintEvent(QPaintEvent*);
    private:
	
	QImage imagetoRender;
};

#endif // GLWIDGET_H

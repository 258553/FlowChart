#pragma once
#include <QGraphicsView>
#include <QMouseEvent>

#include"flowchart_global.h"
#include "flowchart_scene.h"
class FlowChartView :
    public QGraphicsView
{
	FlowChartScene* scene_;
public:
	
	FlowChartView(QWidget* parent = nullptr);
	FlowChartView(FlowChartScene* scene,QWidget* parent = nullptr);
	void SetMouseModel(FlowChartCursor _type);
protected:
	virtual void mousePressEvent(QMouseEvent* event);

	virtual void mouseReleaseEvent(QMouseEvent* event);

	virtual void mouseMoveEvent(QMouseEvent* event);

};


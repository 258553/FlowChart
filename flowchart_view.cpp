#include "flowchart_view.h"

FlowChartView::FlowChartView(QWidget* parent)
	:QGraphicsView(parent)
{
	this->centerOn(QPointF(0, 0));  // 将视图的中心移动到场景的 (0,0) 点
	//this->setAcceptDrops(true);//启用拖放功能
}

FlowChartView::FlowChartView(FlowChartScene* _scene, QWidget* parent)
	:QGraphicsView(parent)
{
	this->centerOn(QPointF(0, 0));  // 将视图的中心移动到场景的 (0,0) 点
	this->setScene(_scene);
	scene_ = _scene;
}


void FlowChartView::mousePressEvent(QMouseEvent* event) {
	QGraphicsView::mousePressEvent(event);
 }

void FlowChartView::mouseReleaseEvent(QMouseEvent* event) {
	QGraphicsView::mouseReleaseEvent(event);
 }

void FlowChartView::mouseMoveEvent(QMouseEvent* event) {
	if (scene_->drag_move) {
		setCursor(Qt::DragCopyCursor);
	}
	else {
		setCursor(Qt::ArrowCursor);
	}
	QGraphicsView::mouseMoveEvent(event);
 }


void FlowChartView::SetMouseModel(FlowChartCursor _type)
{
	QCursor cursor;
	if (_type == FlowChartCursor::ArrowCursor)
	{
		cursor = QCursor(Qt::ArrowCursor);
	}
	else if (_type == FlowChartCursor::DrawLinkCursor)
	{
		cursor = QCursor(Qt::CrossCursor);
	}
	else if (_type == FlowChartCursor::SizeAllCurSor)
	{
		cursor = QCursor(Qt::SizeAllCursor);
	}
	else if (_type == FlowChartCursor::OpenHandCursor)
	{
		cursor = QCursor(Qt::OpenHandCursor);
	}
	else if (_type == FlowChartCursor::ClosedHandCursor)
	{
		cursor = QCursor(Qt::ClosedHandCursor);
	}
	viewport()->setCursor(cursor);
}

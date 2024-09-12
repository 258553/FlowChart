#include "flowchart_link.h"

FlowChartLink::FlowChartLink(ItemType _type)
	:QGraphicsPathItem()
	,FlowChartGraphicsItem(_type)
{
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);  // 设置为可选
	this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);//开启位置位置变化通知

	this->setPen(QPen(QColor(89, 152, 209), 3));
}

FlowChartLink::~FlowChartLink()
{
}

QPointF FlowChartLink::getItemCenter()
{
	return this->path().pointAtPercent(0.5);
}

QRectF FlowChartLink::getItemRect()
{
	return QGraphicsPathItem::boundingRect();
}

bool FlowChartLink::isInItem(const QPointF& _point)
{
	return QGraphicsPathItem::boundingRect().contains(_point);
}

void FlowChartLink::setStart_End(FlowChartGraphicsItem* _start, FlowChartGraphicsItem* _end)
{
	start_ = _start;
	end_ = _end;
	
}

void FlowChartLink::setStart(QPointF _start)
{
	QPainterPath newpath;
	qDebug() << "更新start......oldpath(count)=>" << this->path().elementCount();
	for (int i = 0; i < this->path().elementCount(); i++) {
		QPainterPath::Element element = this->path().elementAt(i);
		if (i == 0) {  // 修改第二个元素的位置
			element.x = _start.x(); // 修改 x 坐标
			element.y = _start.y();  // 修改 y 坐标
		}
		if (element.isMoveTo()) {
			newpath.moveTo(element.x, element.y);
		}
		else {
			newpath.lineTo(element.x, element.y);
		}
	}

	this->setPath(newpath);
	update();
}

void FlowChartLink::setEnd(QPointF _end)
{
	QPainterPath newpath;
	for (int i = 0; i < this->path().elementCount(); i++) {
		QPainterPath::Element element = this->path().elementAt(i);
		if (i == 1) {  // 修改第二个元素的位置
			element.x = _end.x(); // 修改 x 坐标
			element.y = _end.y();  // 修改 y 坐标
		}
		if (element.isMoveTo()) {
			newpath.moveTo(element.x, element.y);
		}
		else {
			newpath.lineTo(element.x, element.y);
		}
	}
	this->setPath(newpath);
	update();
}

FlowChartGraphicsItem* FlowChartLink::getStart()
{
	return start_;
}

FlowChartGraphicsItem* FlowChartLink::getEnd()
{
	return end_;
}

void FlowChartLink::DrawItemText(QPainter* _painter)
{
	
	_painter->setFont(font_);
	QFontMetrics font_metrics = _painter->fontMetrics();
	QString text = text_;
	QRect rect_ = font_metrics.boundingRect(text);
	QRectF rect_resize = font_metrics.boundingRect(rect_, Qt::TextWordWrap, text);
	
	if (text != "") {
		_painter->setBrush(QColor(255, 255, 255));
		_painter->setPen(Qt::NoPen);     // 禁用画笔，因为只需填充背景
		QPointF center = this->getItemCenter();
		rect_resize.moveCenter(center);
		_painter->drawRect(rect_resize.adjusted(-10, -5, 20, 10));    // 绘制背景矩形
	}
	_painter->setPen(QPen(QColor(89, 152, 209),3));
	_painter->drawText(rect_resize, Qt::AlignCenter | Qt::TextWrapAnywhere, text);
}

void FlowChartLink::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	
	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	if (this->isSelected()) {
	}

	QGraphicsPathItem::paint(painter, option, widget);
	for (int i = 0; i < this->path().elementCount(); i++) {
		QPainterPath::Element ele = this->path().elementAt(i);
	}
	DrawItemText(painter);
}

QRectF FlowChartLink::boundingRect() const
{
	QRectF rect = QGraphicsPathItem::boundingRect().adjusted(-20, -20, 20, 20);
	return rect;
}

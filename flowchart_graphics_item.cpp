
#include "flowchart_graphics_item.h"
#include "flowchart_scene.h"

//**********************************************矩形**********************************************
FlowChartGraphicsRectItem::FlowChartGraphicsRectItem(ItemType _type)
	:FlowChartGraphicsItem(_type)
	, QGraphicsRectItem()
{
	Init();
}

FlowChartGraphicsRectItem::FlowChartGraphicsRectItem(QPointF point_, ItemType type_)
	:FlowChartGraphicsItem(type_)
	, QGraphicsRectItem()
{
	//x_ = point_.x() - 50;
	//y_ = point_.y() - 25;
	//qDebug() << QString("(%1，%2) [%3,%4]").arg(x_).arg(y_).arg(w_).arg(h_);
	//this->setRect(x_, y_, w_, h_);
	Init();
}

FlowChartGraphicsRectItem::FlowChartGraphicsRectItem(qreal _x, qreal _y, qreal _w, qreal _h, QGraphicsItem* parent, ItemType type_)
	:FlowChartGraphicsItem(type_)
	, QGraphicsRectItem(_x, _y, _w, _h, parent)
{
	Init();
}

FlowChartGraphicsRectItem::~FlowChartGraphicsRectItem() {

}

void FlowChartGraphicsRectItem::DrawItemText(QPainter* _painter)
{
	_painter->setFont(font_);
	QFontMetrics font_metrics = _painter->fontMetrics();
	
	QRectF rect = this->boundingRect();
	QRectF resize_rect = rect;
	QString text = text_;

	QString tremporart_text;

	resize_rect.setHeight(rect.height());
	resize_rect.setWidth(rect.width());

	tremporart_text = font_metrics.elidedText(text, Qt::ElideRight, resize_rect.width());

	_painter->setPen(text_pen_);

	/*tremporart_text.replace(text, "");
	if (!tremporart_text.isEmpty())
		resize_rect.setWidth(rect.width());*/

	resize_rect.moveCenter(rect.center());
	_painter->drawText(resize_rect, Qt::AlignCenter | Qt::TextWrapAnywhere, text);

	//_painter->restore();

}

QPointF FlowChartGraphicsRectItem::getItemCenter()
{
	return this->sceneBoundingRect().center();
}

QRectF FlowChartGraphicsRectItem::getItemRect()
{
	return this->sceneBoundingRect();
}

bool FlowChartGraphicsRectItem::isInItem(const QPointF& _point)
{
	return this->sceneBoundingRect().contains(_point);
}

void FlowChartGraphicsRectItem::Init()
{
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);  // 设置为可选
	this->setFlag(QGraphicsItem::ItemIsMovable, true);  // 设置为可移动
	this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);//开启位置位置变化通知

	this->setPen(pen_);
	this->setBrush(brush_);

}

void FlowChartGraphicsRectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	painter->save();
	painter->setPen(pen_);
	painter->setBrush(brush_);
	painter->restore();

	QGraphicsRectItem::paint(painter, option, widget);
	DrawItemText(painter);
}

QVariant FlowChartGraphicsRectItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if ((change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemPositionHasChanged) && moveing_ == false) {
		qDebug() << "更新连接线";
		FlowChartScene* scene_ = dynamic_cast<FlowChartScene*>(this->scene());
		if (scene_ != nullptr) {
			QList<QGraphicsItem*>items = scene_->items();
			for (int i = 0; i < this->linkList_start.size(); i++) {
				FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_start.at(i));
				if (link != nullptr && link->getItemType() == ItemType::Link) {
					QPainterPath path;
					path = scene_->linkPath(&items, this->getItemCenter(), link->getEnd()->getItemCenter(), this, link->getEnd());
					link->setPath(path);
				}
			}
			for (int i = 0; i < this->linkList_end.size(); i++) {
				FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_end.at(i));
				if (link != nullptr && link->getItemType() == ItemType::Link) {
					QPainterPath path;
					path = scene_->linkPath(&items, link->getStart()->getItemCenter(), this->getItemCenter(), link->getStart(), this);
					link->setPath(path);
				}
			}
		}
		scene_->update();
	}
	return QGraphicsRectItem::itemChange(change, value);
}

void FlowChartGraphicsRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	moveing_ = true;
	for (int i = 0; i < this->linkList_start.size(); i++) {
		FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_start.at(i));
		if (link != nullptr && link->getItemType() == ItemType::Link) {
			link->hide();
		}
	}
	for (int i = 0; i < this->linkList_end.size(); i++) {
		FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_end.at(i));
		if (link != nullptr && link->getItemType() == ItemType::Link) {
			link->hide();
		}
	}
	QGraphicsItem::mouseMoveEvent(event);
}

void FlowChartGraphicsRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	
	if (this->isSelected() && moveing_) {
		moveing_ = false;
		itemChange(QGraphicsRectItem::ItemPositionHasChanged, this->scenePos());
		for (int i = 0; i < this->linkList_start.size(); i++) {
			FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_start.at(i));
			if (link != nullptr && link->getItemType() == ItemType::Link) {
				link->show();
			}
		}
		for (int i = 0; i < this->linkList_end.size(); i++) {
			FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_end.at(i));
			if (link != nullptr && link->getItemType() == ItemType::Link) {
				link->show();
			}
		}
	}
	QGraphicsItem::mouseReleaseEvent(event);
}

//**********************************************判断**********************************************
FlowChartGraphicsConditionItem::FlowChartGraphicsConditionItem(ItemType type_)
	:FlowChartGraphicsItem(type_)
{
	Init();
}

FlowChartGraphicsConditionItem::FlowChartGraphicsConditionItem(QGraphicsItem* parent, ItemType type_)
	:QGraphicsPolygonItem(parent)
	,FlowChartGraphicsItem(type_)
{
	Init();
}

FlowChartGraphicsConditionItem::FlowChartGraphicsConditionItem(QPointF _top, QPointF _right, QPointF _down, QPointF _left, QGraphicsItem* parent, ItemType type_)
	:QGraphicsPolygonItem(parent)
	,FlowChartGraphicsItem(type_)
{
	diamond << _top << _right << _down << _left;
	this->setPolygon(diamond);
	Init();
}

FlowChartGraphicsConditionItem::~FlowChartGraphicsConditionItem() {

}

void FlowChartGraphicsConditionItem::DrawItemText(QPainter* _painter) {
	_painter->setFont(font_);
	QFontMetrics font_metrics = _painter->fontMetrics();

	QRectF rect = this->boundingRect();
	QRectF resize_rect = rect;
	QString text = text_;

	QString tremporart_text;

	resize_rect.setHeight(rect.height());
	resize_rect.setWidth(rect.width());

	tremporart_text = font_metrics.elidedText(text, Qt::ElideRight, resize_rect.width());

	_painter->setPen(text_pen_);

	resize_rect.moveCenter(rect.center());
	_painter->drawText(resize_rect, Qt::AlignCenter | Qt::TextWrapAnywhere, text);
}

QPointF FlowChartGraphicsConditionItem::getItemCenter()
{
	return this->sceneBoundingRect().center();
}

QRectF FlowChartGraphicsConditionItem::getItemRect()
{
	return this->sceneBoundingRect();
}

bool FlowChartGraphicsConditionItem::isInItem(const QPointF& _point)
{
	return this->sceneBoundingRect().contains(_point);	
}

void FlowChartGraphicsConditionItem::Init() {
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);  // 设置为可选
	this->setFlag(QGraphicsItem::ItemIsMovable, true);  // 设置为可移动
	this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);//开启位置位置变化通知
	this->setPen(pen_);
	this->setBrush(brush_);
}

void FlowChartGraphicsConditionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);//启用抗锯齿、文本抗锯齿

	painter->save();
	painter->restore();

	QGraphicsPolygonItem::paint(painter, option, widget);
	DrawItemText(painter);
}

QVariant FlowChartGraphicsConditionItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if ((change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemPositionHasChanged) && moveing_ == false) {
		qDebug() << "更新连接线";
		FlowChartScene* scene_ = dynamic_cast<FlowChartScene*>(this->scene());
		if (scene_ != nullptr) {
			QList<QGraphicsItem*>items = scene_->items();
			for (int i = 0; i < this->linkList_start.size(); i++) {
				FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_start.at(i));
				if (link != nullptr && link->getItemType() == ItemType::Link) {
					QPainterPath path;
					path = scene_->linkPath(&items, this->getItemCenter(), link->getEnd()->getItemCenter(), this, link->getEnd());
					link->setPath(path);
				}
			}
			for (int i = 0; i < this->linkList_end.size(); i++) {
				FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_end.at(i));
				if (link != nullptr && link->getItemType() == ItemType::Link) {
					QPainterPath path;
					path = scene_->linkPath(&items, link->getStart()->getItemCenter(), this->getItemCenter(), link->getStart(), this);
					link->setPath(path);
				}
			}
		}
		scene_->update();
	}
	return QGraphicsPolygonItem::itemChange(change, value);
}

void FlowChartGraphicsConditionItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	moveing_ = true;
	for (int i = 0; i < this->linkList_start.size(); i++) {
		FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_start.at(i));
		if (link != nullptr && link->getItemType() == ItemType::Link) {
			link->hide();
		}
	}
	for (int i = 0; i < this->linkList_end.size(); i++) {
		FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_end.at(i));
		if (link != nullptr && link->getItemType() == ItemType::Link) {
			link->hide();
		}
	}
	QGraphicsPolygonItem::mouseMoveEvent(event);
}

void FlowChartGraphicsConditionItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	moveing_ = false;
	if (this->isSelected()) {
		itemChange(QGraphicsRectItem::ItemPositionHasChanged, this->scenePos());
		for (int i = 0; i < this->linkList_start.size(); i++) {
			FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_start.at(i));
			if (link != nullptr && link->getItemType() == ItemType::Link) {
				link->show();
			}
		}
		for (int i = 0; i < this->linkList_end.size(); i++) {
			FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_end.at(i));
			if (link != nullptr && link->getItemType() == ItemType::Link) {
				link->show();
			}
		}
	}
	QGraphicsPolygonItem::mouseReleaseEvent(event);
}

//**********************************************开始/结束**********************************************
FlowChartGraphicsStartItem::FlowChartGraphicsStartItem(qreal _x, qreal _y, qreal _w, qreal _h, QGraphicsItem* parent, ItemType _type)
	: FlowChartGraphicsItem(_type)
	, QGraphicsRectItem(_x, _y, _w, _h, parent)
{
	Init();
	radius_x = (_w > _h ? _h : _w) / 2;
	radius_y = radius_x;
}

FlowChartGraphicsStartItem::~FlowChartGraphicsStartItem() {

}

void FlowChartGraphicsStartItem::DrawItemText(QPainter* _painter)
{
	_painter->setFont(font_);
	QFontMetrics font_metrics = _painter->fontMetrics();

	QRectF rect = this->boundingRect();
	QRectF resize_rect = rect;
	QString text = text_;

	QString tremporart_text;

	resize_rect.setHeight(rect.height());
	resize_rect.setWidth(rect.width());

	tremporart_text = font_metrics.elidedText(text, Qt::ElideRight, resize_rect.width());

	_painter->setPen(text_pen_);

	resize_rect.moveCenter(rect.center());
	_painter->drawText(resize_rect, Qt::AlignCenter | Qt::TextWrapAnywhere, text);
}

QPointF FlowChartGraphicsStartItem::getItemCenter()
{
	return this->sceneBoundingRect().center();
}

QRectF FlowChartGraphicsStartItem::getItemRect()
{
	return this->sceneBoundingRect();
}

bool FlowChartGraphicsStartItem::isInItem(const QPointF& _point)
{
	return this->sceneBoundingRect().contains(_point);
}

void FlowChartGraphicsStartItem::Init()
{
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);  // 设置为可选
	this->setFlag(QGraphicsItem::ItemIsMovable, true);  // 设置为可移动
	this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);//开启位置位置变化通知
	this->setPen(pen_);
	this->setBrush(brush_);
}

void FlowChartGraphicsStartItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setPen(pen_);
	painter->setBrush(brush_);
	painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	painter->drawRoundedRect(rect(), radius_x, radius_y);
	
	if (this->isSelected()) {
		QRect rect = this->rect().toRect();
		QPen dashPen(Qt::DashLine);
		dashPen.setColor(QColor(108, 108, 108));
		dashPen.setWidth(2);
		painter->setPen(dashPen);
		painter->setBrush(Qt::NoBrush);
		painter->drawRoundedRect(rect.adjusted(-1, -1, 2, 2), 2, 2);
	}
	DrawItemText(painter);
}

QVariant FlowChartGraphicsStartItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	if ((change == QGraphicsItem::ItemPositionChange || change == QGraphicsItem::ItemPositionHasChanged) && moveing_ == false) {
		qDebug() << "更新连接线";
		FlowChartScene* scene_ = dynamic_cast<FlowChartScene*>(this->scene());
		if (scene_ != nullptr) {
			QList<QGraphicsItem*>items = scene_->items();
			for (int i = 0; i < this->linkList_start.size(); i++) {
				FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_start.at(i));
				if (link != nullptr && link->getItemType() == ItemType::Link) {
					QPainterPath path;
					path = scene_->linkPath(&items, this->getItemCenter(), link->getEnd()->getItemCenter(), this, link->getEnd());
					link->setPath(path);
				}
			}
			for (int i = 0; i < this->linkList_end.size(); i++) {
				FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_end.at(i));
				if (link != nullptr && link->getItemType() == ItemType::Link) {
					QPainterPath path;
					path = scene_->linkPath(&items, link->getStart()->getItemCenter(), this->getItemCenter(), link->getStart(), this);
					link->setPath(path);
				}
			}
		}
		scene_->update();
	}
	return QGraphicsRectItem::itemChange(change, value);
}

void FlowChartGraphicsStartItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	moveing_ = true;
	for (int i = 0; i < this->linkList_start.size(); i++) {
		FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_start.at(i));
		if (link != nullptr && link->getItemType() == ItemType::Link) {
			link->hide();
		}
	}
	for (int i = 0; i < this->linkList_end.size(); i++) {
		FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_end.at(i));
		if (link != nullptr && link->getItemType() == ItemType::Link) {
			link->hide();
		}
	}
	QGraphicsItem::mouseMoveEvent(event);
}

void FlowChartGraphicsStartItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	moveing_ = false;
	if (this->isSelected()) {
		itemChange(QGraphicsRectItem::ItemPositionHasChanged, this->scenePos());
		for (int i = 0; i < this->linkList_start.size(); i++) {
			FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_start.at(i));
			if (link != nullptr && link->getItemType() == ItemType::Link) {
				link->show();
			}
		}
		for (int i = 0; i < this->linkList_end.size(); i++) {
			FlowChartLink* link = dynamic_cast<FlowChartLink*>(linkList_end.at(i));
			if (link != nullptr && link->getItemType() == ItemType::Link) {
				link->show();
			}
		}
	}
	QGraphicsItem::mouseReleaseEvent(event);
}
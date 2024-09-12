#pragma once

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QCursor>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QPainter>
#include "flowchart_global.h"


class FlowChartGraphicsItem
{
public:
    QPen	pen_;		// 基本画笔样式--背景相关
    QBrush	brush_;		// 基本画刷样式--背景相关
    QPen	text_pen_;	// 文本画笔样式
    QFont	font_;		// 字体样式
    QString text_ = "";   // 文本
    QList<FlowChartGraphicsItem*> linkList_start;
    QList<FlowChartGraphicsItem*> linkList_end;
    bool moveing_;
private:
    ItemType itemtype_;
public:

    FlowChartGraphicsItem(ItemType _type = ItemType::Null);
    ~FlowChartGraphicsItem();
    //绘制图元文本
    virtual void DrawItemText(QPainter* _painter) = 0;
    //获取图元类型
    ItemType getItemType() const;
    //获取图元中心点
    virtual QPointF getItemCenter() = 0;
    //获取图元Rect
    virtual QRectF getItemRect() = 0;
    //判断点是否在图元内
    virtual bool isInItem(const QPointF& _point) = 0;

};


#pragma once

#include "flowchart_graphics_item_base.h"
#include "flowchart_link.h"

#pragma region 流程矩形

class FlowChartGraphicsRectItem :
    public FlowChartGraphicsItem,
    public QGraphicsRectItem
{
public:
    FlowChartGraphicsRectItem(ItemType type_);
    FlowChartGraphicsRectItem(QPointF point_, ItemType type_ = ItemType::Rect);
    FlowChartGraphicsRectItem(qreal _x, qreal _y, qreal _w, qreal _h, QGraphicsItem* parent = nullptr, ItemType type_ = ItemType::Rect);
    ~FlowChartGraphicsRectItem();

    virtual void DrawItemText(QPainter* _painter)override;
    virtual QPointF getItemCenter()override;
    virtual QRectF getItemRect()override;
    virtual bool isInItem(const QPointF& _point)override;
private:
    void Init();
protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = Q_NULLPTR);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
};
#pragma endregion 流程矩形

#pragma region 判定

class FlowChartGraphicsConditionItem :
    public FlowChartGraphicsItem,
    public QGraphicsPolygonItem
{
   
public:
    FlowChartGraphicsConditionItem(ItemType type_);
    FlowChartGraphicsConditionItem(QGraphicsItem* parent = nullptr, ItemType type_ = ItemType::Condition);
    FlowChartGraphicsConditionItem(QPointF _top, QPointF _right, QPointF _down, QPointF _left, QGraphicsItem* parent = nullptr, ItemType type_ = ItemType::Condition);
    ~FlowChartGraphicsConditionItem();
    void DrawItemText(QPainter* _painter);

    virtual QPointF getItemCenter()override;
    virtual QRectF getItemRect()override;
    bool isInItem(const QPointF& _point)override;
private:
    void Init();
protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = Q_NULLPTR);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
private:
    QPolygonF diamond;
};
#pragma endregion 判定

#pragma region 开始/结束

class FlowChartGraphicsStartItem :
    public FlowChartGraphicsItem,
    public QGraphicsRectItem
{
public:
    FlowChartGraphicsStartItem(qreal _x, qreal _y, qreal _w, qreal _h, QGraphicsItem* parent = nullptr, ItemType type_ = ItemType::Start_End);

    ~FlowChartGraphicsStartItem();
    void DrawItemText(QPainter* _painter);

    virtual QPointF getItemCenter()override;
    virtual QRectF getItemRect()override;
    bool isInItem(const QPointF& _point)override;
private:
    void Init();
protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = Q_NULLPTR);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
private:
    qreal radius_x, radius_y;
};
#pragma endregion 开始/结束
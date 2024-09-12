#pragma once
#include <qgraphicsitem.h>
#include "flowchart_graphics_item.h"
class FlowChartLink 
    :public QGraphicsPathItem
    ,public FlowChartGraphicsItem
{
public:
    FlowChartLink(ItemType _type = ItemType::Link);
    ~FlowChartLink();

    virtual QPointF getItemCenter()override;
    virtual QRectF getItemRect()override;
    virtual bool isInItem(const QPointF& _point)override;
    virtual void DrawItemText(QPainter* _painter);
    //设置、获取，起点、终点
    void setStart_End(FlowChartGraphicsItem* _start, FlowChartGraphicsItem* _end);
    void setStart(QPointF _start);
    void setEnd(QPointF _end);
    FlowChartGraphicsItem* getStart();
    FlowChartGraphicsItem* getEnd();
private:
    FlowChartGraphicsItem* start_, * end_; //起点终点的对象

protected:
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
    virtual QRectF boundingRect() const;
};


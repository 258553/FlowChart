#pragma once
#include <QGraphicsScene>
#include <QMimeData>
#include <QTextEdit>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <vector>
#include <flowchart_edit.h>
#include "flowchart_global.h"
#include "flowchart_graphics_item.h"
#include "flowchart_link.h"
class FlowChartScene :
    public QGraphicsScene
{
private:
	// 初始化编辑框
	void InitLineEdit();
	//删除项
	void DeleteItem();
public:
    FlowChartScene(QObject* parent = nullptr);
    ~FlowChartScene();

	//设置view
	void setView(QGraphicsView* view);
	
	void setTempLineDraw(bool _flag);
	bool getTempLineDraw();
	//获取连线路径
	QPainterPath linkPath(QList<QGraphicsItem*>* _items, QPointF startPoint, QPointF endPoint, FlowChartGraphicsItem* _start = nullptr, FlowChartGraphicsItem* _end = nullptr);
public:
	FlowChartGraphicsItem* Add(ItemType type);

private slots:
	void FocusOutSlot(QString);

protected:

	virtual void drawBackground(QPainter* painter, const QRectF& rect);

	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

	virtual void keyPressEvent(QKeyEvent* event);

	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);

	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

	virtual void dragEnterEvent(QGraphicsSceneDragDropEvent* event);

	virtual void dragMoveEvent(QGraphicsSceneDragDropEvent* event);

	virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent* event);

	virtual void dropEvent(QGraphicsSceneDragDropEvent* event);
public:
	bool drag_move;
	FlowChartCursor mode_;					//鼠标状态
private:
	QPointF mouse_move_point_;				//鼠标当前位置
	FlowChartEdit* input_widget_;			// 图元编辑框
	FlowChartGraphicsItem* item_edit_;		//当前编辑的图元
	QGraphicsLineItem* temp_line_;			//临时连线
	QPointF press_pos_;						//鼠标按下的位置
	bool temp_line_draw = false;			//是否绘制标志
	QGraphicsView* widget_;					//view对象
	QGraphicsItem* flow_item_temp_;			//临时图元对象
	QVector<FlowChartGraphicsItem*> flow_item_selected;	//已选图元数组

	// 迪杰斯特拉算法实现，返回最小距离并生成路径点集
	long long dijkstra(const std::vector<std::vector<bool>>& grid, std::vector<std::pair<int, int>>& path, std::pair<int, int> start, std::pair<int, int> end);
	//初始化点阵
	void InitMatrix(std::vector<std::vector<bool>>& _matrix, qint8 gridSize, FlowChartScene* _scene, FlowChartGraphicsItem* _start, FlowChartGraphicsItem* _end);
	//判断三点是否共线
	bool areAligned(const std::pair<int, int>& A, const std::pair<int, int>& B, const std::pair<int, int>& C);
};

// 节点结构体，用于存储节点的坐标、到该节点的最小距离以及当前方向
struct Node {
	int x, y;          // 节点坐标
	double dist;       // 从起点到该节点的距离
	int direction;     // 进入该节点的方向
	bool operator>(const Node& other) const {
		return dist > other.dist; // 重载>操作符，用于优先队列的比较
	}
};
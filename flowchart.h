#pragma once

#include <QtWidgets/QMainWindow>
#include <QUuid>
#include <QHBoxLayout>
#include <QToolBar>
#include <QWidget>
#include <QLabel>
#include <QStackedWidget>
#include <QDockWidget>
#include <QToolButton>
#include <QMap>
#include <QDrag>
#include <QMimeData>
#include <QPushButton>
#include <QResizeEvent>
#include <QFileDialog>
#include <QTransform>
#include "flowchart_scene.h"
#include "flowchart_global.h"
#include "flowchart_view.h"

class FlowchartScene;
class FlowChartToolSideBar;
class FlowChartToolBar;
class FlowChart : public QMainWindow
{
    Q_OBJECT

public:
    FlowChart(QWidget *parent = nullptr);
    ~FlowChart();

private:
	FlowChartView *view;
	FlowChartScene*			scene_;			// 流程图编辑界面
	FlowChartToolSideBar*	tool_side_bar_;	// 左侧形状菜单
	FlowChartToolBar*		tool_bar_;		// 顶层工具栏

protected:
	virtual void resizeEvent(QResizeEvent* event);
	
};

////*******************************左侧形状菜单************************************
class FlowChartToolSideBar : public QDockWidget {
	Q_OBJECT

public:
	FlowChartToolSideBar(QWidget* parent = Q_NULLPTR);
	~FlowChartToolSideBar();

	void SetFlowChartScene(FlowChartScene* _scene);

private slots:
	void FlowButtonSlot();
	void FlowButtonClickedSlot();

private:

	QStackedWidget* stacked_widget_;

	FlowChartScene* scene;
	QMap<ItemType, QPushButton*> vec_btn_;
};


////*******************************工具栏************************************
class FlowChartToolBar : public QToolBar {
	Q_OBJECT
public:

	FlowChartToolBar(QWidget* parent = Q_NULLPTR);

	~FlowChartToolBar();

	// 传入场景对象
	void SetFlowChartScene(FlowChartScene* _scene);
	void SetFlowChartView(FlowChartView* _view);

	// 传入形状栏
	void SetToolSideBar(FlowChartToolSideBar* _side_bar);
	//保存为文件
	void FileSave();

private slots:
	void FlowActionSlot();

private:

	// 设置鼠标状态		指针工具	连接线
	QAction* mouse_status_nomal_, * mouse_status_link_;
	// 形状
	QAction* item_type_;
	// 保存为文件	读取文件
	QAction* file_save_, * file_read_;
	FlowChartView* view_;
	FlowChartScene* scene_;
	FlowChartToolSideBar* side_bar_;
};
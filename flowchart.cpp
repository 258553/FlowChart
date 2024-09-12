#include "flowchart.h"

#include <QGraphicsView>
#include "flowchart_global.h"
#include "flowchart_graphics_item.h"
FlowChart::FlowChart(QWidget* parent)
	: QMainWindow(parent)
{
	//流程图编辑界面
	scene_ = new FlowChartScene(this);
	view = new FlowChartView(scene_,this);
	
	this->setCentralWidget(view);
	scene_->setView(view);


	// 左侧形状菜单
	tool_side_bar_ = new FlowChartToolSideBar(this);
	tool_side_bar_->SetFlowChartScene(scene_);
	this->addDockWidget(Qt::LeftDockWidgetArea, tool_side_bar_);
	//工具栏
	tool_bar_ = new FlowChartToolBar(this);
	/*tool_bar_->SetFlowchartScene();*/
	tool_bar_->SetToolSideBar(tool_side_bar_);
	tool_bar_->SetFlowChartView(view);
	tool_bar_->SetFlowChartScene(scene_);
	this->addToolBar(tool_bar_);

	QWidget* main_widget = new QWidget();
	QHBoxLayout* main_layout = new QHBoxLayout();
	main_widget->setLayout(main_layout);

	this->resize(1400, 800);
	
	QPen pen;
	pen.setWidth(1);
	pen.setBrush(Qt::black);

	view->setSceneRect(0, 0, this->width() - tool_side_bar_->width(), this->height());
	QRectF rect_view=view->sceneRect();
	scene_->setSceneRect(rect_view);
}

FlowChart::~FlowChart()
{}

void FlowChart::resizeEvent(QResizeEvent *event)
{
	view->setSceneRect(0, 0, this->width() - tool_side_bar_->width(), this->height());
	QMainWindow::resizeEvent(event);
}

//*******************************左侧形状菜单************************************

FlowChartToolSideBar::FlowChartToolSideBar(QWidget* parent)
	: QDockWidget(parent) {
	this->setAllowedAreas(Qt::LeftDockWidgetArea);
	this->setWindowTitle(QStringLiteral("形状"));
	this->setMinimumWidth(170);
	this->setMaximumWidth(200);

	QWidget* main_widget = new QWidget();
	QVBoxLayout* main_layout = new QVBoxLayout();
	main_widget->setLayout(main_layout);
	this->setWidget(main_widget);

	vec_btn_.insert(ItemType::Start_End,	new QPushButton(Utils::itemtype_to_string(ItemType::Start_End)));
	vec_btn_.insert(ItemType::Rect,			new QPushButton(Utils::itemtype_to_string(ItemType::Rect)));
	vec_btn_.insert(ItemType::Condition,	new QPushButton(Utils::itemtype_to_string(ItemType::Condition)));

	for (QMap<ItemType, QPushButton*>::iterator iter = vec_btn_.begin(); iter != vec_btn_.end(); iter++) {
		//iter.value();
		main_layout->addWidget(iter.value());
		connect(iter.value(), SIGNAL(pressed()), this, SLOT(FlowButtonSlot()));
		connect(iter.value(), SIGNAL(clicked()), this, SLOT(FlowButtonClickedSlot()));
	}
	main_layout->addStretch();//添加弹簧
}
FlowChartToolSideBar::~FlowChartToolSideBar() {
	qDebug() << "释放形状菜单";
}

void FlowChartToolSideBar::SetFlowChartScene(FlowChartScene* _scene) {
	scene = _scene;
}

void FlowChartToolSideBar::FlowButtonClickedSlot()
{
	QPushButton* clicked_btn = (QPushButton*)sender();

	if (clicked_btn == vec_btn_[ItemType::Start_End]) {
		scene->Add(ItemType::Start_End);
	}
	else if (clicked_btn == vec_btn_[ItemType::Rect]) {
		scene->Add(ItemType::Rect);
	}
	else if (clicked_btn == vec_btn_[ItemType::Condition]) {
		scene->Add(ItemType::Condition);
	}
}

void FlowChartToolSideBar::FlowButtonSlot() {
	QDrag* drag = new QDrag(this);
	QMimeData* data = new QMimeData();
	data->clear();
	//获取发信号的按钮
	QPushButton* clicked_btn = (QPushButton*)sender();

	if (clicked_btn == vec_btn_[ItemType::Start_End]) {
		data->setText(Utils::itemtype_to_string(ItemType::Start_End));
	}
	else if (clicked_btn == vec_btn_[ItemType::Rect]) {
		data->setText(Utils::itemtype_to_string(ItemType::Rect));
	}
	else if (clicked_btn == vec_btn_[ItemType::Condition]) {
		data->setText(Utils::itemtype_to_string(ItemType::Condition));
	}
	drag->setMimeData(data);
	drag->exec(Qt::MoveAction);

	qDebug()<<data->text();
}

////*******************************工具栏************************************
FlowChartToolBar::FlowChartToolBar(QWidget* parent)
	: QToolBar(parent) {
	setFloatable(false);
	setMovable(false);
	setAllowedAreas(Qt::TopToolBarArea | Qt::RightToolBarArea);
	this->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);//文本在图标旁

	//创建
	file_save_			= new QAction(QPixmap(":/Resources/FileSave.png"), QStringLiteral("保存"));
	file_read_			= new QAction(QPixmap(":/Resources/FileOpen.png"), QStringLiteral("打开"));
	item_type_			= new QAction(QPixmap(":/Resources/Shape.png"), QStringLiteral("形状"));
	mouse_status_nomal_ = new QAction(QPixmap(":/Resources/ArrowCursor.png"), QStringLiteral("指针工具"));
	mouse_status_link_	= new QAction(QPixmap(":/Resources/DrawLinkCursor.png"), QStringLiteral("连接线"));

	//添加到视图
	this->addAction(file_save_);
	//this->addAction(file_read_);
	this->addAction(item_type_);
	this->addAction(mouse_status_nomal_);
	this->addAction(mouse_status_link_);

	//连接信号——槽
	connect(file_save_, SIGNAL(triggered()), this, SLOT(FlowActionSlot()));
	connect(file_read_, SIGNAL(triggered()), this, SLOT(FlowActionSlot()));
	connect(item_type_, SIGNAL(triggered()), this, SLOT(FlowActionSlot()));
	connect(mouse_status_nomal_, SIGNAL(triggered()), this, SLOT(FlowActionSlot()));
	connect(mouse_status_link_, SIGNAL(triggered()), this, SLOT(FlowActionSlot()));
}

FlowChartToolBar::~FlowChartToolBar() {

}

// 传入场景对象
void FlowChartToolBar::SetFlowChartScene(FlowChartScene* _scene) {
	scene_ = _scene;
}

void FlowChartToolBar::SetFlowChartView(FlowChartView* _view)
{
	view_ = _view;
}

// 传入形状栏
void FlowChartToolBar::SetToolSideBar(FlowChartToolSideBar* _side_bar) {
	side_bar_ = _side_bar;
}

void FlowChartToolBar::FileSave()
{
	scene_->clearSelection();

	QList< QGraphicsItem*> items_ = scene_->items();
	QRectF rect_ = items_.front()->sceneBoundingRect();

	//获取包含图元的最小矩形
	for (int i = 0; i < items_.size(); i++) {
		QRectF _rect = items_.at(i)->sceneBoundingRect();
		if (_rect.top() < rect_.top()) {
			rect_.setTop(_rect.top());
		}
		if (_rect.left() < rect_.left()) {
			rect_.setLeft(_rect.left());
		}
		if (_rect.bottom() > rect_.bottom()) {
			rect_.setBottom(_rect.bottom());
		}
		if (_rect.right() > rect_.right()) {
			rect_.setRight(_rect.right());
		}
	}
	rect_.adjust(-50, -50, 50, 50);

	
	// 创建 QImage 用于保存场景内容
	QImage image(rect_.size().toSize(), QImage::Format_ARGB32);
	image.fill(Qt::white); // 设置白色背景

	// 创建 QPainter 并将场景渲染到 QImage	
	QPainter painter(&image);
	
	//图片中的绘制范围
	QRectF targetRect_ = rect_;
	targetRect_.moveTo(0, 0);
	scene_->render(&painter, targetRect_, rect_);
	painter.end();

	// 保存图像
	QString fileName = QFileDialog::getSaveFileName(nullptr, "保存为图片", "", "PNG Files (*.png);;JPEG Files (*.jpg)");
	if (!fileName.isEmpty()) {
		image.save(fileName);
	}
}
//工具槽
void FlowChartToolBar::FlowActionSlot() {
	//获取信号发射对象
	QAction* signal_action = (QAction*)sender();
	if (signal_action == mouse_status_nomal_) {
		if (scene_ == nullptr)
			return;
		scene_->clearSelection();
		scene_->mode_ = FlowChartCursor::ArrowCursor;
		view_->setDragMode(QGraphicsView::RubberBandDrag);
	}
	else if (signal_action == mouse_status_link_) {
		if (scene_ == nullptr)
			return;
		scene_->clearSelection();
		scene_->mode_ = FlowChartCursor::DrawLinkCursor;
		view_->setDragMode(QGraphicsView::NoDrag);
		
		qDebug() << "设置鼠标状态";
	}
	else if (signal_action == item_type_) {//图形菜单
		if (side_bar_ == nullptr)
			return;
		else {
			if (side_bar_->isVisible()) {
				side_bar_->hide();
			}
			else {
				side_bar_->show();
			}	
		}
	}
	else if (signal_action == file_save_) {
		FileSave();
	}
	else if (signal_action == file_read_) {

	}
}
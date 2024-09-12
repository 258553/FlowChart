#include "flowchart_scene.h"
#include <QGraphicsSceneMouseEvent>
#include <limits>
#include <queue>
#include <unordered_set>
#include <QElapsedTimer>
// 初始化编辑框
void FlowChartScene::InitLineEdit()
{
	input_widget_ = new FlowChartEdit(widget_);
	input_widget_->setStyleSheet("QTextEdit{background:rgba(255,255,255,200); border:1px solid rgb(0,0,0);}");
	input_widget_->hide();
	connect(input_widget_, &FlowChartEdit::FocusOutSignal, this, &FlowChartScene::FocusOutSlot);
}

void FlowChartScene::DeleteItem()
{
	qDebug() << "删除项";
	QList<QGraphicsItem*>select_items = this->selectedItems();
	//QList<QGraphicsItem*>select_items = this->items();
	std::unordered_set<int> deleted_link;//保存已经删除的连线的下标
	foreach(QGraphicsItem * curItem, select_items) {
		qDebug()<<curItem;
	}

	qDebug() << "共" << select_items.size() << "项";
	for (int i = 0; i < select_items.size(); i++) {
		//qDebug()<<select_items.at(i);
		//已经删除的连线无需判断
		if (deleted_link.find(i) != deleted_link.end()) {
			continue;
		}
		QGraphicsItem* curItem = select_items.at(i);
		if (curItem == nullptr) {
			continue;
		}
		qDebug() << curItem;
		FlowChartGraphicsItem* select_item = dynamic_cast<FlowChartGraphicsItem*>(curItem);
		if (nullptr == select_item) {
			qDebug() << "非此类型";
			continue;
		}
		if (select_item->getItemType() == ItemType::Link) {
			qDebug() << "连线类型";
			continue;
		}
		//处理起点
		for (int i = 0; i < select_item->linkList_start.size(); i++) {
			FlowChartGraphicsItem* link_start = select_item->linkList_start.at(i);
			//获取一条连线
			FlowChartLink* link = dynamic_cast<FlowChartLink*> (link_start);
			if (link != nullptr) {
				FlowChartGraphicsItem* end_item = link->getEnd();		//获取连线终点
				//end_item->linkList_end.indexOf(link);
				end_item->linkList_end.removeOne(link);				//在终点项中删除连线
				deleted_link.insert(select_items.indexOf(link));
				this->removeItem(link);							//将连线从视图中删除
				delete link;											//释放连线的内存
				link = nullptr;
				
				
			}

		}
		//处理终点
		for (int i = 0; i < select_item->linkList_end.size(); i++) {
			FlowChartGraphicsItem* link_end = select_item->linkList_end.at(i);
			//获取一条连线
			FlowChartLink* link = dynamic_cast<FlowChartLink*> (link_end);
			if (link != nullptr) {
				FlowChartGraphicsItem* start_item = link->getStart();	//获取连线起点
				//end_item->linkList_end.indexOf(link);
				start_item->linkList_start.removeOne(link);			//在终点项中删除连线
				deleted_link.insert(select_items.indexOf(link));
				this->removeItem(link);							//将连线从视图中删除
				delete link;											//释放连线的内存
				link = nullptr;
			}

		}
		if (select_item->getItemType() == ItemType::Rect) {
			FlowChartGraphicsRectItem* item_ = dynamic_cast<FlowChartGraphicsRectItem*>(select_item);
			this->removeItem(item_);
			delete item_;
			item_ = nullptr;
		}
		else if (select_item->getItemType() == ItemType::Condition) {
			FlowChartGraphicsConditionItem* item_ = dynamic_cast<FlowChartGraphicsConditionItem*>(select_item);
			this->removeItem(item_);
			delete item_;
			item_ = nullptr;
		}
		else if (select_item->getItemType() == ItemType::Start_End) {
			FlowChartGraphicsStartItem* item_ = dynamic_cast<FlowChartGraphicsStartItem*>(select_item);
			this->removeItem(item_);
			delete item_;
			item_ = nullptr;
		}
	}
}

FlowChartScene::FlowChartScene(QObject* parent)
	: QGraphicsScene(parent)
{
	mode_ = FlowChartCursor::ArrowCursor;
}

FlowChartScene::~FlowChartScene() {

}

void FlowChartScene::setView(QGraphicsView* view)
{
	widget_ = view;
}

void FlowChartScene::setTempLineDraw(bool _flag)
{
	temp_line_draw = _flag;
}

bool FlowChartScene::getTempLineDraw()
{
	return temp_line_draw;
}

QPainterPath FlowChartScene::linkPath(QList<QGraphicsItem*>* _items, QPointF startPoint, QPointF endPoint, FlowChartGraphicsItem* _start, FlowChartGraphicsItem* _end)
{
	qDebug() << "startPoint:" << startPoint;
	qDebug() << "endPoint:" << endPoint;

	QPainterPath  res_link_path;
	if (startPoint == endPoint) {
		return res_link_path;
	}
	qint8 gridSize = 5;
	QRectF sceneRect = this->sceneRect();
	long long rows = static_cast<int>(sceneRect.height() / gridSize) + 2;//点阵行数
	long long cols = static_cast<int>(sceneRect.width() / gridSize) + 2;//点阵列数
	//qDebug() << QString("Scene=>[%1,%2]").arg(rows).arg(cols);
	std::vector<std::vector<bool>> matrix(int(rows), std::vector<bool>(qsizetype(cols), false));//界面点阵

	// 遍历所有图元获取点阵
	InitMatrix(matrix, gridSize, this, _start, _end);
	int startX = startPoint.y() / gridSize;
	int startY = startPoint.x() / gridSize;
	int endX = endPoint.y() / gridSize, endY = endPoint.x() / gridSize;
	std::vector<std::pair<int, int>> path_point;
	long long ret = dijkstra(matrix, path_point, { startX ,startY }, { endX ,endY });
	if (ret == std::numeric_limits<long long>::max() || path_point.size() < 2) {
		qDebug() << "没找到合适连线";
		return res_link_path;
	}

	bool flag = true;			//false 标志纵向 true 标志横向
	std::pair<int, int> previous_point = path_point.back();
	res_link_path.moveTo(previous_point.first * gridSize, previous_point.second * gridSize);


	for (long long i = path_point.size() - 2; i > 0; i--) {
		if (!areAligned(path_point[i + 1], path_point[i], path_point[i - 1])) {
			res_link_path.lineTo(path_point[i].first * gridSize, path_point[i].second * gridSize);
		}
	}
	res_link_path.lineTo(path_point[0].first * gridSize, path_point[0].second * gridSize);
	qDebug() << res_link_path.elementCount();

	for (int i = 0; i < res_link_path.elementCount(); i++) {
		qDebug() << res_link_path.elementAt(i);
	}
	return res_link_path;
}

//判断三点是否共线
bool FlowChartScene::areAligned(const std::pair<int, int>& A, const std::pair<int, int>& B, const std::pair<int, int>& C) {
	return (A.first == B.first && B.first == C.first) || (A.second == B.second && B.second == C.second);
}
// 迪杰斯特拉算法实现，返回最小距离并生成路径点集
long long FlowChartScene::dijkstra(const std::vector<std::vector<bool>>& grid, std::vector<std::pair<int,int>>& path, std::pair<int, int> start, std::pair<int, int> end) {

	// 方向数组，分别表示右、下、左、上
	const int dx[] = { 0, 1, 0, -1 }; // x方向
	const int dy[] = { 1, 0, -1, 0 }; // y方向

	long long rows = grid.size();           // 网格的行数
	long long cols = grid[0].size();        // 网格的列数
	std::vector<std::vector<long long>> distances(rows, std::vector<long long>(cols, std::numeric_limits<long long>::max()));
	std::vector<std::vector<std::pair<int, int>>> previous(rows, std::vector<std::pair<int, int>>(cols, { -1, -1 })); // 记录前驱节点
	std::vector<std::vector<int>> direction(rows, std::vector<int>(cols, -1)); // 记录每个节点的方向
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq; // 优先队列（最小堆）

	int startX = start.first, startY = start.second;
	int endX = end.first, endY = end.second;

	// 初始化起点
	pq.push({ startX, startY, 0, -1 }); // 起点没有方向
	distances[startX][startY] = 0; // 起点距离为0

	while (!pq.empty()) {
		Node current = pq.top(); // 获取优先队列中的最小距离节点
		pq.pop();

		int x = current.x, y = current.y; // 当前节点的坐标
		double dist = current.dist;        // 当前节点的最小距离
		int dir = current.direction;      // 当前节点的方向

		// 如果到达终点，构建路径并返回结果
		if (x == endX && y == endY) {
			while (x != startX || y != startY) {
				path.push_back({ y, x });
				auto prev = previous[x][y];
				x = prev.first;
				y = prev.second;
			}
			path.push_back({ startY, startX });
			//reverse(path.begin(), path.end()); // 反转路径，起点到终点
			return dist;
		}

		// 遍历所有可能的移动方向
		for (int i = 0; i < 4; ++i) { // 只允许水平和垂直移动
			int nx = x + dx[i]; // 新节点的x坐标
			int ny = y + dy[i]; // 新节点的y坐标
			if (nx >= 0 && nx < rows && ny >= 0 && ny < cols && grid[nx][ny] == 0) { // 检查新节点是否在网格内
				double cost = 1; // 水平和垂直移动代价为1
				// 如果方向改变，即发生拐角，增加拐角代价
				if (dir != -1 && dir != i) {
					cost += 1; // 拐角代价为1
				}
				double newDist = dist + cost; // 计算从起点到新节点的距离
				// 如果找到更短的路径，更新距离并将新节点加入优先队列
				if (newDist < distances[nx][ny]) {
					distances[nx][ny] = newDist;
					previous[nx][ny] = { x, y }; // 记录前驱节点
					direction[nx][ny] = i; // 记录方向
					pq.push({ nx, ny, newDist, i });
				}
			}
		}
	}

	return std::numeric_limits<long long>::max(); // 如果没有路径到达目标
}
//初始化点阵
void FlowChartScene::InitMatrix(std::vector<std::vector<bool>>& _matrix, qint8 gridSize, FlowChartScene* _scene, FlowChartGraphicsItem* _start, FlowChartGraphicsItem* _end) {
	int line = 0;
	int shap = 0;
	QRectF itemRect;
	for (QGraphicsItem* item : _scene->items())
	{
		itemRect = item->boundingRect();// .translated(item->pos());
		FlowChartGraphicsItem* item_ = dynamic_cast<FlowChartGraphicsItem*>(item);
		// 设置点阵中是否有图元
		if (item_ == nullptr || item_->getItemType() == ItemType::Null) {
			//item_为空，或者图元类型为空时忽略图元，忽略其实图元和终点图元；
			continue;
		}
		if ((_start != nullptr && item_ == _start) || (_end != nullptr && item_ == _end)) {
			continue;
		}
		if (item_->getItemType() == ItemType::Link) {//处理线条图元
			line++;
			FlowChartLink* linkItem_ = dynamic_cast<FlowChartLink*>(item_);
			if (linkItem_ != nullptr) {
				QPainterPath path = linkItem_->path();
				QList<QPolygonF> polygons = path.toSubpathPolygons();
				qDebug() << "path 图形数：" << polygons.size();
				for (const QPolygonF& polygon : polygons) {
					qDebug() << "图形点数：" << polygon.size();
					for (int i = 1; i < polygon.size(); i++) {
						QPointF point_previous = polygon.at(i - 1);
						QPointF point_current = polygon.at(i);
						qDebug() << QString("(%1,%2)").arg(point_previous.x()).arg(point_previous.y());
						qDebug() << QString("(%1,%2)").arg(point_current.x()).arg(point_current.y());
						if (point_previous.x() == point_current.x()) {
							long long current_Col = point_current.x() / gridSize;
							long long start = 0, end = 0;
							if (point_previous.y() <= point_current.y()) {
								start = point_previous.y();
								end = point_current.y();
							}
							else {
								start = point_current.y();
								end = point_previous.y();
							}
							for (int j = start; j <= end; j += gridSize) {
								_matrix[j / gridSize][current_Col] = true;
							}
						}
						else if (point_previous.y() == point_current.y()) {
							long long current_Row = point_current.y() / gridSize;
							long long start = 0, end = 0;
							if (point_previous.x() <= point_current.x()) {
								start = point_previous.x();
								end = point_current.x();
							}
							else {
								start = point_current.x();
								end = point_previous.x();
							}
							for (int j = start; j <= end; j += gridSize) {
								_matrix[current_Row][j / gridSize] = true;
							}
						}
						//qDebug() << "Point:" << point_;
					}
				}
			}
		}
		else {//判断非线条图元
			long long rowLen = itemRect.x() + itemRect.width();
			long long colLen = itemRect.y() + itemRect.height();
			shap++;
			for (long long row = itemRect.y(); row <= colLen; row += gridSize) {
				for (long long col = itemRect.x(); col <= rowLen; col += gridSize) {
					//QPointF point(col * gridSize, row * gridSize);
					//if (itemRect.contains(point)) {
					//	matrix[row][col] = true;
					//}
					_matrix[row / gridSize][col / gridSize] = true;
				}
			}
		}
	}

	long long rowLen = _start->getItemRect().x() + _start->getItemRect().width();
	long long colLen = _start->getItemRect().y() + _start->getItemRect().height();
	for (long long row = _start->getItemRect().y(); row <= colLen; row += gridSize) {
		for (long long col = _start->getItemRect().x(); col <= rowLen; col += gridSize) {
			_matrix[row / gridSize][col / gridSize] = false;
		}
	}
	rowLen = _end->getItemRect().x() + _end->getItemRect().width();
	colLen = _end->getItemRect().y() + _end->getItemRect().height();
	for (long long row = _end->getItemRect().y(); row <= colLen; row += gridSize) {
		for (long long col = _end->getItemRect().x(); col <= rowLen; col += gridSize) {
			_matrix[row / gridSize][col / gridSize] = false;
		}
	}
}

FlowChartGraphicsItem* FlowChartScene::Add(ItemType type)
{
	if (type == ItemType::Rect)
	{
		// 流程
		FlowChartGraphicsRectItem* _item = new FlowChartGraphicsRectItem(0, 0, 100, 50);
		_item->setZValue(double(ItemLevel::RectItem));
		this->addItem(_item);
		return _item;
	}
	else if (type == ItemType::Condition)
	{
		// 判断
		QPoint p1(50, 0);
		QPoint p2(100, 25);
		QPoint p3(50, 50);
		QPoint p4(0, 25);
		FlowChartGraphicsConditionItem* _item = new FlowChartGraphicsConditionItem(p1, p2, p3, p4);
		_item->setZValue(double(ItemLevel::ConditionItem));
		this->addItem(_item);
		return _item;
	}
	else if (type == ItemType::Start_End)
	{
		// 开始/结束
		FlowChartGraphicsStartItem* _item = new FlowChartGraphicsStartItem(0, 0, 100, 50);
		_item->setZValue(double(ItemLevel::Start_EndItem));
		this->addItem(_item);
		return _item;
	}
	return nullptr;
}

void FlowChartScene::FocusOutSlot(QString text) {
	item_edit_->text_ = text;
	input_widget_->hide();
	this->update();
}

void FlowChartScene::drawBackground(QPainter* painter, const QRectF& rect) {
	QGraphicsScene::drawBackground(painter, rect);
}

void FlowChartScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
	//初始化编辑框
	if (input_widget_ == nullptr) {
		InitLineEdit();
	}

	input_widget_->setAlignment(Qt::AlignCenter);
	if (mode_ == FlowChartCursor::ArrowCursor) {
		QList<QGraphicsItem*> graphics_items = this->selectedItems();
		if (graphics_items.size() == 1) {
			item_edit_ = dynamic_cast<FlowChartGraphicsItem*>(graphics_items.at(0));
			if (item_edit_->getItemType() == ItemType::Rect) {
				FlowChartGraphicsRectItem* item = dynamic_cast<FlowChartGraphicsRectItem*>(item_edit_);
				QPoint itemp = item->pos().toPoint();

				QPointF p = widget_->mapToScene(itemp);
				itemp = p.toPoint();

				QRectF rect = item->boundingRect();
				QPointF Point = item->pos();
				input_widget_->resize(rect.width(), rect.height());
				qDebug() << itemp.x() << "," << itemp.y();
				input_widget_->move(itemp);
				input_widget_->setText(item->text_);
				input_widget_->show();
			}
			else if (item_edit_->getItemType() == ItemType::Condition) {
				FlowChartGraphicsConditionItem* item = dynamic_cast<FlowChartGraphicsConditionItem*>(item_edit_);
				QPoint itemp = item->pos().toPoint();

				QPointF p = widget_->mapToScene(itemp);
				itemp = p.toPoint();

				QRectF rect = item->boundingRect();
				QPointF Point = item->pos();
				input_widget_->resize(rect.width(), rect.height());
				qDebug() << itemp.x() << "," << itemp.y();
				input_widget_->move(itemp);
				input_widget_->setText(item->text_);

			}
			else if (item_edit_->getItemType() == ItemType::Start_End) {
				FlowChartGraphicsStartItem* item = dynamic_cast<FlowChartGraphicsStartItem*>(item_edit_);
				QPoint itemp = item->pos().toPoint();

				QPointF p = widget_->mapToScene(itemp);
				itemp = p.toPoint();

				QRectF rect = item->boundingRect();
				QPointF Point = item->pos();
				input_widget_->resize(rect.width(), rect.height());
				//qDebug() << itemp.x() << "," << itemp.y();
				input_widget_->move(itemp);
				input_widget_->setText(item->text_);
			}
			else if (item_edit_->getItemType() == ItemType::Link) {
				FlowChartLink* item = dynamic_cast<FlowChartLink*>(item_edit_);
				QPointF center = item->getItemCenter();
				//QRectF rect(center.x() - 20, center.y() - 10, 40, 20);
				input_widget_->resize(80, 40);
				//qDebug() << itemp.x() << "," << itemp.y();
				input_widget_->move(center.x() - 40, center.y() - 20);
				input_widget_->setText(item->text_);
			}
			input_widget_->show();
			input_widget_->setFocus();
		}
	}

	QGraphicsScene::mouseDoubleClickEvent(event);
}

void FlowChartScene::keyPressEvent(QKeyEvent* event) {

	if (event->key() == Qt::Key_Delete) {//按下Delete删除项及其连接线
		qDebug() << "按下Delete";
		DeleteItem();
	}
	else if (event->key() == Qt::Key_A && event->modifiers() & Qt::ControlModifier) {//选中所有项
		QList<QGraphicsItem*> items_ = this->items();
		for (int i = 0; i < items_.count(); i++) {
			items_.at(i)->setSelected(true);
		}
	}
	QGraphicsScene::keyPressEvent(event);
}

void FlowChartScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	press_pos_ = event->scenePos();
	if (this->selectedItems().size() == 1 && mode_ == FlowChartCursor::DrawLinkCursor) {
		temp_line_ = new QGraphicsLineItem(QLineF(event->scenePos(), event->scenePos()));
		temp_line_->setPen(QPen(QColor(89, 152, 209), 3));
		this->addItem(temp_line_);
	}
	QGraphicsScene::mousePressEvent(event);
}

void FlowChartScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
	if (mode_ == FlowChartCursor::ArrowCursor) {
		QGraphicsScene::mouseMoveEvent(event);
	}
	else if (mode_ == FlowChartCursor::DrawLinkCursor && temp_line_ != nullptr) {
		QLineF temp_line(temp_line_->line().p1(), event->scenePos());
		temp_line_->setLine(temp_line);
	}
}

void FlowChartScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
	drag_move = false;
	if (mode_ == FlowChartCursor::ArrowCursor) {
		QGraphicsScene::mouseReleaseEvent(event);
	}
	else if (mode_ == FlowChartCursor::DrawLinkCursor && temp_line_ != nullptr) {
		this->removeItem(temp_line_);
		delete temp_line_;
		temp_line_ = nullptr;
		QList<QGraphicsItem*>items = this->items();
		if (items.size() > 1) {
			FlowChartGraphicsItem* startItem = nullptr;
			FlowChartGraphicsItem* endItem = nullptr;
			for (int i = 0; i < items.size(); i++) {
				FlowChartGraphicsItem* item_ = dynamic_cast<FlowChartGraphicsItem*>(items.at(i));
				if (!item_) {
					qDebug() << "非此类型";
					continue;
				}
				if (item_->getItemType() == ItemType::Link) {
					qDebug() << "连线类型";
					continue;
				}
				qDebug() << item_->getItemRect();
				//qDebug() << item_.po
				if (item_->isInItem(press_pos_)) {
					startItem = item_;
					break;
				}
			}
			if (startItem == nullptr) {
				qDebug() << "头为空";
			}
			if (startItem != nullptr) {
				qDebug() << QString("start=>(%1,%2)").arg(startItem->getItemCenter().x()).arg(startItem->getItemCenter().y());
				for (int i = 0; i < items.size(); i++) {
					FlowChartGraphicsItem* item_ = dynamic_cast<FlowChartGraphicsItem*>(items.at(i));
					if (!item_) {
						qDebug() << "非此类型";
						continue;
					}
					if (item_->getItemType() == ItemType::Link) {
						qDebug() << "连线类型";
						continue;
					}
					if (startItem != item_ && item_->isInItem(event->scenePos())) {
						endItem = item_;
						break;
					}
				}
			}

			if (startItem != nullptr && endItem != nullptr) {
				//qDebug() << QString("endItem=>(%1,%2)").arg(endItem->getItemCenter().x()).arg(endItem->getItemCenter().y());
				qDebug() << "创建连线";
				FlowChartLink* link = new FlowChartLink();
				link->setZValue(double(ItemLevel::LinkItem));//设置连线层级
				QPainterPath path = this->linkPath(&items, startItem->getItemCenter(), endItem->getItemCenter(), startItem, endItem);
				if (path.isEmpty())
				{
					qDebug() << "空连线";
				}
				else {
					link->setPath(path);
					link->setStart_End(startItem, endItem);

					startItem->linkList_start.append(link);
					endItem->linkList_end.append(link);
					this->addItem(link);
				}
			}
			for (int i = 0; i < this->items().size(); i++) {
				qDebug() << this->items().at(i);
			}
		}
		//this->removeItem(temp_line_);
		
	}
}

void FlowChartScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event) {

	if (event->source() == widget_) {
		event->ignore();
		return QGraphicsScene::dragEnterEvent(event);
	}
	else if (event->mimeData()->hasText()) {
		qDebug() << "拖拽组件";
		flow_item_temp_ = nullptr;
		event->setDropAction(Qt::CopyAction);
		event->acceptProposedAction();  // 接受拖拽操作
	}
	event->accept();
}

void FlowChartScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event) {
	mouse_move_point_ = event->scenePos();


	if (flow_item_temp_ == nullptr) {
		qDebug() << "拖拽添加";
		event->setDropAction(Qt::MoveAction);
		drag_move = true;
		ItemType type = Utils::string_to_typeitem(event->mimeData()->text());
		flow_item_temp_ = dynamic_cast<QGraphicsItem*>(this->Add(type));
	}
	if (flow_item_temp_ != nullptr) {
		QGraphicsItem* item_temp_ = dynamic_cast<QGraphicsItem*>(flow_item_temp_);
		QPointF point_ = event->scenePos();
		
		QRectF rect_ = flow_item_temp_->boundingRect();
		point_.rx() = point_.x() - (rect_.width() / 2);
		point_.ry() = point_.y() - (rect_.height() / 2);
		item_temp_->setPos(point_);
		event->acceptProposedAction();
	}
}

void FlowChartScene::dragLeaveEvent(QGraphicsSceneDragDropEvent* event) {
	qDebug() << "dragLeaveEvent";
	if (flow_item_temp_ != nullptr) {
		this->removeItem(flow_item_temp_);
		delete flow_item_temp_;
		flow_item_temp_ = nullptr;
		qDebug() << "删除临时图元";
	}
	else {
		QGraphicsScene::dragLeaveEvent(event);
	}
}

void FlowChartScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
	qDebug() << "dropEvent";
	if (event->source() == widget_) {
		event->ignore();
		return QGraphicsScene::dragEnterEvent(event);
	}
	else {
		flow_item_temp_ = nullptr;
		//event->acceptProposedAction();
	}

}

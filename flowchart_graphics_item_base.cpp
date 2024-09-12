#include "flowchart_graphics_item_base.h"


FlowChartGraphicsItem::FlowChartGraphicsItem(ItemType _type) {
	itemtype_ = _type;
	pen_.setWidth(1);
	pen_.setColor(QColor(65, 113, 156));

	brush_ = QBrush(QColor(89, 152, 209));

	text_pen_ = QPen();;
	text_pen_.setColor(QColor(254, 255, 255));
	text_pen_.setWidth(2);

	font_ = QFont("Microsoft YaHei", 12, 2);
}
FlowChartGraphicsItem::~FlowChartGraphicsItem() {

}

ItemType FlowChartGraphicsItem::getItemType() const
{
	return itemtype_;
}
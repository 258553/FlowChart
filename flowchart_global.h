#pragma once


#include <QPen>
#include <QBrush>
#include <QFont>
#include <QString>

#define int_max 2147483648

enum class ItemType
{
	Null = 0,		// 空
	Link,			// 连接线
	Start_End,		//开始，结束
	Rect,			// 流程
	Condition,		// 判定
};

class Utils {
public:
	//枚举转字符串
	static QString itemtype_to_string(ItemType type) {
		QString type_s;
		switch (type)
		{
		case ItemType::Null:
			type_s = QStringLiteral("空");
			break;
		case ItemType::Link:
			type_s = QStringLiteral("连接线");
			break;
		case ItemType::Start_End:
			type_s = QStringLiteral("开始/结束");
			break;
		case ItemType::Rect:
			type_s = QStringLiteral("流程");
			break;
		case ItemType::Condition:
			type_s = QStringLiteral("判定");
			break;
		default:
			break;
		}
		return type_s;
	}
	//字符串转枚举
	static ItemType string_to_typeitem(QString type) {
		if (type == QStringLiteral("空")) {
			return ItemType::Null;
		}
		else if (type == QStringLiteral("连接线")) {
			return ItemType::Link;
		}
		else if (type == QStringLiteral("开始/结束")) {
			return ItemType::Start_End;
		}
		else if (type == QStringLiteral("流程")) {
			return ItemType::Rect;
		}
		else if (type == QStringLiteral("判定")) {
			return ItemType::Condition;
		}
		return ItemType::Null;
	}
};

//鼠标状态
enum class  FlowChartCursor
{
	ArrowCursor = 0,
	DrawLinkCursor,
	SizeAllCurSor,
	OpenHandCursor,
	ClosedHandCursor,
};
//图层
enum class ItemLevel {
	LinkItem=-1,
	Start_EndItem,
	RectItem,
	ConditionItem,
	Text
};

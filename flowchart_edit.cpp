#include "flowchart_edit.h"

#include <QDebug>
FlowChartEdit::FlowChartEdit(QWidget* parent)
	:QTextEdit(parent)
{
	this->setAlignment(Qt::AlignCenter);
	
}

void FlowChartEdit::focusOutEvent(QFocusEvent* e)
{
	qDebug() << "失去焦点";
	emit FocusOutSignal(this->toPlainText());
	
	QString str = this->toPlainText();
	qDebug() << str;
	this, clear();
	return QTextEdit::focusOutEvent(e);
}

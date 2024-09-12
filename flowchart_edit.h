#pragma once
#include <qtextedit.h>
class FlowChartEdit :
    public QTextEdit
{
    Q_OBJECT
public:
    FlowChartEdit(QWidget* parent);
signals:
    void FocusOutSignal(QString _text);
protected:
    virtual void focusOutEvent(QFocusEvent* e);
};


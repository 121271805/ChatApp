#pragma once

#include <QScrollArea>
#include <QVBoxLayout>
#include <QTimer>
#include <QStyleOption>
#include <QPainter>
/******************************************************************************
 *
 * @file       chatview.h
 * @brief      ��д���촰���е��������
 *
 * @author     Carpe_Diem
 * @date       2024/12/04
 * @history
 *****************************************************************************/
class ChatView : public QWidget {
	Q_OBJECT
public:
	ChatView(QWidget* parent = Q_NULLPTR);
	void appendChatItem(QWidget* item);//β����Ϣ����item
	void prependChatItem(QWidget* item);//ͷ����Ϣ����item
	void insertChatItem(QWidget* before, QWidget* item);//�м����Ϣ����item
	void removeAllItem();
protected:
	bool eventFilter(QObject* o, QEvent* e) override;
	void paintEvent(QPaintEvent* event) override;
private slots:
	void onVScrollBarMoved(int min, int max);
private:
	void initStyleSheet();
	//QWidget *m_pCenterWidget;
	QVBoxLayout* m_pVl;//��ֱ����
	QScrollArea* m_pScrollArea;//��������
	bool isAppended;//���Ƽ��������¼��ʱ����

};


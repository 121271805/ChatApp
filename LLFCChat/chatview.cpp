#include "chatview.h"
#include <QEvent>
#include <QDebug>
#include <QScrollBar>

ChatView::ChatView(QWidget* parent) : QWidget(parent), isAppended(false) {
	//��ֱ����
	QVBoxLayout* pMainLayout = new QVBoxLayout();
	this->setLayout(pMainLayout);
	pMainLayout->setContentsMargins(0, 0, 0, 0);

	//���ù�������
	m_pScrollArea = new QScrollArea();
	m_pScrollArea->setObjectName("chat_area");
	pMainLayout->addWidget(m_pScrollArea);

	//��������widget
	QWidget* w = new QWidget(this);
	w->setObjectName("chat_bg");
	w->setAutoFillBackground(true);//������������

	//�����Ӵ�ֱ����
	QVBoxLayout* pHLayout_1 = new QVBoxLayout();
	pHLayout_1->addWidget(new QWidget(), 100000);
	w->setLayout(pHLayout_1);
	m_pScrollArea->setWidget(w);

	m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//ȡ��������
	QScrollBar* pVScrollBar = m_pScrollArea->verticalScrollBar();//��ȡ��ֱ������
	//��������Χ����ʱ�������¼��������ʱ������
	connect(pVScrollBar, &QScrollBar::rangeChanged, this, &ChatView::onVScrollBarMoved);

	//�Ѵ�ֱ�������ŵ��ϱ� ������ԭ���Ĳ���
	QHBoxLayout* pHLayout_2 = new QHBoxLayout();
	pHLayout_2->addWidget(pVScrollBar, 0, Qt::AlignRight);
	pHLayout_2->setContentsMargins(0, 0, 0, 0);
	m_pScrollArea->setLayout(pHLayout_2);
	pVScrollBar->setHidden(true);//���ع�����

	m_pScrollArea->setWidgetResizable(true);
	m_pScrollArea->installEventFilter(this);
	initStyleSheet();
}

void ChatView::onVScrollBarMoved(int min, int max) {
	if (isAppended) //���item���ܵ��ö��
	{
		QScrollBar* pVScrollBar = m_pScrollArea->verticalScrollBar();
		pVScrollBar->setSliderPosition(pVScrollBar->maximum());//���ù�����λ��
		//500�����ڿ��ܵ��ö��
		QTimer::singleShot(500, [this]() {
			isAppended = false;
			});
	}
}

void ChatView::appendChatItem(QWidget* item) {
	QVBoxLayout* vl = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
	qDebug() << "vl->count() is " << vl->count();
	vl->insertWidget(vl->count() - 1, item);//������������
	isAppended = true;
}

void ChatView::prependChatItem(QWidget* item) {

}

void ChatView::insertChatItem(QWidget* before, QWidget* item) {

}

void ChatView::removeAllItem() {
	QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());

	int count = layout->count();

	for (int i = 0; i < count - 1; ++i) {
		QLayoutItem* item = layout->takeAt(0); // ʼ�մӵ�һ���ؼ���ʼɾ��
		if (item) {
			if (QWidget* widget = item->widget()) {
				delete widget;
			}
			delete item;
		}
	}
}

bool ChatView::eventFilter(QObject* o, QEvent* e) {
	/*if(e->type() == QEvent::Resize && o == )
	{

	}
	else */if (e->type() == QEvent::Enter && o == m_pScrollArea) {//����maximum()�Ƿ����������Ƿ���ʾ������
		m_pScrollArea->verticalScrollBar()->setHidden(m_pScrollArea->verticalScrollBar()->maximum() == 0);
	} else if (e->type() == QEvent::Leave && o == m_pScrollArea) {//����뿪�����ع�����
		m_pScrollArea->verticalScrollBar()->setHidden(true);
	}
	return QWidget::eventFilter(o, e);
}

void ChatView::paintEvent(QPaintEvent* event) {
	QStyleOption opt;
	opt.initFrom(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatView::initStyleSheet() {

}

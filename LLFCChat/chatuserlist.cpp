#include "chatuserlist.h"

ChatUserList::ChatUserList(QWidget* parent) : QListWidget(parent), _load_pending(false) {
	Q_UNUSED(parent);
	//�ر�Ĭ�ϵĺ��������Ĺ�����
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	// ��װ�¼�������
	this->viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject* watched, QEvent* event) {
	//����¼��Ƿ����������������뿪Ŀ������
	if (watched == this->viewport()) {
		if (event->type() == QEvent::Enter) {
			//�����������ʾ������
			this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		} else if (event->type() == QEvent::Leave) {
			//����뿪�����ع�����
			this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		}
	}

	//����¼��Ƿ����������¼�
	if (watched == this->viewport() && event->type() == QEvent::Wheel) {
		QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
		int numDegrees = wheelEvent->angleDelta().y() / 8;//���ù�������
		int numSteps = numDegrees / 15;//���ò���

		//���ù�������
		this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

		//����Ƿ�������ײ�
		QScrollBar* scrollBar = this->verticalScrollBar();
		int maxScrollValue = scrollBar->maximum();
		int currentValue = scrollBar->value();
		int pageSize = 10;//ÿҳ���ص���ϵ������

		if (maxScrollValue - currentValue <= 0) {
			//�������ײ��������µ���ϵ��
			qDebug() << "load more char user";
			//�����ź�֪ͨ���������ظ��������¼
			emit sig_loading_chat_user();
		}
		return true;//ֹͣ�¼�����
	}
	return QListWidget::eventFilter(watched, event);//�����¼�ִ�л����е�Ĭ�ϲ���
}

#include "applyfriendlist.h"
#include <QWheelEvent>
#include <QScrollBar>
#include "listitembase.h"

ApplyFriendList::ApplyFriendList(QWidget* parent) {
	Q_UNUSED(parent);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	// ��װ�¼�������
	this->viewport()->installEventFilter(this);
}

bool ApplyFriendList::eventFilter(QObject* watched, QEvent* event) {
	// ����¼��Ƿ����������������뿪
	if (watched == this->viewport()) {
		if (event->type() == QEvent::Enter) {
			// �����������ʾ������
			this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		} else if (event->type() == QEvent::Leave) {
			// ����뿪�����ع�����
			this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		}
	}

	if (watched == this->viewport()) {
		if (event->type() == QEvent::MouseButtonPress) {
			emit sig_show_search(false);
		}
	}

	// ����¼��Ƿ����������¼�
	if (watched == this->viewport() && event->type() == QEvent::Wheel) {
		QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
		int numDegrees = wheelEvent->angleDelta().y() / 8;
		int numSteps = numDegrees / 15; // �����������

		// ���ù�������
		this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

		return true; // ֹͣ�¼�����
	}

	return QListWidget::eventFilter(watched, event);

}
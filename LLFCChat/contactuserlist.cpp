#include "contactuserlist.h"
#include "global.h"
#include "listitembase.h"
#include "grouptipitem.h"
#include "conuseritem.h"
#include <QRandomGenerator>
#include "tcpmgr.h"
#include "usermgr.h"
#include <QTimer>
#include <QCoreApplication>
#include "usermgr.h"

ContactUserList::ContactUserList(QWidget* parent) 
	: _add_friend_item(nullptr), _load_pending(false) {
	Q_UNUSED(parent);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	// ��װ�¼�������
	this->viewport()->installEventFilter(this);

	//ģ������ݿ���ߺ�˴������������,�����б����
	addContactUserList();
	//���ӵ�����źźͲ�
	connect(this, &QListWidget::itemClicked, this, &ContactUserList::slot_item_clicked);
	//���ӶԶ�ͬ����֤��֪ͨ���ź�
	connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend, this,
		&ContactUserList::slot_add_auth_firend);

	//�����Լ����ͬ����֤�����ˢ��
	connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp, this,
		&ContactUserList::slot_auth_rsp);
}

void ContactUserList::ShowRedPoint(bool bshow /*= true*/) {
	_add_friend_item->ShowRedPoint(bshow);
}

void ContactUserList::addContactUserList() {
	//��ȡ�����б�
	auto* groupTip = new GroupTipItem();
	QListWidgetItem* item = new QListWidgetItem;
	item->setSizeHint(groupTip->sizeHint());
	this->addItem(item);
	this->setItemWidget(item, groupTip);
	item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

	_add_friend_item = new ConUserItem();
	_add_friend_item->setObjectName("new_friend_item");
    _add_friend_item->SetInfo(0, tr("New Friend"), ":/res/add_friend.png");
	_add_friend_item->SetItemType(ListItemType::APPLY_FRIEND_ITEM);
	QListWidgetItem* add_item = new QListWidgetItem;
	//qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
	add_item->setSizeHint(_add_friend_item->sizeHint());
	this->addItem(add_item);
	this->setItemWidget(add_item, _add_friend_item);
	//Ĭ�������µ�����������Ŀ��ѡ��
	this->setCurrentItem(add_item);

	auto* groupCon = new GroupTipItem();
    groupCon->SetGroupTip(tr("Contacts"));
	_groupitem = new QListWidgetItem;
	_groupitem->setSizeHint(groupCon->sizeHint());
	this->addItem(_groupitem);
	this->setItemWidget(_groupitem, groupCon);
	_groupitem->setFlags(_groupitem->flags() & ~Qt::ItemIsSelectable);

	////���غ�˷��͹����ĺ����б�
	//auto con_list = UserMgr::GetInstance()->GetConListPerPage();
	//for (auto& con_ele : con_list) {
	//	auto* con_user_wid = new ConUserItem();
	//	con_user_wid->SetInfo(con_ele->_uid, con_ele->_name, con_ele->_icon);
	//	QListWidgetItem* item = new QListWidgetItem;
	//	//qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
	//	item->setSizeHint(con_user_wid->sizeHint());
	//	this->addItem(item);
	//	this->setItemWidget(item, con_user_wid);
	//}

	//UserMgr::GetInstance()->UpdateContactLoadedCount();

	// ģ���б� ����QListWidgetItem���������Զ����widget
	for (int i = 0; i < 13; i++) {
		int randomValue = QRandomGenerator::global()->bounded(100); // ����0��99֮����������
		int str_i = randomValue % strs.size();
		int head_i = randomValue % heads.size();
		int name_i = randomValue % names.size();

		auto* con_user_wid = new ConUserItem();
		con_user_wid->SetInfo(0, names[name_i], heads[head_i]);
		QListWidgetItem* item = new QListWidgetItem;
		//qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
		item->setSizeHint(con_user_wid->sizeHint());
		this->addItem(item);
		this->setItemWidget(item, con_user_wid);
	}
}

bool ContactUserList::eventFilter(QObject* watched, QEvent* event) {
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

	// ����¼��Ƿ����������¼�
	if (watched == this->viewport() && event->type() == QEvent::Wheel) {
		QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
		int numDegrees = wheelEvent->angleDelta().y() / 8;
		int numSteps = numDegrees / 15; // �����������

		// ���ù�������
		this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

		// ����Ƿ�������ײ�
		QScrollBar* scrollBar = this->verticalScrollBar();
		int maxScrollValue = scrollBar->maximum();
		int currentValue = scrollBar->value();
		//int pageSize = 10; // ÿҳ���ص���ϵ������

		if (maxScrollValue - currentValue <= 0) {
			//auto b_loaded = UserMgr::GetInstance()->IsLoadChatFin();
			//if (b_loaded) {
			//	return true;
			//}

			//if (_load_pending) {
			//	return true;
			//}

			//_load_pending = true;

			//QTimer::singleShot(100, [this]() {
			//	_load_pending = false;
			//	QCoreApplication::quit(); // ��ɺ��˳�Ӧ�ó���
			//	});
			// �������ײ��������µ���ϵ��
			qDebug() << "load more contact user";
			//�����ź�֪ͨ���������ظ�����������
			emit sig_loading_contact_user();
		}
		return true; // ֹͣ�¼�����
	}
	return QListWidget::eventFilter(watched, event);
}

void ContactUserList::slot_item_clicked(QListWidgetItem* item) {
	QWidget* widget = this->itemWidget(item); // ��ȡ�Զ���widget����
	if (!widget) {
		qDebug() << "slot item clicked widget is nullptr";
		return;
	}

	// ���Զ���widget���в����� ��item ת��Ϊ����ListItemBase
	ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
	if (!customItem) {
		qDebug() << "slot item clicked widget is nullptr";
		return;
	}

	auto itemType = customItem->GetItemType();
	if (itemType == ListItemType::INVALID_ITEM
		|| itemType == ListItemType::GROUP_TIP_ITEM) {
		qDebug() << "slot invalid item clicked ";
		return;
	}

	if (itemType == ListItemType::APPLY_FRIEND_ITEM) {
		// �����Ի�����ʾ�û�
		qDebug() << "apply friend item clicked ";
		//��ת�������������
		emit sig_switch_apply_friend_page();
		return;
	}

	if (itemType == ListItemType::CONTACT_USER_ITEM) {
		// �����Ի�����ʾ�û�
		qDebug() << "contact user item clicked ";

		auto con_item = qobject_cast<ConUserItem*>(customItem);
		auto user_info = con_item->GetInfo();
		//��ת�������������
		emit sig_switch_friend_info_page();
		return;
	}
}

void ContactUserList::slot_add_auth_firend(std::shared_ptr<AuthInfo> auth_info) {
	qDebug() << "slot add auth friend ";
	bool isFriend = UserMgr::GetInstance()->CheckFriendById(auth_info->_uid);
	if (isFriend) {
		return;
	}
	// �� groupitem ֮���������
	int randomValue = QRandomGenerator::global()->bounded(100); // ����0��99֮����������
	int str_i = randomValue % strs.size();
	int head_i = randomValue % heads.size();

	auto* con_user_wid = new ConUserItem();
	con_user_wid->SetInfo(auth_info);
	QListWidgetItem* item = new QListWidgetItem;
	//qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
	item->setSizeHint(con_user_wid->sizeHint());

	// ��ȡ groupitem ������
	int index = this->row(_groupitem);
	// �� groupitem ֮���������
	this->insertItem(index + 1, item);

	this->setItemWidget(item, con_user_wid);

}

void ContactUserList::slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp) {
	qDebug() << "slot auth rsp called";
	bool isFriend = UserMgr::GetInstance()->CheckFriendById(auth_rsp->_uid);
	if (isFriend) {
		return;
	}
	// �� groupitem ֮���������
	int randomValue = QRandomGenerator::global()->bounded(100); // ����0��99֮����������
	int str_i = randomValue % strs.size();
	int head_i = randomValue % heads.size();

	auto* con_user_wid = new ConUserItem();
	con_user_wid->SetInfo(auth_rsp->_uid, auth_rsp->_name, heads[head_i]);
	QListWidgetItem* item = new QListWidgetItem;
	//qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
	item->setSizeHint(con_user_wid->sizeHint());

	// ��ȡ groupitem ������
	int index = this->row(_groupitem);
	// �� groupitem ֮���������
	this->insertItem(index + 1, item);

	this->setItemWidget(item, con_user_wid);

}

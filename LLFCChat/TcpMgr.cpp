#include "tcpmgr.h"
#include <QAbstractSocket>
#include <QJsonDocument>
#include "usermgr.h"

TcpMgr::TcpMgr() : _host(""), _port(0), _b_recv_pending(false), _message_id(0), _message_len(0) {
	QObject::connect(&_socket, &QTcpSocket::connected, [&]() {
		qDebug() << "Connected to server!";
		// ���ӽ���������Ϣ
		emit sig_con_success(true);
		});
	QObject::connect(&_socket, &QTcpSocket::readyRead, [&] {
		// �������ݿɶ�ʱ����ȡ��������
		// ��ȡ�������ݲ�׷�ӵ�������
		_buffer.append(_socket.readAll());
		QDataStream stream(&_buffer, QIODevice::ReadOnly);
		stream.setVersion(QDataStream::Qt_6_0);

		forever{
			//�Ƚ���ͷ��
			if (!_b_recv_pending) {
				// ��黺�����е������Ƿ��㹻������һ����Ϣͷ����ϢID + ��Ϣ���ȣ�
				if (_buffer.size() < static_cast<int>(sizeof(quint16) * 2)) {
					return;//���ݲ������ȴ���������
				}
				//Ԥ��ȡ��Ϣid����Ϣ���ȣ������ӻ��������Ƴ�
				stream >> _message_id >> _message_len;
				//��buffer�е�ǰ�ĸ��ֽ��Ƴ�
				_buffer = _buffer.mid(sizeof(quint16) * 2);
				//�����ȡ������
				qDebug() << "Message ID:" << _message_id << ", Length:" << _message_len;
			}

			//bufferʣ�೤���Ƿ�������Ϣ�峤�ȣ����������˳������ȴ�����
			if (_buffer.size() < _message_len) {
				_b_recv_pending = true;
				return;
			}
			_b_recv_pending = false;

			// ��ȡ��Ϣ��
			QByteArray messageBody = _buffer.mid(0, _message_len);
			qDebug() << "receive body msg is " << messageBody;

			_buffer = _buffer.mid(_message_len);
			handleMsg(RegId(_message_id), _message_len, messageBody);//���ݲ����ҵ���Ӧ�Ļص�����
		}
	});

	//�������
	QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
		Q_UNUSED(socketError)
		qDebug() << "Error:" << _socket.errorString();
	});

	// �������ӶϿ�
	QObject::connect(&_socket, &QTcpSocket::disconnected, [&]() {
		qDebug() << "Disconnected from server.";
		});
	//���ӷ����ź�������������
	QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
	//ע����Ϣ
	initHandlers();
}

TcpMgr::~TcpMgr() {

}

void TcpMgr::initHandlers() {
	_handlers.insert(ID_CHAT_LOGIN_RSP, [this](RegId id, int len, QByteArray data) {
		Q_UNUSED(len);
		qDebug() << "handle id is " << id;
		// ��QByteArrayת��ΪQJsonDocument
		QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

		//���ת���Ƿ�ɹ�
		if (jsonDoc.isNull()) {
			qDebug() << "Failed to create QJsonDocument.";
			return;
		}

		QJsonObject jsonObj = jsonDoc.object();
		qDebug() << "data jsonobj is " << jsonObj;
		
		if (!jsonObj.contains("error")) {
			int err = ErrorCodes::ERR_JSON;
			qDebug() << "Login Failed, err is Json Parse Err" << err;
			emit sig_login_failed(err);
			return;
		}

		int err = jsonObj["error"].toInt();
		if (err != ErrorCodes::SUCCESS) {
			qDebug() << "Login Failed, err is " << err;
			emit sig_login_failed(err);
			return;
		}

		auto uid = jsonObj["uid"].toInt();
		auto name = jsonObj["name"].toString();
		auto nick = jsonObj["nick"].toString();
		auto icon = jsonObj["icon"].toString();
		auto sex = jsonObj["sex"].toInt();
		auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex);

		UserMgr::GetInstance()->SetUserInfo(user_info);
		UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());
		if (jsonObj.contains("apply_list")) {
			UserMgr::GetInstance()->AppendApplyList(jsonObj["apply_list"].toArray());
		}

		//��Ӻ����б�
		if (jsonObj.contains("friend_list")) {
			UserMgr::GetInstance()->AppendFriendList(jsonObj["friend_list"].toArray());
		}

		emit sig_swich_chatdlg();
	});

	_handlers.insert(ID_SEARCH_USER_RSP, [this](RegId id, int len, QByteArray data) {
		Q_UNUSED(len);
		qDebug() << "handle id is " << id << " data is " << data;
		// ��QByteArrayת��ΪQJsonDocument
		QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

		// ���ת���Ƿ�ɹ�
		if (jsonDoc.isNull()) {
			qDebug() << "Failed to create QJsonDocument.";
			return;
		}

		QJsonObject jsonObj = jsonDoc.object();

		if (!jsonObj.contains("error")) {
			int err = ErrorCodes::ERR_JSON;
			qDebug() << "Login Failed, err is Json Parse Err" << err;

			emit sig_user_search(nullptr);
			return;
		}

		int err = jsonObj["error"].toInt();
		if (err != ErrorCodes::SUCCESS) {
			qDebug() << "Login Failed, err is " << err;
			emit sig_user_search(nullptr);
			return;
		}
		auto search_info = std::make_shared<SearchInfo>(jsonObj["uid"].toInt(), 
			jsonObj["name"].toString(), jsonObj["nick"].toString(), 
			jsonObj["desc"].toString(), jsonObj["sex"].toInt(), jsonObj["icon"].toString());

		emit sig_user_search(search_info);
		});

	_handlers.insert(ID_ADD_FRIEND_RSP, [this](RegId id, int len, QByteArray data) {
		Q_UNUSED(len);
		qDebug() << "handle id is " << id << " data is " << data;
		// ��QByteArrayת��ΪQJsonDocument
		QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

		// ���ת���Ƿ�ɹ�
		if (jsonDoc.isNull()) {
			qDebug() << "Failed to create QJsonDocument.";
			return;
		}

		QJsonObject jsonObj = jsonDoc.object();

		if (!jsonObj.contains("error")) {
			int err = ErrorCodes::ERR_JSON;
			qDebug() << "Add Friend Failed, err is Json Parse Err" << err;
			return;
		}

		int err = jsonObj["error"].toInt();
		if (err != ErrorCodes::SUCCESS) {
			qDebug() << "Add Friend Failed, err is " << err;
			return;
		}

		qDebug() << "Add Friend Success ";
		});

	_handlers.insert(ID_NOTIFY_ADD_FRIEND_REQ, [this](RegId id, int len, QByteArray data) {
		Q_UNUSED(len);
		qDebug() << "handle id is " << id << " data is " << data;
		// ��QByteArrayת��ΪQJsonDocument
		QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

		// ���ת���Ƿ�ɹ�
		if (jsonDoc.isNull()) {
			qDebug() << "Failed to create QJsonDocument.";
			return;
		}

		QJsonObject jsonObj = jsonDoc.object();

		if (!jsonObj.contains("error")) {
			int err = ErrorCodes::ERR_JSON;
			qDebug() << "Login Failed, err is Json Parse Err" << err;

			emit sig_user_search(nullptr);
			return;
		}

		int err = jsonObj["error"].toInt();
		if (err != ErrorCodes::SUCCESS) {
			qDebug() << "Login Failed, err is " << err;
			emit sig_user_search(nullptr);
			return;
		}

		int from_uid = jsonObj["applyuid"].toInt();
		QString name = jsonObj["name"].toString();
		QString desc = jsonObj["desc"].toString();
		QString icon = jsonObj["icon"].toString();
		QString nick = jsonObj["nick"].toString();
		int sex = jsonObj["sex"].toInt();

		auto apply_info = std::make_shared<AddFriendApply>(
			from_uid, name, desc,
			icon, nick, sex);

		emit sig_friend_apply(apply_info);
		});

	_handlers.insert(ID_NOTIFY_AUTH_FRIEND_REQ, [this](RegId id, int len, QByteArray data) {
		Q_UNUSED(len);
		qDebug() << "handle id is " << id << " data is " << data;
		// ��QByteArrayת��ΪQJsonDocument
		QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

		// ���ת���Ƿ�ɹ�
		if (jsonDoc.isNull()) {
			qDebug() << "Failed to create QJsonDocument.";
			return;
		}

		QJsonObject jsonObj = jsonDoc.object();
		if (!jsonObj.contains("error")) {
			int err = ErrorCodes::ERR_JSON;
			qDebug() << "Auth Friend Failed, err is " << err;
			return;
		}

		int err = jsonObj["error"].toInt();
		if (err != ErrorCodes::SUCCESS) {
			qDebug() << "Auth Friend Failed, err is " << err;
			return;
		}

		int from_uid = jsonObj["fromuid"].toInt();
		QString name = jsonObj["name"].toString();
		QString nick = jsonObj["nick"].toString();
		QString icon = jsonObj["icon"].toString();
		int sex = jsonObj["sex"].toInt();

		auto auth_info = std::make_shared<AuthInfo>(from_uid, name,
			nick, icon, sex);

		emit sig_add_auth_friend(auth_info);
		});

	_handlers.insert(ID_AUTH_FRIEND_RSP, [this](RegId id, int len, QByteArray data) {
		Q_UNUSED(len);
		qDebug() << "handle id is " << id << " data is " << data;
		// ��QByteArrayת��ΪQJsonDocument
		QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

		// ���ת���Ƿ�ɹ�
		if (jsonDoc.isNull()) {
			qDebug() << "Failed to create QJsonDocument.";
			return;
		}

		QJsonObject jsonObj = jsonDoc.object();

		if (!jsonObj.contains("error")) {
			int err = ErrorCodes::ERR_JSON;
			qDebug() << "Auth Friend Failed, err is Json Parse Err" << err;
			return;
		}

		int err = jsonObj["error"].toInt();
		if (err != ErrorCodes::SUCCESS) {
			qDebug() << "Auth Friend Failed, err is " << err;
			return;
		}

		auto name = jsonObj["name"].toString();
		auto nick = jsonObj["nick"].toString();
		auto icon = jsonObj["icon"].toString();
		auto sex = jsonObj["sex"].toInt();
		auto uid = jsonObj["uid"].toInt();
		auto rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);

		emit sig_auth_rsp(rsp);

		qDebug() << "Auth Friend Success ";
		});


}

void TcpMgr::handleMsg(RegId id, int len, QByteArray data) {
	auto find_iter = _handlers.find(id);
	if (find_iter == _handlers.end()) {
		qDebug() << "not found id [" << id << "] to handle";
		return;
	}

	find_iter.value()(id, len, data);
}

void TcpMgr::slot_tcp_connect(ServerInfo si) {
	qDebug() << "receive tcp connect signal";
	// �������ӵ�������
	qDebug() << "Connecting to server...";
	_host = si.Host;
	_port = static_cast<uint16_t>(si.Port.toUInt());
	_socket.connectToHost(si.Host, _port);
}

void TcpMgr::slot_send_data(RegId reqId, QByteArray dataBytes) {
    uint16_t id = reqId;

    // ���㳤�ȣ�ʹ�������ֽ���ת����
    quint16 len = static_cast<quint16>(dataBytes.length());

    // ����һ��QByteArray���ڴ洢Ҫ���͵���������
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    // ����������ʹ�������ֽ���
    out.setByteOrder(QDataStream::BigEndian);

    // д��ID�ͳ���
    out << id << len;

    // ����ַ�������
    block.append(dataBytes);

    // ��������
    _socket.write(block);
    qDebug() << "tcp mgr send byte data is " << block;
}

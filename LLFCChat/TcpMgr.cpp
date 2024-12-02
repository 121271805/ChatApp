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

        UserMgr::GetInstance()->SetUid(jsonObj["uid"].toInt());
        UserMgr::GetInstance()->SetName(jsonObj["name"].toString());
        UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());

		emit sig_swich_chatdlg();
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

void TcpMgr::slot_send_data(RegId reqId, QString data) {
    uint16_t id = reqId;

    QByteArray dataBytes = data.toUtf8();

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

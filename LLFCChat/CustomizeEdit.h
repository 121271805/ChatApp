#pragma once
#include <QLineEdit>
#include <QDebug>

/******************************************************************************
 *
 * @file       CustomizeEdit.h
 * @brief      �Զ��������
 *
 * @author     Carpe_Diem
 * @date       2024/12/04
 * @history
 *****************************************************************************/
class CustomizeEdit : public QLineEdit {
	Q_OBJECT
public:
	CustomizeEdit(QWidget* parent = nullptr);
	void SetMaxLength(int maxLen);
protected:
	//ִ��ʧȥ����ʱ�Ĵ����߼�
	void focusOutEvent(QFocusEvent* event) override {
		QLineEdit::focusOutEvent(event);
		//����ʧȥ������ź�
		emit sig_focus_out();
	}
private:
	//�������볤��
	void limitTextLength(QString text) {
		if (_max_len <= 0) {
			return;
		}
		QByteArray byteArray = text.toUtf8();
		if (byteArray.size() > _max_len) {
			byteArray = byteArray.left(_max_len);
			this->setText(QString::fromUtf8(byteArray));
		}
	}
	int _max_len;
signals:
	void sig_focus_out();
};


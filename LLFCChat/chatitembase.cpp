#include "chatitembase.h"
#include <QFont>
#include <QVBoxLayout>
#include "BubbleFrame.h"

ChatItemBase::ChatItemBase(ChatRole role, QWidget* parent) 
	: QWidget(parent), m_role(role) {
	//名字
	m_pNameLabel = new QLabel();
	m_pNameLabel->setObjectName("chat_user_name");
	QFont font("Microsoft YaHei");
	font.setPointSize(9);
	m_pNameLabel->setFont(font);
	m_pNameLabel->setFixedHeight(20);

	//头像
	m_pIconLabel = new QLabel();
	m_pIconLabel->setScaledContents(true);
	m_pIconLabel->setFixedSize(42, 42);

	//气泡
	m_pBubble = new QWidget();

	//网格布局
	QGridLayout* pGLayout = new QGridLayout();
	pGLayout->setVerticalSpacing(3);
	pGLayout->setHorizontalSpacing(3);
	pGLayout->setContentsMargins(3, 3, 3, 3);

	//弹簧
	QSpacerItem* pSpacer = new QSpacerItem(40, 20, 
		QSizePolicy::Expanding, QSizePolicy::Minimum);

	if (m_role == ChatRole::Self) {//发出的记录
		m_pNameLabel->setContentsMargins(0, 0, 8, 0);
		m_pNameLabel->setAlignment(Qt::AlignRight);
		//位于组件的第零行第一列，占用一行一列
		pGLayout->addWidget(m_pNameLabel, 0, 1, 1, 1);
		//位于组件的第零行第二列，占用两行一列，且顶部对齐
		pGLayout->addWidget(m_pIconLabel, 0, 2, 2, 1, Qt::AlignTop);
		pGLayout->addItem(pSpacer, 1, 0, 1, 1);
		pGLayout->addWidget(m_pBubble, 1, 1, 1, 1);
		//拉伸比例，第零行占比为2，第一列占比为3
		pGLayout->setColumnStretch(0, 2);
		pGLayout->setColumnStretch(1, 3);
	} else {//接收的记录
		m_pNameLabel->setContentsMargins(8, 0, 0, 0);
		m_pNameLabel->setAlignment(Qt::AlignLeft);
		pGLayout->addWidget(m_pIconLabel, 0, 0, 2, 1, Qt::AlignTop);
		pGLayout->addWidget(m_pNameLabel, 0, 1, 1, 1);
		pGLayout->addWidget(m_pBubble, 1, 1, 1, 1);
		pGLayout->addItem(pSpacer, 2, 2, 1, 1);
		pGLayout->setColumnStretch(1, 3);
		pGLayout->setColumnStretch(2, 2);
	}
	this->setLayout(pGLayout);
}

void ChatItemBase::setUserName(const QString& name) {
	m_pNameLabel->setText(name);
}

void ChatItemBase::setUserIcon(const QPixmap& icon) {
	m_pIconLabel->setPixmap(icon);
}

void ChatItemBase::setWidget(QWidget* w) {
	QGridLayout* pGLayout = (qobject_cast<QGridLayout*>)(this->layout());
	pGLayout->replaceWidget(m_pBubble, w);
	delete m_pBubble;
	m_pBubble = w;
}
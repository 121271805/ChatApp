#include "textbubble.h"
#include <QFontMetricsF>
#include <QDebug>
#include <QFont>
#include "global.h"
#include <QTimer>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextLayout>
#include <QFont>

TextBubble::TextBubble(ChatRole role, const QString& text, QWidget* parent)
	:BubbleFrame(role, parent) {
	m_pTextEdit = new QTextEdit();
	m_pTextEdit->setReadOnly(true);
	m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pTextEdit->installEventFilter(this);
	QFont font("Microsoft YaHei");
	font.setPointSize(12);
	m_pTextEdit->setFont(font);
	setPlainText(text);
	setWidget(m_pTextEdit);
	initStyleSheet();
}

bool TextBubble::eventFilter(QObject* o, QEvent* e) {
	//�������ڴ�Сʱ�����»������ݣ���TextEdit����Paint�¼�ʱ�����ı���߶�
	if (m_pTextEdit == o && e->type() == QEvent::Paint) {
		adjustTextHeight(); //PaintEvent������
	}
	return BubbleFrame::eventFilter(o, e);
}

void TextBubble::setPlainText(const QString& text) {
	m_pTextEdit->setPlainText(text);
	//m_pTextEdit->setHtml(text);
	//�ҵ������������
	qreal doc_margin = m_pTextEdit->document()->documentMargin();
	int margin_left = this->layout()->contentsMargins().left();
	int margin_right = this->layout()->contentsMargins().right();
	QFontMetricsF fm(m_pTextEdit->font());
	QTextDocument* doc = m_pTextEdit->document();
	int max_width = 0;
	//����ÿһ���ҵ� ������һ��
	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())//�����ܳ�
	{
		int txtW = int(fm.horizontalAdvance(it.text()));
		max_width = max_width < txtW ? txtW : max_width;//�ҵ�����Ƕ�
	}
	//����������ݵ������ ֻ��Ҫ����һ��
	//����qt6��width()����Ϊ��horizontalAdvance()����Ҫ�ں����1
	setMaximumWidth(max_width + doc_margin * 2 + (margin_left + margin_right) + 1);//���������
}

void TextBubble::adjustTextHeight() {
	qreal doc_margin = m_pTextEdit->document()->documentMargin();//���嵽�߿�ľ���Ĭ��Ϊ4
	QTextDocument* doc = m_pTextEdit->document();
	qreal text_height = 0;
	//��ÿһ�εĸ߶����=�ı���
	for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next()) {
		QTextLayout* pLayout = it.layout();
		QRectF text_rect = pLayout->boundingRect();//��ε�rect
		text_height += text_rect.height();
	}
	int vMargin = this->layout()->contentsMargins().top();
	//�������������Ҫ�ĸ߶� �ı���+�ı��߾�+TextEdit�߿����ݱ߿�ľ���
	setFixedHeight(text_height + doc_margin * 2 + vMargin * 2);
}

void TextBubble::initStyleSheet() {
	m_pTextEdit->setStyleSheet("QTextEdit{background:transparent;border:none}");
}
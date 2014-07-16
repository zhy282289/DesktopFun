#include "stdafx.h"
#include "item.h"
#include "data.h"


Item::Item(QWidget *parent)
	: QWidget(parent)
{
	m_bLeftBtnDown = false;
	m_bWindowHadMove = false;
	m_bMouseEnter = false;
	m_bgColor = QColor(0, 250, 0, 100);
	//m_bItemCanMove = true;

	m_itemPixmap = new ItemPixmap(this);
	m_itemText = new ItemText(this);
	
	m_actRemove = new QAction("�Ƴ�", this);
	m_actLocateDirectory = new QAction("��λ���ļ���", this);
	connect(m_actRemove, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actLocateDirectory, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));



	QVBoxLayout *vlayout = new QVBoxLayout;
	vlayout->addWidget(m_itemPixmap);
	vlayout->addWidget(m_itemText);
	vlayout->setStretch(0, 100);
	vlayout->setContentsMargins(5,5,5,5);
	setLayout(vlayout);
}

Item::~Item()
{

}

void Item::paintEvent( QPaintEvent *event )
{
	if (m_bMouseEnter)
	{
		QPainter painter(this);
		painter.fillRect(rect(), QBrush(m_bgColor));
	}

}

void Item::mousePressEvent( QMouseEvent *event )
{
	raise();
	if (event->button() == Qt::LeftButton)
	{
		m_bLeftBtnDown = true;
		if (m_data.bItemCanMove)
		{
			m_oldPoint = event->pos();
		}
		
	}
}

void Item::mouseReleaseEvent( QMouseEvent *event )
{
	m_bLeftBtnDown = false;

	if (!m_bWindowHadMove)
	{
		if (event->button() == Qt::LeftButton && geometry().contains(mapToParent(event->pos())))
		{
			emit SigLeftMouseClicked(m_data.path);
		}
		else if(event->button() == Qt::RightButton)
		{
			emit SigRightMouseClicked();
		}
		
	}
	else
	{
		emit SigItemHadMove();
	}

	m_bWindowHadMove = false;

}

void Item::mouseMoveEvent( QMouseEvent *event )
{
	if (m_bLeftBtnDown && m_data.bItemCanMove)
	{
		QPoint newPoint = /*mapToParent*/(m_oldPoint);
		move(mapToParent(event->pos()) - newPoint);
		m_oldPoint = newPoint;
		m_bWindowHadMove = true;
		m_data.rect.setTopLeft(m_oldPoint);
	}
}

void Item::SetData( const ItemData &data )
{
	m_data = data;
	m_itemText->SetText(QFileInfo(m_data.path).baseName());
	m_itemPixmap->SetPixmap(m_data.pix);
}

void Item::UpdatePostion()
{
	show();
	setGeometry(m_data.rect);
}

void Item::enterEvent( QEvent *event )
{
	m_bMouseEnter = true;
	update();
	QApplication::setActiveWindow(parentWidget());
}

void Item::leaveEvent( QEvent *event )
{
	m_bMouseEnter = false;
	update();
}

void Item::SlotActTriggered()
{
	QObject *obj = sender();
	if (m_actRemove == obj)
	{
		emit SigItemMenu(0);
	}
	else if (m_actLocateDirectory == obj)
	{
		OpenExplorerAndSelectFile(m_data.path);
	}
}

void Item::contextMenuEvent( QContextMenuEvent *event )
{
	QMenu menu(this);
	menu.addAction(m_actRemove);
	menu.addAction(m_actLocateDirectory);
	menu.exec(event->globalPos());
}

ItemData Item::GetData()
{
	return m_data;
}

void Item::SetCanMove( bool canMove )
{
	m_data.bItemCanMove = canMove;
}

bool Item::GetCanMove()
{
	return m_data.bItemCanMove;
}

void Item::SetBGColor( QColor color )
{
	m_bgColor = color;
	update();
}

ItemText::ItemText( QWidget *parent )
	:QLabel(parent)
{
	setFixedHeight(22);
	//setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	setStyleSheet("ItemText{font-size: 14px;font-style:bold;color:rgb(200,200,200)}");
}

void ItemText::SetText( const QString &text )
{
	QString showText = text;
	QFontMetrics metrics(font());
	int w = metrics.width(showText);
	if ( w > 60)
	{
		setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		showText = (showText.left(showText.size()-3) + QString(".."));
	}
	else
	{
		setAlignment(Qt::AlignCenter);
	}

	setText(showText);
	setToolTip(text);
}

ItemPixmap::ItemPixmap( QWidget *parent )
	:QLabel(parent)
{

}

void ItemPixmap::SetPixmap( const QPixmap &pixmap )
{
	m_pixmap = pixmap;
}

void ItemPixmap::paintEvent( QPaintEvent *event )
{
	if (!m_pixmap.isNull())
	{
		QPainter painter(this);
		QRect rect = this->rect();
		int minLen = 0;
		if (rect.width() > rect.height())
		{
			rect.setX((rect.width()-rect.height())/2);
			rect.setWidth(rect.height());
		}
		else
		{
			rect.setY((rect.height()-rect.width())/2);
			rect.setHeight(rect.width());
		}
		painter.drawPixmap(rect, m_pixmap);
	}

}



QDataStream &operator<<(QDataStream &out, const ItemData &data)
{
	return out << data.path << data.pix << data.rect << data.bItemCanMove;
}

QDataStream &operator>>(QDataStream &in, ItemData &data)
{
	return in >> data.path >> data.pix >> data.rect >> data.bItemCanMove;
}

//////////////////////////////////////////////////////////////////////////
AboutDlg::AboutDlg( QWidget *parent /*= NULL*/ )
	:QLabel(parent)
{
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_TranslucentBackground);

}


void AboutDlg::mousePressEvent( QMouseEvent *event )
{
	close();
}

void AboutDlg::paintEvent( QPaintEvent *event )
{
	QPainter painter(this);
	const QString text("���ڴ�App:\n"
		"��������Ȩ��: BOY ����\n"
		"QQ:3969622**\n"
		"IP:159995108**\n"
		"ԭ������һ������ݰ�����");
	painter.fillRect(rect(), QBrush(QColor(0,250,250)));
	painter.setPen(QPen(QColor(250,0,0,150)));
	painter.drawText(rect(), Qt::AlignCenter, text);
}


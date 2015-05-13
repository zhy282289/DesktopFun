#include "stdafx.h"
#include "item.h"
#include "data.h"
#include "settingsdlg.h"


ItemEx::ItemEx(ItemData *data, QColor bgColor,bool *canMove,  QWidget *parent)
	: QWidget(parent)
	, m_data(data)
	,m_canMove(canMove)
	,m_bgColor(bgColor)
{
	m_bLeftBtnDown = false;
	m_bWindowHadMove = false;
	m_bMouseEnter = false;
	m_bgColor = QColor(0, 250, 0, 100);
	//m_bItemCanMove = true;

	m_itemPixmap = new ItemPixmap(this);
	m_itemText = new ItemText(this);
	m_itemText->SetText(QFileInfo(m_data->path).baseName());
	m_itemPixmap->SetPixmap(m_data->pix);	

	QVBoxLayout *vlayout = new QVBoxLayout;
	vlayout->addWidget(m_itemPixmap);
	vlayout->addWidget(m_itemText);
	vlayout->setStretch(0, 100);
	vlayout->setContentsMargins(5,5,5,5);
	setLayout(vlayout);
}

ItemEx::~ItemEx()
{

}

void ItemEx::paintEvent( QPaintEvent *event )
{
	if (m_bMouseEnter)
	{
		QPainter painter(this);
		painter.fillRect(rect(), QBrush(m_bgColor));
	}

}

void ItemEx::mousePressEvent( QMouseEvent *event )
{
	raise();
	if (event->button() == Qt::LeftButton)
	{
		m_bLeftBtnDown = true;
		if (*m_canMove)
			m_oldPoint = event->pos();
		
	}
}

void ItemEx::mouseReleaseEvent( QMouseEvent *event )
{
	m_bLeftBtnDown = false;
	if (!m_bWindowHadMove)
	{
		if (event->button() == Qt::LeftButton && geometry().contains(mapToParent(event->pos())))
			QDesktopServices::openUrl(QUrl::fromLocalFile(m_data->path));
	}
	else
		emit WindowStateChanged();
	
	m_bWindowHadMove = false;
}

void ItemEx::mouseMoveEvent( QMouseEvent *event )
{
	if (m_bLeftBtnDown && *m_canMove)
	{
		QPoint newPoint = /*mapToParent*/(m_oldPoint);
		move(mapToParent(event->pos()) - newPoint);
		m_oldPoint = newPoint;
		m_bWindowHadMove = true;
		m_data->rect = this->geometry();
	}
}


void ItemEx::SetGeometry()
{
	show();
	setGeometry(m_data->rect);
}

void ItemEx::enterEvent( QEvent *event )
{
	m_bMouseEnter = true;
	update();
	QApplication::setActiveWindow(parentWidget());
}

void ItemEx::leaveEvent( QEvent *event )
{
	m_bMouseEnter = false;
	update();
}

void ItemEx::contextMenuEvent( QContextMenuEvent *event )
{
	QMenu menu(this);
	QAction *actRemove = menu.addAction(tr("Remove"));
	QAction *actLocateDirectory = menu.addAction(tr("Location"));
	QAction *act = menu.exec(event->globalPos());
	if (act == actRemove)
	{
		hide();
		deleteLater();
		emit DeleteLater(m_data->id);
		emit WindowStateChanged();
	}
	else if (act == actLocateDirectory)
		OpenExplorerAndSelectFile(m_data->path);

}

//////////////////////////////////////////////////////////////////////////
ItemText::ItemText( QWidget *parent )
	:QLabel(parent)
{
	setFixedHeight(22);
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



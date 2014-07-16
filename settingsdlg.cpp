#include "stdafx.h"
#include "settingsdlg.h"

SettingsDlg::SettingsDlg(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("设置");
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	resize(680, 590);

	m_dialogTitle = new DialogTitle(this);	
	m_sliderOpacity = new QSlider(Qt::Horizontal, this);
	m_sliderOpacity->setRange(0, 255);
	m_btnColor = new ColorBtn(this);
	m_btnColor->setMinimumSize(100, 22);
	m_btnColor->SetColor(QColor(250, 0, 250));
	m_btnBgPix = new PixmapBtn(this);
	//m_btnBgPix->setMinimumSize(500, 250);
	m_btnBgPix->SetPixmap(QueryDesktopPixmap());

	

	connect(m_sliderOpacity, SIGNAL(valueChanged(int)), this, SLOT(SlotSliderValueChanged(int)));
	connect(m_btnColor, SIGNAL(clicked()), this, SLOT(SlotTriggered()));
	connect(m_btnBgPix, SIGNAL(clicked()), this, SLOT(SlotTriggered()));

	QPushButton *btnClose = new QPushButton("关闭", this);
	btnClose->setFixedWidth(65);
	connect(btnClose, SIGNAL(clicked()), this, SLOT(close()));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(m_sliderOpacity);
	mainLayout->addWidget(m_btnColor);
	mainLayout->addWidget(m_btnBgPix);
	mainLayout->addWidget(btnClose, 0, Qt::AlignRight);
	//mainLayout->addWidget(new QWidget(this), 100);
	mainLayout->setStretch(2, 100);
	const int margins = 20;
	mainLayout->setContentsMargins(margins, margins, margins, margins);
	m_layoutWidget = new QWidget(this);
	m_layoutWidget->setLayout(mainLayout);
}

SettingsDlg::~SettingsDlg()
{

}

void SettingsDlg::Show(const SettingData &data)
{
	m_data = data;
	m_sliderOpacity->setValue(m_data.color.alpha());
	m_btnColor->SetColor(m_data.color);
	m_btnBgPix->SetPixmap(m_data.bgPix.bgPixmap);
	show();
}

void SettingsDlg::SlotTriggered()
{
	if (sender() == m_btnColor)
	{
		QColorDialog dlg(this);
		if (dlg.exec())
		{
			int alpha = m_data.color.alpha();
			m_data.color = dlg.selectedColor();
			m_data.color.setAlpha(alpha);
			m_btnColor->SetColor(m_data.color);
			emit SigSettingChanged(m_data);
		}
	}
	else if (sender() == m_btnBgPix)
	{
		QString pixmapPath = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, "select pixmap", "./", "Images (*.*)"));
		if (!pixmapPath.isEmpty())
		{
			QImage image;
			bool bimage = image.load(pixmapPath);
			m_data.bgPix.bgPixmap = QPixmap::fromImage(image);
			m_data.bgPix.path = pixmapPath;
			m_btnBgPix->SetPixmap(m_data.bgPix.bgPixmap);
			emit SigSettingChanged(m_data);
		}
	}
}

void SettingsDlg::SlotSliderValueChanged( int value )
{
	m_data.color.setAlpha(m_sliderOpacity->value());
	m_btnColor->SetColor(m_data.color);
	emit SigSettingChanged(m_data);
}

void SettingsDlg::paintEvent( QPaintEvent *event )
{
	QPainter painter(this);
	painter.fillRect(rect(), QBrush(QColor(255, 255, 255)));
}

void SettingsDlg::resizeEvent( QResizeEvent *event )
{
	m_dialogTitle->setGeometry(0, 0, rect().width(), 22);
	m_layoutWidget->setGeometry(0, m_dialogTitle->geometry().bottom(), rect().width(), rect().height()-22);
}


ColorBtn::ColorBtn( QWidget *parent /*= NULL*/ )
	:QWidget(parent)
{

}

void ColorBtn::SetColor( QColor color )
{
	m_color = color;
	update();
}

QColor ColorBtn::GetColor()
{
	return m_color;
}

void ColorBtn::paintEvent( QPaintEvent *event )
{
	QPainter painter(this);
	painter.drawText(rect(), Qt::AlignCenter, "选择背景颜色");
	painter.fillRect(rect(), QBrush(m_color));
}

void ColorBtn::mouseReleaseEvent( QMouseEvent *event )
{
	emit clicked();
}

PixmapBtn::PixmapBtn( QWidget *parent /*= NULL*/ )
	:QWidget(parent)
{

}

void PixmapBtn::SetPixmap( QPixmap pixmap )
{
	m_pixmap = pixmap;
	update();
}

QPixmap PixmapBtn::GetPixmap()
{
	return m_pixmap;
}

void PixmapBtn::paintEvent( QPaintEvent *event )
{
	QPainter painter(this);
	painter.fillRect(rect(), QBrush(QColor(250, 120, 120, 50)));
	painter.drawText(rect(), Qt::AlignCenter, "选择背景图片");
	QPixmap pixmap = m_pixmap.scaled(rect().size(), Qt::KeepAspectRatio);
	painter.drawPixmap(pixmap.rect(), pixmap, pixmap.rect());
}

void PixmapBtn::mouseReleaseEvent( QMouseEvent *event )
{
	emit clicked();
}

//////////////////////////////////////////////////////////////////////////
DialogTitle::DialogTitle( QWidget *parent /*= NULL*/ )
	:QLabel(parent)
{
	m_bLeftBtnDown = false;
	setStyleSheet("DialogTitle{background-color:rgb(250,80,200)}");
}

void DialogTitle::mousePressEvent( QMouseEvent *event )
{
	m_bLeftBtnDown = true;
	m_oldPoint = event->pos();
}

void DialogTitle::mouseReleaseEvent( QMouseEvent *event )
{
	m_bLeftBtnDown = false;
}

void DialogTitle::mouseMoveEvent( QMouseEvent *event )
{
	if (m_bLeftBtnDown)
	{
		QWidget *parentWidget = this->parentWidget();
		parentWidget->move(event->globalPos() - m_oldPoint);
	}
}

//////////////////////////////////////////////////////////////////////////
AddFilesOrDirectoryDlg::AddFilesOrDirectoryDlg( QWidget *parent /*= NULL*/ )
	:QDialog(parent)
{
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	resize(750, 450);
	m_fileDlg = new QFileDialog(this, Qt::SubWindow);
	m_fileDlg->setFixedSize(750, 450-22);
	connect(m_fileDlg, SIGNAL(finished(int)), this, SLOT(SlotFinish(int)));
	
	m_dlgTitle = new DialogTitle(this);
}

QStringList AddFilesOrDirectoryDlg::AddFiles()
{
	m_dlgTitle->setText(" 选择文件");
	m_fileDlg->setAcceptMode(QFileDialog::AcceptOpen);
	m_fileDlg->setFileMode(QFileDialog::ExistingFiles);
	exec();
	return m_fileDlg->result() ? m_fileDlg->selectedFiles() : QStringList();
}

QString AddFilesOrDirectoryDlg::AddDirectory()
{
	m_dlgTitle->setText(" 选择文件夹");
	m_fileDlg->setAcceptMode(QFileDialog::AcceptOpen);
	m_fileDlg->setFileMode(QFileDialog::Directory);
	m_fileDlg->setOption(QFileDialog::ShowDirsOnly);
	exec();
	return m_fileDlg->result() ? m_fileDlg->selectedFiles().at(0) : "";
}

void AddFilesOrDirectoryDlg::resizeEvent( QResizeEvent *event )
{
	m_dlgTitle->setGeometry(0, 0, rect().width(), 22);
	m_fileDlg->setGeometry(0, 22, rect().width(), rect().height() - 22);
}



void AddFilesOrDirectoryDlg::SlotFinish(int code)
{
	done(code);
}

//////////////////////////////////////////////////////////////////////////
MsgBox::MsgBox( QWidget *parent /*= NULL*/ )
	:QDialog(parent)
{
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	resize(600, 270);

	m_lbText = new QLabel(this);
	m_lbText->setAlignment(Qt::AlignCenter);
	m_lbText->setStyleSheet("QLabel{background-color:rgb(250,120,120)}");
	m_btnYes = new QPushButton("确定", this);
	m_btnNo = new QPushButton("取消", this);
	connect(m_btnYes, SIGNAL(clicked()), this, SLOT(SlotBtnClicked()));
	connect(m_btnNo, SIGNAL(clicked()), this, SLOT(SlotBtnClicked()));
	
	m_dlgTitle = new DialogTitle(this);

	QVBoxLayout *vlayout = new QVBoxLayout;
	vlayout->addWidget(m_lbText);
	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addWidget(new QWidget(this));
	hlayout->addWidget(m_btnYes);
	hlayout->addWidget(m_btnNo);
	hlayout->setStretch(0, 100);
	vlayout->addLayout(hlayout);
	m_layoutWidget = new QWidget(this);
	m_layoutWidget->setLayout(vlayout);

	
}

void MsgBox::MsgWarning( const QString &title, const QString &text )
{
	m_dlgTitle->setText(" " + title);
	m_lbText->setText(text);
	exec();
}

bool MsgBox::MsgQuestion( const QString &title, const QString &text )
{
	m_dlgTitle->setText(" " + title);
	m_lbText->setText(text);
	return exec();
}

void MsgBox::resizeEvent( QResizeEvent *event )
{
	m_dlgTitle->setGeometry(0, 0, rect().width(), 22);
	m_layoutWidget->setGeometry(0, 22, rect().width(), rect().height() - 22);
}


void MsgBox::SlotBtnClicked()
{
	done(sender() == m_btnYes);
}

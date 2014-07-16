#include "stdafx.h"
#include "settingsdlg.h"

SettingsDlg::SettingsDlg(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("ÉèÖÃ");
	setAttribute(Qt::WA_DeleteOnClose);

	resize(680, 590);
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

	QPushButton *btnClose = new QPushButton("È·¶¨", this);
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
	setLayout(mainLayout);
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
	painter.drawText(rect(), Qt::AlignCenter, "Ñ¡Ôñ±³¾°ÑÕÉ«");
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
	painter.drawText(rect(), Qt::AlignCenter, "Ñ¡Ôñ±³¾°Í¼Æ¬");
	painter.drawPixmap(rect(), m_pixmap, m_pixmap.rect());
}

void PixmapBtn::mouseReleaseEvent( QMouseEvent *event )
{
	emit clicked();
}

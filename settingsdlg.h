#ifndef SETTINGSDLG_H
#define SETTINGSDLG_H

#include <QDialog>

struct BGPixmap
{
	QPixmap bgPixmap;
	QString path;
};
struct SettingData
{
	QColor color;
	BGPixmap bgPix;
};

class ColorBtn;
class PixmapBtn;
class SettingsDlg : public QDialog
{
	Q_OBJECT

public:
	SettingsDlg(QWidget *parent = 0);
	~SettingsDlg();

	void Show(const SettingData &data);

signals:
	void SigSettingChanged(SettingData);

private slots:
	void SlotTriggered();
	void SlotSliderValueChanged(int value);

protected:
	void paintEvent(QPaintEvent *event);

private:
	QSlider *m_sliderOpacity;
	ColorBtn *m_btnColor;
	PixmapBtn *m_btnBgPix;

	SettingData m_data;
};

class ColorBtn : public QWidget
{
	Q_OBJECT
public:
	ColorBtn(QWidget *parent = NULL);

	void SetColor(QColor color);
	QColor GetColor();

signals:
	void clicked();

protected:
	void paintEvent(QPaintEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	QColor m_color;
};

class PixmapBtn : public QWidget
{
	Q_OBJECT
public:
	PixmapBtn(QWidget *parent = NULL);

	void SetPixmap(QPixmap pixmap);
	QPixmap GetPixmap();

signals:
	void clicked();
protected:
	void paintEvent(QPaintEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	QPixmap m_pixmap;
};


#endif // SETTINGSDLG_H

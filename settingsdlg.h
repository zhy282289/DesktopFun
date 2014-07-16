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
class DialogTitle;
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
	void resizeEvent(QResizeEvent *event);
private:
	QSlider *m_sliderOpacity;
	ColorBtn *m_btnColor;
	PixmapBtn *m_btnBgPix;

	SettingData m_data;
	DialogTitle *m_dialogTitle;
	QWidget *m_layoutWidget;
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


class DialogTitle : public QLabel
{
	Q_OBJECT
public:
	DialogTitle(QWidget *parent = NULL);
	
protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);


private:
	bool	m_bLeftBtnDown;
	QPoint  m_oldPoint;
};


class AddFilesOrDirectoryDlg : public QDialog
{
	Q_OBJECT
public:
	AddFilesOrDirectoryDlg(QWidget *parent = NULL);

	QStringList AddFiles();
	QString AddDirectory();

private slots:
	void SlotFinish(int);

protected:
	void resizeEvent(QResizeEvent *event);

private:
	QFileDialog		*m_fileDlg;
	DialogTitle		*m_dlgTitle;
};


class MsgBox : public QDialog
{
	Q_OBJECT
public:
	MsgBox(QWidget *parent = NULL);

	void MsgWarning(const QString &title, const QString &text);
	bool MsgQuestion(const QString &title, const QString &text);

private slots:
	void SlotBtnClicked();

protected:
	void resizeEvent(QResizeEvent *event);

private:
	QLabel			*m_lbText;
	DialogTitle		*m_dlgTitle;
	QPushButton		*m_btnYes;
	QPushButton		*m_btnNo;
	QWidget			*m_layoutWidget;
};
#endif // SETTINGSDLG_H

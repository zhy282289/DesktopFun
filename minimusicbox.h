#ifndef MINIMUSICBOX_H
#define MINIMUSICBOX_H

#include <QObject>

#include <phonon/VideoPlayer>
#include <phonon/SeekSlider>

class DialogTitle;
class MiniMusicBox : public QLabel
{
	Q_OBJECT
public:
	MiniMusicBox(QWidget *parent = NULL);

	void Show();

private slots:
	void SlotFinish();
	void SlotActTriggered();
	void SlotActionGroupTriggered(QAction *act);
	void SlotStateChanged(Phonon::State newState,Phonon::State oldState);
	void NextSong();
private:
	void SavePaths();
	void LoadSong();
protected:
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
private:
	Phonon::VideoPlayer *m_player;
	Phonon::SeekSlider *m_slider;
	DialogTitle		*m_dlgTitle;
	QActionGroup	*m_actionGroup;
	QStringList		m_songList;
	qint64			m_curFrames;
private:
	QAction		*m_actAddFiles;
	QAction		*m_actExit;
	QAction		*m_actRemoveFiles;
	QPushButton	*m_btnShowMinimmun;
	QPushButton	*m_btnStartStop;
};

MiniMusicBox* GetMiniMusicBox();
#endif // MINIMUSICBOX_H

#include "StdAfx.h"
#include "minimusicbox.h"

#include "settingsdlg.h"
#include <phonon/phonon>

//////////////////////////////////////////////////////////////////////////
MiniMusicBox::MiniMusicBox( QWidget *parent /*= NULL*/ )
	:QLabel(parent)
{
	setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_TranslucentBackground);
	setFixedSize(700, 300);
	m_dlgTitle = new DialogTitle(this);
	m_dlgTitle->setText(" 米你播放器");


	m_actAddFiles = new QAction("增加音频文件", this);
	m_actExit = new QAction("退出", this);
	m_actRemoveFiles = new QAction("移除文件", this);
	connect(m_actAddFiles, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actExit, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));
	connect(m_actRemoveFiles, SIGNAL(triggered()), this, SLOT(SlotActTriggered()));

	m_actionGroup = new QActionGroup(this);
	connect(m_actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(SlotActionGroupTriggered(QAction*)));

	m_btnShowMinimmun = new QPushButton("Hide", this);
	m_btnStartStop = new QPushButton("Start/Stop", this);
	m_btnStartStop->setStyleSheet("QPushButton{min-height:10px;}");
	m_btnShowMinimmun->setStyleSheet("QPushButton{min-height:10px;}");
	connect(m_btnShowMinimmun, SIGNAL(clicked()), this, SLOT(SlotActTriggered()));
	connect(m_btnStartStop, SIGNAL(clicked()), this, SLOT(SlotActTriggered()));

	m_player = new Phonon::VideoPlayer(Phonon::MusicCategory, this);
	connect(m_player, SIGNAL(finished()), this, SLOT(SlotFinish()));
	connect((QObject*)m_player->mediaObject(), SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(SlotStateChanged(Phonon::State,Phonon::State)));
	m_slider = new Phonon::SeekSlider(this);
	m_slider->setMediaObject(m_player->mediaObject());

	LoadSong();
	QTimer::singleShot(0, this, SLOT(NextSong()));
}


void MiniMusicBox::mousePressEvent( QMouseEvent *event )
{
	if (event->button() & Qt::RightButton)
	{
		QMenu menu(this);
		menu.addAction(m_actAddFiles);
		menu.addSeparator();
		menu.addActions(m_actionGroup->actions());
		menu.addSeparator();
		menu.addAction(m_actRemoveFiles);
		menu.addAction(m_actExit);
		QAction *acceptAct = menu.exec(event->globalPos());		
	}

}

void MiniMusicBox::paintEvent( QPaintEvent *event )
{

	QPainter painter(this);
	const QString text = QString("关于程序:\n"
		"最终解释权归: BOY 所有\n"
		"QQ:3969622**\n\n\n\n\n"
		"下面我为大家唱一首歌：\n\n你是我的小呀小苹果!!!!"
		);

	painter.fillRect(rect(), QBrush(QColor(255,210,255, 200)));
	painter.setPen(QPen(QColor(250,0,0,150)));
	QRect rect = this->rect();
	rect.setLeft(50);
	painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
}

void MiniMusicBox::SlotFinish()
{
	NextSong();
}

void MiniMusicBox::resizeEvent( QResizeEvent *event )
{
	m_dlgTitle->setGeometry(0, 0, rect().width(), 22);

	
	m_slider->setGeometry(10, rect().height()-22, 580, 22);

	int left = m_slider->geometry().right() + 10;
	m_btnStartStop->setGeometry(left, rect().height()-35, 72, 15);
	m_btnShowMinimmun->setGeometry(left, rect().height()-19, 72, 15);
}

void MiniMusicBox::SlotActTriggered()
{
	if (m_actAddFiles == sender())
	{
		AddFilesOrDirectoryDlg dlg(this);
		QStringList paths = dlg.AddFiles(QStringList() << "*.mp3");
		bool bNewPath = false;
		for (int i = 0; i < paths.size(); ++i)
		{
			if (!m_songList.contains(paths.at(i)))
			{
				QAction *act = new QAction(QFileInfo(paths.at(i)).completeBaseName(), this);
				act->setCheckable(true);
				act->setData(paths.at(i));
				m_actionGroup->addAction(act);
				bNewPath = true;
				m_songList.push_back(paths.at(i));
			}

		}
		if (bNewPath)
		{
			SavePaths();
		}

	}
	else if (m_actExit == sender())
	{
		hide();
	}
	else if (m_actRemoveFiles == sender())
	{

	}
	else if (m_btnShowMinimmun == sender())
	{
		showMinimized();
	}
	else if (m_btnStartStop == sender())
	{
		if (Phonon::PlayingState == m_player->mediaObject()->state())
		{
			m_player->stop();
			m_curFrames = m_player->mediaObject()->currentTime();
		}
		else
		{
			m_player->play();
			m_player->seek(m_curFrames);
		}
	}
}

void MiniMusicBox::closeEvent( QCloseEvent *event )
{
	QLabel::closeEvent(event);
}

void MiniMusicBox::NextSong()
{
	QAction *act = m_actionGroup->checkedAction();
	QList<QAction*> acts = m_actionGroup->actions();
	if (acts.isEmpty())
		return;

	int index = 0;
	if (act)
	{
		index = acts.indexOf(act);
		++index;
		index = index % acts.size();
	}

	acts.at(index)->setChecked(true);
	m_player->play(Phonon::MediaSource(acts.at(index)->data().toString()));
}

void MiniMusicBox::SlotActionGroupTriggered( QAction *act )
{
	if (act)
	{
		m_player->play(Phonon::MediaSource(act->data().toString()));
	}

}

void MiniMusicBox::LoadSong()
{
	QSettings settings(GetSongSettingPath()+"setting", QSettings::IniFormat);
	m_songList = settings.value("songlist").toStringList();

	QDir dir(GetSongSettingPath());
	QFileInfoList infoList = dir.entryInfoList(QStringList()<<"*.mp3",QDir::Files);
	for (int i = 0; i < infoList.size(); ++i)
	{
		QAction *act = new QAction(infoList.at(i).completeBaseName(), this);
		act->setCheckable(true);
		act->setData(infoList.at(i).filePath());
		m_actionGroup->addAction(act);
	}

	for (int i = m_songList.size()-1; i >= 0; --i)
	{
		if (!QFile::exists(m_songList.at(i)))
		{
			m_songList.removeOne(m_songList.at(i));
		}
	}
	for (int i = 0; i < m_songList.size(); ++i)
	{
		QAction *act = new QAction(QFileInfo(m_songList.at(i)).completeBaseName(), this);
		act->setCheckable(true);
		act->setData(m_songList.at(i));
		m_actionGroup->addAction(act);
	}
}

void MiniMusicBox::SlotStateChanged( Phonon::State newState,Phonon::State oldState )
{
	if (newState == Phonon::ErrorState)
	{
		NextSong();
	}
}

void MiniMusicBox::SavePaths()
{
	QSettings settings(GetSongSettingPath()+"setting", QSettings::IniFormat);
	settings.setValue("songlist", m_songList);
}

void MiniMusicBox::Show()
{
	//setFixedSize(700, 300);
	resize(700, 300);
	move(500, 500);
	show();
}


MiniMusicBox *g_miniMusicBox = NULL;

MiniMusicBox* GetMiniMusicBox()
{
	if (g_miniMusicBox == NULL)
	{
		g_miniMusicBox = new MiniMusicBox();
		::SetParent(g_miniMusicBox->winId(), FindDesktopWnd());
	}
	return g_miniMusicBox;
}

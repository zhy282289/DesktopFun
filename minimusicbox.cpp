#include "StdAfx.h"
#include "minimusicbox.h"

#include "settingsdlg.h"
#include <phonon/phonon>


//////////////////////////////////////////////////////////////////////////
void LoadSong(AudioList &audioList)
{
	QSettings settings(GetSongSettingPath()+"setting", QSettings::IniFormat);
	QStringList songList= settings.value("songlist").toStringList();
	audioList.FromStringList(songList);

}
void SavePaths(const AudioList &audioList)
{
	QStringList songList = audioList.ToStringList();
	songList.removeAt(0);
	QSettings settings(GetSongSettingPath()+"setting", QSettings::IniFormat);
	settings.setValue("songlist", songList);
}

//////////////////////////////////////////////////////////////////////////
MiniMusicBox::MiniMusicBox( QWidget *parent /*= NULL*/ )
	:QLabel(parent)
{
	setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint);
	//setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_TranslucentBackground);
	setFixedSize(700, 300);
	m_dlgTitle = new DialogTitle(this);
	m_dlgTitle->setText(" 米你播放器");

	m_audioList.AddFile(":/DesktopFun/小苹果(必有歌曲).mp3");
	m_view = new AudioPlayListView(this);
	connect(m_view, SIGNAL(SigPlay(QString)), this, SLOT(SlotPlay(QString)));
	connect(m_view, SIGNAL(SigAddFiles()), this, SLOT(SlotAddFiles()));
	connect(m_view, SIGNAL(SigRemoveFiles(QStringList)), this, SLOT(SlotRemoveFiles(QStringList)));
	m_model = new AudioPlayListMode(&m_audioList, this);
	m_view->setModel(m_model);
	m_delegate = new AudioPlayListDelegate(this);
	m_view->setItemDelegateForColumn(0, m_delegate);


	m_btnShowMinimmun = new QPushButton("隐藏", this);
	m_btnStartStop = new QPushButton("开始/暂停", this);
	m_btnStartStop->setStyleSheet("QPushButton{min-height:10px;}");
	m_btnShowMinimmun->setStyleSheet("QPushButton{min-height:10px;}");
	connect(m_btnShowMinimmun, SIGNAL(clicked()), this, SLOT(SlotActTriggered()));
	connect(m_btnStartStop, SIGNAL(clicked()), this, SLOT(SlotActTriggered()));

	m_player = new Phonon::VideoPlayer(Phonon::MusicCategory, this);
	connect(m_player, SIGNAL(finished()), this, SLOT(SlotFinish()));
	connect((QObject*)m_player->mediaObject(), SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(SlotStateChanged(Phonon::State,Phonon::State)));
	m_slider = new Phonon::SeekSlider(this);
	m_slider->setMediaObject(m_player->mediaObject());

	LoadSong(m_audioList);
	m_view->reset();

	QTimer::singleShot(0, this, SLOT(NextSong()));
}


void MiniMusicBox::mousePressEvent( QMouseEvent *event )
{


}

void MiniMusicBox::paintEvent( QPaintEvent *event )
{

	QPainter painter(this);
	painter.fillRect(rect(), QBrush(QColor(255,210,255)));;
}

void MiniMusicBox::SlotFinish()
{
	NextSong();
}

void MiniMusicBox::resizeEvent( QResizeEvent *event )
{
	m_dlgTitle->setGeometry(0, 0, rect().width(), 22);
	const int margins = 10;
	int top = m_dlgTitle->geometry().bottom()+4;
	m_view->setGeometry(margins, top, rect().width()-2*margins, rect().height()- 80);
	m_slider->setGeometry(10, rect().height()-22, 580, 22);

	int left = m_slider->geometry().right() + 10;
	m_btnStartStop->setGeometry(left, rect().height()-45, 72, 18);
	m_btnShowMinimmun->setGeometry(left, rect().height()-26, 72, 18);
}

void MiniMusicBox::SlotActTriggered()
{
	if (m_btnShowMinimmun == sender())
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
	QString path = m_model->NextFile();
	if (!path.isEmpty())
	{
		m_player->play(Phonon::MediaSource(path));
	}
	else
	{
		//m_player->play(Phonon::MediaSource(QUrl("http://sc.111ttt.com/up/mp3/403976/23D19644B90448BD07848D721F77E4C9.mp3")));
		m_player->play(Phonon::MediaSource(m_audioList.at(0).path));
		m_audioList.SetIndexRunning(0, true);
	}
	
}





void MiniMusicBox::SlotStateChanged( Phonon::State newState,Phonon::State oldState )
{
	if (newState == Phonon::ErrorState)
	{
		NextSong();
	}
}



void MiniMusicBox::Show()
{
	//setFixedSize(700, 300);
	resize(700, 300);
	//move(500, 500);
	show();
}

void MiniMusicBox::SlotPlay( QString path )
{
	m_player->play(Phonon::MediaSource(path));
}


MiniMusicBox *g_miniMusicBox = NULL;

MiniMusicBox* GetMiniMusicBox()
{
	if (g_miniMusicBox == NULL)
	{
		g_miniMusicBox = new MiniMusicBox();
		//::SetParent(g_miniMusicBox->winId(), FindDesktopWnd());
	}
	return g_miniMusicBox;
}

AudioPlayListMode::AudioPlayListMode( AudioList *songList, QObject *parent )
	:QAbstractTableModel(parent)
	,m_audioList(songList)
{

}

Qt::ItemFlags AudioPlayListMode::flags( const QModelIndex & index ) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int AudioPlayListMode::columnCount( const QModelIndex & parent /*= QModelIndex() */ ) const
{
	return 1;
}

int AudioPlayListMode::rowCount( const QModelIndex & parent /*= QModelIndex() */ ) const
{
	return m_audioList->size();
}

QVariant AudioPlayListMode::data( const QModelIndex & index, int role /*= Qt::DisplayRole */ ) const
{
	int row = index.row();
	int column = index.column();
	if (!index.isValid() || row >= m_audioList->size())
	{
		return QVariant();
	}

	if (role == Qt::DisplayRole)
	{
		
		if (row == 0)
		{
			return QFileInfo(m_audioList->at(row).path).completeBaseName();
		}
		else
		{
			return m_audioList->at(row).path;
		}
		
	}
	else if (role == Qt::EditRole)
	{
		return m_audioList->at(row).path;
	}
	else if (role == AudioRunning)
	{
		return m_audioList->at(row).running;
	}
	return QVariant();
}

bool AudioPlayListMode::setData( const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole */ )
{
	int row = index.row();
	int column = index.column();
	if (!index.isValid() || row >= m_audioList->size())
	{
		return false;
	}

	if (role == Qt::DisplayRole)
	{
		
	}
	else if (role == Qt::EditRole)
	{
		
	}
	else if (role == AudioRunning)
	{
		m_audioList->SetIndexRunning(row, value.toBool());
	}
	emit dataChanged(this->index(0,0), this->index(m_audioList->size()-1, 0));
	return true;
}

void AudioPlayListMode::AddFiles( QStringList paths )
{
	bool bNewPath = false;
	for (int i = 0; i < paths.size(); ++i)
	{
		if (!m_audioList->Contains(paths.at(i)))
		{
			m_audioList->AddFile(paths.at(i));
			bNewPath = true;
		}
	}
	if (bNewPath)
	{
		SavePaths(*m_audioList);
		reset();
	}
}

void AudioPlayListMode::RemoveFiles( QStringList paths )
{
	m_audioList->RemoveFiles(paths);
	SavePaths(*m_audioList);
	reset();
}

QString AudioPlayListMode::NextFile()
{
	return m_audioList->NextFile();
}

AudioPlayListView::AudioPlayListView( QWidget *parent /*= NULL*/ )
	:QTableView(parent)
{
	verticalHeader()->hide();
	horizontalHeader()->setStretchLastSection(true);
	horizontalHeader()->hide();
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(SlotDoubleClicked(const QModelIndex&)));

}

void AudioPlayListView::SlotDoubleClicked( const QModelIndex &index )
{
	model()->setData(index, true, AudioPlayListMode::AudioRunning);
	emit SigPlay(model()->data(index, Qt::EditRole).toString());
}

void AudioPlayListView::mousePressEvent( QMouseEvent *event )
{
	QTableView::mousePressEvent(event);
}

void AudioPlayListView::mouseReleaseEvent( QMouseEvent *event )
{
	if (event->button() == Qt::RightButton)
	{
		QMenu menu(this);

		QAction *addFiles = menu.addAction("增加文件");
		QAction *removeFiles = menu.addAction("移除文件");

		QAction *acceptAct = menu.exec(event->globalPos());

		if (acceptAct == addFiles)
		{
			emit SigAddFiles();
		}
		else if (acceptAct == removeFiles)
		{
			QStringList audioPaths;
			QModelIndexList indexList = selectionModel()->selectedIndexes();
			for (int i = 0; i < indexList.size(); ++i)
			{
				audioPaths << indexList.at(i).data(Qt::EditRole).toString();
			}
			emit SigRemoveFiles(audioPaths);
		}
	}
}

void MiniMusicBox::SlotAddFiles(  )
{
	AddFilesOrDirectoryDlg dlg(this);
	QStringList paths = dlg.AddFiles(QStringList() << "*.mp3");
	m_model->AddFiles(paths);
}

void MiniMusicBox::SlotRemoveFiles( QStringList paths )
{
	m_model->RemoveFiles(paths);
}


AudioPlayListDelegate::AudioPlayListDelegate( QObject *parent /*= NULL*/ )
	:QItemDelegate(parent)
{
	m_runningColor = QColor(255,255,198);
}


void AudioPlayListDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{

	bool running = index.data(AudioPlayListMode::AudioRunning).toBool();
	if (running)
	{
		painter->fillRect(option.rect, QBrush(m_runningColor));
	}

	if (option.state & QStyle::State_Selected)
	{
		painter->fillRect(option.rect, option.palette.highlight());
	}

	QString path = index.data().toString();
	QRect textRect = option.rect.adjusted(10, 0, 0, 0);
	painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,  path);
}



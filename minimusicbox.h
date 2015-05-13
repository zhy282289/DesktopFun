#ifndef MINIMUSICBOX_H
#define MINIMUSICBOX_H

#include <QObject>

#include <phonon/VideoPlayer>
#include <phonon/SeekSlider>


struct AudioData
{
	bool running;
	QString path;
	AudioData()
	{
		running = false;
	}
};

class AudioList :public QList<AudioData>
{
public:
	bool Contains(const QString &path)
	{
		for (int i = 0; i < size(); ++i)
		{
			if (this->at(i).path.compare(path) == 0)
			{
				return true;
			}
		}
		return false;
	}
	void FromStringList(QStringList songList)
	{
		for (int i = 0; i < songList.size(); ++i)
		{
			AudioData audioData;
			audioData.path= songList.at(i);
			push_back(audioData);
		}
	}
	QStringList ToStringList() const
	{
		QStringList songList;
		for (int i = 0; i < size(); ++i)
		{
			songList << this->at(i).path;
		}
		return songList;
	}

	void SetIndexRunning(int index, bool running)
	{
		for (int i = 0; i < size(); ++i)
		{
			(*this)[i].running  = false;
		}
		(*this)[index].running  = running;
	}
	void AddFile(QString path)
	{
		AudioData audioData;
		audioData.path = path;
		push_back(audioData);
	}
	void RemoveFiles(QStringList paths)
	{
		for (int i = 0; i < paths.size(); ++i)
		{
			RemoveFile(paths.at(i));
		}
	}
	void RemoveFile(const QString &path)
	{
		for (int i = 1; i < size(); ++i)
		{
			if (path.compare(this->at(i).path) == 0)
			{
				removeAt(i);
			}
		}
	}
	QString NextFile()
	{
		int index =0;
		for (;index < size(); ++index)
		{
			if (at(index).running)
			{
				if (++index >= size())
				{
					index = 0;
				}

				SetIndexRunning(index, true);
				return at(index).path;
			}
		}
		return QString("");
	}
};


class DialogTitle;
class AudioPlayListMode;
class AudioPlayListDelegate;
class MiniMusicBox : public QLabel
{
	Q_OBJECT
public:
	MiniMusicBox(QWidget *parent = NULL);

	void Show();

private slots:
	void SlotFinish();
	void SlotActTriggered();
	void SlotStateChanged(Phonon::State newState,Phonon::State oldState);
	void NextSong();
	void SlotPlay(QString path);
	void SlotAddFiles();
	void SlotRemoveFiles(QStringList paths);

protected:
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
private:
	Phonon::VideoPlayer *m_player;
	Phonon::SeekSlider *m_slider;
	DialogTitle		*m_dlgTitle;
	qint64			m_curFrames;
private:
	QPushButton	*m_btnShowMinimmun;
	QPushButton	*m_btnStartStop;

private:
	QTableView	*m_view;
	AudioPlayListMode	*m_model;
	AudioPlayListDelegate	*m_delegate;
	AudioList	m_audioList;
};

class AudioPlayListView : public QTableView
{
	Q_OBJECT
public:
	enum{MenuAddFiles, MenuRemoveFiles};
public:
	AudioPlayListView(QWidget *parent = NULL);

signals:
	void SigPlay(QString);
	void SigAddFiles();
	void SigRemoveFiles(QStringList);
private slots:
	void SlotDoubleClicked(const QModelIndex &index);
	

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

};
class AudioPlayListMode : public QAbstractTableModel
{
	Q_OBJECT
public:
	enum {AudioRunning = Qt::UserRole + 1, };
public:
	explicit AudioPlayListMode(AudioList *songList, QObject *parent);

	void AddFiles(QStringList paths);
	void RemoveFiles(QStringList paths);
	QString NextFile();

	virtual Qt::ItemFlags	flags ( const QModelIndex & index ) const;
	virtual int	columnCount ( const QModelIndex & parent = QModelIndex() ) const;
	virtual int	rowCount ( const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant	data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual bool	setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

private:
	AudioList *m_audioList;
};


class AudioPlayListDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	AudioPlayListDelegate(QObject *parent = NULL);

	virtual void	paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

private:
	QColor m_runningColor;
};

MiniMusicBox* GetMiniMusicBox();
#endif // MINIMUSICBOX_H

#ifndef THREAD_H
#define THREAD_H

#include <QThread>
#include <thread>
#include <chrono>

class MainWindow;

class Thread : public QThread
{
public:
	Thread(QObject* parent) : QThread(parent) {}
	virtual bool succeeded() const = 0;
	virtual const QString& message() const = 0;
	virtual ~Thread() {}
};


template<typename FunctionType>
class TestThread : public Thread
{
public:
	typedef FunctionType ThreadFunctionType;

	TestThread(MainWindow* mainWindow, FunctionType function,
			   QObject *parent = nullptr)
		: Thread(parent), function(function), mainWindow(mainWindow), success(false) {}

	virtual bool succeeded() const {return success;}
	virtual const QString& message() const {return _message;}
	~TestThread() override{}
protected:
	ThreadFunctionType function;
	MainWindow* mainWindow;
	QString _message;
	bool success;
};

#endif // THREAD_H

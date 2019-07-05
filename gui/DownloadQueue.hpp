#ifndef DOWNLOADQUEUE_H
#define DOWNLOADQUEUE_H

#include <curl/curl.h>
#include <curl/easy.h>

#include <functional>
#include <string>
#include <list>

struct DownloadOperation;

enum class DownloadStatus
{
	QUEUED,
	DOWNLOADING,
	COMPLETE,
	FAILED,
};

struct DownloadOperation
{
	std::string url;
	std::string buffer;
	DownloadStatus status;
	CURL *eh;

	std::function<void(DownloadOperation*)> cb;
	void *cbdata;
};

class DownloadQueue
{
public:
	DownloadQueue();
	~DownloadQueue();

	// add a new download operation
	void downloadAdd(DownloadOperation *download);

	// cancel a download operation
	void downloadCancel(DownloadOperation *download);

	// process finished and queued downloads
	int process();

	// static instance
	static void init();
	static void quit();
	static DownloadQueue* downloadQueue;

private:
	// start a transfer operation
	void transferStart(DownloadOperation *download);

	// finish a transfer operation
	void transferFinish(DownloadOperation *download);

	// start new transfers from the queue
	void startTransfersFromQueue();

	// curl multi handle
	CURLM *cm;

	// queue of downloads
	std::list<DownloadOperation*> queue;

	// number of active transfers
	int transfers = 0;
};

#endif

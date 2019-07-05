#include "DownloadQueue.hpp"

#define MAX_PARALLEL_DOWNLOADS	4

DownloadQueue* DownloadQueue::downloadQueue = NULL;

void DownloadQueue::init()
{
	downloadQueue = new DownloadQueue();
}

void DownloadQueue::quit()
{
	delete downloadQueue;
}

static size_t WriteCallback(char *data, size_t n, size_t l, void *userp)
{
	DownloadOperation *download = (DownloadOperation *)userp;
	download->buffer.append(data, n * l);
	return n * l;
}

DownloadQueue::DownloadQueue()
{
	cm = curl_multi_init();
	curl_multi_setopt(cm, CURLMOPT_MAXCONNECTS, MAX_PARALLEL_DOWNLOADS);
}

DownloadQueue::~DownloadQueue()
{
	curl_multi_cleanup(cm);
}

// add a new download operation
void DownloadQueue::downloadAdd(DownloadOperation *download)
{
	download->status = DownloadStatus::QUEUED;
	queue.push_back(download);
}

// cancel a download operation
void DownloadQueue::downloadCancel(DownloadOperation *download)
{
	if (download->status == DownloadStatus::DOWNLOADING)
		transferFinish(download);
	else if (download->status == DownloadStatus::QUEUED)
		queue.remove(download);
}

// start a transfer operation
void DownloadQueue::transferStart(DownloadOperation *download)
{
	download->eh = curl_easy_init();

#if defined(__WIIU__)
	// network optimizations
	curl_easy_setopt(download->eh, (CURLoption)213, 1);
	curl_easy_setopt(download->eh, (CURLoption)212, 0x8000);
#endif

	curl_easy_setopt(download->eh, CURLOPT_URL, download->url.c_str());
	curl_easy_setopt(download->eh, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(download->eh, CURLOPT_WRITEDATA, download);
	curl_easy_setopt(download->eh, CURLOPT_PRIVATE, download);

	curl_multi_add_handle(cm, download->eh);

	transfers++;
}

// finish a transfer operation
void DownloadQueue::transferFinish(DownloadOperation *download)
{
	curl_multi_remove_handle(cm, download->eh);
	curl_easy_cleanup(download->eh);

	transfers--;
}

// start new transfers from the queue
void DownloadQueue::startTransfersFromQueue()
{
	while ((transfers < MAX_PARALLEL_DOWNLOADS) && (queue.size() > 0))
	{
		// remove the first element from the download queue
		DownloadOperation *download = queue.front();
		queue.pop_front();

		// start a new download
		download->status = DownloadStatus::DOWNLOADING;
		transferStart(download);
	}
}

// process finished and queued downloads
int DownloadQueue::process()
{
	DownloadOperation *download;
	int still_alive = 1;
	int msgs_left = -1;
	CURLMsg *msg;

	curl_multi_perform(cm, &still_alive);

	// handle completed or failed downloads
	while(msg = curl_multi_info_read(cm, &msgs_left))
	{
		long response_code = 404;

		if (msg->msg != CURLMSG_DONE)
			continue;

		curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &download);
		curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &response_code);

		transferFinish(download);
		startTransfersFromQueue();

		if (response_code == 200)
			download->status = DownloadStatus::COMPLETE;
		else
			download->status = DownloadStatus::FAILED;

		download->cb(download);
	}

	startTransfersFromQueue();

	return ((still_alive) || (msgs_left > 0) || (queue.size() > 0));
}

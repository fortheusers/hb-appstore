#include "ProgressScreen.hpp"
#include "../libs/chesto/src/Constraint.hpp"

using namespace Chesto;

ProgressScreen::ProgressScreen(std::shared_ptr<Package> pkg, std::string op, double partialDownloadPercent)
	: package(pkg), operation(std::move(op)), partialOffset(partialDownloadPercent / 100.0)
{
	rebuildUI();
}

void ProgressScreen::rebuildUI()
{
	removeAll();

	// add semi-transparent overlay background
	auto overlay = createNode<Element>();
	overlay->width = SCREEN_WIDTH;
	overlay->height = SCREEN_HEIGHT;
	overlay->backgroundColor = fromRGB(0, 0, 0);
	overlay->backgroundOpacity = 0x90;
	overlay->cornerRadius = 1; // forces transparency
	overlay->hasBackground = true;
	overlay->isAbsolute = true;

	// Title, operation, package name
	std::string titleText = operation + " " + package->getTitle();
	auto titleElement = createNode<TextElement>(titleText.c_str(), 30);
	titleElement->constrain(ALIGN_CENTER_HORIZONTAL, 0);
	titleElement->position(0, SCREEN_HEIGHT / 2 - 100);

	auto progressContainer = createNode<Element>();
	progressContainer->width = 600;
	progressContainer->height = 30;
	progressContainer->constrain(ALIGN_CENTER_BOTH);
	
	// Progress bar
	progressBar = progressContainer->createNode<ProgressBar>();
	progressBar->width = 600;
	progressBar->height = 30;
	progressBar->percent = progress;

	// Status text
	statusLabel = createNode<TextElement>(statusText.c_str(), 20);
	statusLabel->constrain(ALIGN_CENTER_HORIZONTAL, 0);
	statusLabel->position(0, SCREEN_HEIGHT / 2 + 50);
	
	// Cancel button at the bottom
	auto cancelBtn = createNode<Button>("Cancel", B_BUTTON, false, 18);
	cancelButton = cancelBtn;
	cancelButton->width = 200;
	cancelButton->height = 50;
	cancelButton->constrain(ALIGN_CENTER_HORIZONTAL, 0);
	cancelButton->position(0, SCREEN_HEIGHT / 2 + 120);
	cancelButton->action = [this]() {
		if (cancelCallback) {
			cancelCallback();
		}
		// only pops ProgressScreen, not AppDetails
		RootDisplay::popScreen();
	};
}

void ProgressScreen::hideCancelButton()
{
	if (cancelButton) {
		cancelButton->hidden = true;
	}
}

void ProgressScreen::updateProgress(double percent, std::string status)
{
	// augment progress with partial download offset
	// if we resumed at 45%, and new download reports 50% progress,
	// we want to show: 45% + (50% * 55%) = 72.5%
	double augmentedPercent = partialOffset + (percent * (1.0 - partialOffset));

	progress = augmentedPercent;
	
	if (!status.empty()) {
		statusText = status;
	}
	
	if (progressBar) {
		progressBar->percent = progress;
	}
	if (statusLabel && !statusText.empty()) {
		statusLabel->setText(statusText);
		statusLabel->update();
	}
}

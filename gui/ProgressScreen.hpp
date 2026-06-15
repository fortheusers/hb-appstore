#pragma once

#include "../libs/chesto/src/RootDisplay.hpp"
#include "../libs/chesto/src/Screen.hpp"
#include "../libs/chesto/src/ProgressBar.hpp"
#include "../libs/chesto/src/TextElement.hpp"
#include "../libs/chesto/src/Button.hpp"
#include "../libs/get/src/Package.hpp"
#include <string>
#include <memory>
#include <functional>

using namespace Chesto;

class ProgressScreen : public Screen
{
public:
	ProgressScreen(std::shared_ptr<Package> pkg, std::string operation, double partialDownloadPercent = 0.0);
	void rebuildUI() override;
	
	void updateProgress(double percent, std::string status = "");
	
	double getProgress() const { return progress; }
	
	void setCancelCallback(std::function<void()> callback) { cancelCallback = callback; }
	
	void hideCancelButton();

private:
	std::shared_ptr<Package> package;
	std::string operation; // "Installing" or "Removing"
	std::string statusText;
	double progress = 0.0;
	double partialOffset = 0.0; // Portion already completed from partial download
	
	ProgressBar* progressBar = nullptr;
	TextElement* statusLabel = nullptr;
	Button* cancelButton = nullptr;
	
	std::function<void()> cancelCallback;
};

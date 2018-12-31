all:
	@echo "      __           __   __   __  ___  __   __   ___"
	@echo "|__| |__)     /\  |__) |__) /__\`  |  /  \ |__) |__"
	@echo "|  | |__)    /~~\ |    |    .__/  |  \__/ |  \ |___"
	@echo
	@echo "Hi! See the readme for instructions on how to make this program for different supported platforms."
	@echo "In short, you can add \"-f Makefile.switch\" or \"-f Makefile.wiiu\" to build apps for either of those targets."
	@echo
	@echo "Defaulting to the Switch build:"
	make -f Makefile.switch

clean:
	make -f Makefile.switch clean

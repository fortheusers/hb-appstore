all:
	@echo -e "      __           __   __   __  ___  __   __   ___\r\n|__| |__)     /\  |__) |__) /__\`  |  /  \ |__) |__\r\n|  | |__)    /~~\ |    |    .__/  |  \__/ |  \ |___"
	@echo -e "\r\nHi! See the readme for instructions on how to make this program for different supported platforms."
	@echo "In short, you can add \"-f Makefile.switch\" or \"-f Makefile.wiiu\" to build apps for either of those targets."
	@echo -e "\r\nDefaulting to the Switch build:"
	make -f Makefile.switch

clean:
	make -f Makefile.switch clean

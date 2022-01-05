echo "HOMEBREW APP STORE"
echo
echo "Hi! See the readme for instructions on how to make this program for different supported platforms."
echo "In short, you can add \"-f Makefile.switch\" or \"-f Makefile.wiiu\" to build apps for either of those targets."
echo
echo "If you need dependencies, check out spheal's dependency_helper.sh for assistance"
echo "-> https://gitlab.com/4TU/spheal/-/blob/master/dependency_helper.sh"
echo
echo "If you have docker installed, you can build in spheal's container directly:"
echo "-> docker run -v \$(pwd):/code -it registry.gitlab.com/4tu/spheal \"make \$PLATFORM\""
echo
echo "Running make"
make


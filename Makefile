BUILD_DIR = _build
SOURCE_DIR = .
TESTS_DIR = tests

.IGNORE: clean distclean maintainer-clean
.PHONY: all install virtualenv tests

SOURCES := $(shell find $(SOURCE_DIR) -name '*.c')
TEST_REQUESTS := $(shell find $(TESTS_DIR) -name '*.http')


help:
	@echo "Please use 'make <target>' where <target> is one of"
	@echo "  install                     install dependencies and prepare environment"
	@echo "  install-dev                 install dependencies and everything needed to run tests"
	@echo "  build-requirements          install all requirements and freeze them in requirements.txt"
	@echo "  serve                       start the API server on default port"
	@echo "  tests-once                  only run the tests once with the default python interpreter"
	@echo "  flake8                      run the flake8 linter"
	@echo "  tests                       run all the tests with all the supported python interpreters (same as travis)"
	@echo "  clean                       remove *.pyc files and __pycache__ directory"
	@echo "  distclean                   remove *.egg-info files and *.egg, build and dist directories"
	@echo "  maintainer-clean            remove the .tox and the .venv directories"
	@echo "  docs                        build the docs"
	@echo "Check the Makefile to know exactly what each target is doing."



build:
	mkdir -p $(BUILD_DIR)
	bison -d -o core/parser/http_parser.tab.c core/parser/http_parser.y
	flex -o core/parser/http_parser.yy.c core/parser/http_parser.l
	gcc -o $(BUILD_DIR)/server \
		core/utils/queue.c\
		core/utils/utils.c\
	   	core/parser/http_parser.tab.c\
	   	core/parser/http_parser.yy.c\
	   	core/resource/resource.c\
	   	core/server.c -lfl -ly

tests: build
	for req in $(TEST_REQUESTS) ; do \
		./$(BUILD_DIR)/server html $$req out.txt log.txt; \
	done


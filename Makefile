BUILD_DIR = _build
SOURCE_DIR = .
TESTS_DIR = tests

.PHONY: build

SOURCES := $(shell find $(SOURCE_DIR) -name '*.c')
TEST_REQUESTS := $(shell find $(TESTS_DIR) -name '*.http')


help:
	@echo "Please use 'make <target>'"
	@echo "___________________________________________________________________"
	@echo "| TARGET      |                  DESCRIPTION                      |"
	@echo "|-----------------------------------------------------------------|"
	@echo "| build       |                  build binaries                   |"
	@echo "| tests       |                  run tests                        |"
	@echo "| tests-once  |                  run sample test                  |"
	@echo "|_____________|___________________________________________________|"

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
	   	core/server.c -lfl -ly -lpthread

serve: build
	touch log.txt
	./$(BUILD_DIR)/server 8000 ./html ./log.txt 8

tests-env: build
	virtualenv venv
	venv/bin/pip install -r test-requirements.txt
	mkdir -p tests/test_webspace
	touch tests/log.txt
	rm tests/log.txt
	./$(BUILD_DIR)/server 8000 tests/test_webspace tests/log.txt 8

tests-all:
	py.test -v tests

tests-visual:
	firefox http://localhost:8000

tests-once: build
	./$(BUILD_DIR)/server tests/test-html tests/requests/get/index.http tests/out.txt tests/log.txt;

BUILD_DIR = _build
SOURCE_DIR = .
TESTS_DIR = tests

.PHONY: build tests

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
	mkdir -p $(TESTS_DIR)/test_webspace
	touch $(TESTS_DIR)/log.txt
	rm $(TESTS_DIR)/log.txt
	./$(BUILD_DIR)/server 8000 tests/test_webspace tests/log.txt 8 &

tests-run:
	venv/bin/pytest -v tests ; pkill "server"

tests: tests-env tests-run

tests-visual:
	firefox http://localhost:8000

clean:
	rm -r venv
	rm -r _build
	rm */__pycache__

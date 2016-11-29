BUILD_DIR = _build
SOURCE_DIR = .
TESTS_DIR = tests

.IGNORE: tests test-once
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
	   	core/server.c -lfl -ly

serve: build
	./$(BUILD_DIR)/server 8000 html tests/log.txt;

tests: build
	touch tests/log.txt
	rm tests/log.txt
	for req in $(TEST_REQUESTS) ; do \
		./$(BUILD_DIR)/server tests/test-html $$req $$req.out.txt tests/log.txt; \
	done

tests-once: build
	./$(BUILD_DIR)/server tests/test-html tests/requests/get/index.http tests/out.txt tests/log.txt;

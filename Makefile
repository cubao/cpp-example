PROJECT_SOURCE_DIR ?= $(abspath ./)
PROJECT_NAME ?= $(basename $(PROJECT_SOURCE_DIR))
BUILD_DIR ?= $(PROJECT_SOURCE_DIR)/build
INSTALL_DIR ?= $(BUILD_DIR)/install
NUM_JOB ?= 8
BUILD_TYPE ?= Release

all:
	@echo nothing special

clean:
	rm -rf $(BUILD_DIR)
force_clean:
	docker run --rm -v `pwd`:`pwd` -w `pwd` -it alpine/make make clean
.PHONY: clean force_clean

lint:
	pre-commit run -a
lint_install:
	pre-commit install

CMAKE_ARGS ?= \
	-DCMAKE_INSTALL_PREFIX=$(INSTALL_DIR) \
	-DBUILD_SHARED_LIBS=OFF \
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
build:
	mkdir -p $(BUILD_DIR) && cd $(BUILD_DIR) && \
	cmake $(PROJECT_SOURCE_DIR) $(CMAKE_ARGS) && \
	make -j $(NUM_JOB) && make install
.PHONY: build

data_pull:
	make -C data pull
data_clean:
	make -C data clean
.PHONY: data_pull data_clean

RED := '\033[31;7m'
NC := '\033[0m'
test: data_pull
	@cd $(BUILD_DIR) && for t in $(wildcard $(BUILD_DIR)/bin/test_*); do echo $$t && eval $$t >/dev/null 2>&1 && echo 'ok' || echo $(RED)Not Ok$(NC); done
.PHONY: test

tar.gz:
	cd .. && \
	tar -cvz --exclude .git -f $(PROJECT_NAME).tar.gz -C $(PROJECT_NAME) `ls $(PROJECT_NAME)` && \
	ls -alh $(PROJECT_NAME).tar.gz
.PHONY: tar.gz

# https://stackoverflow.com/a/25817631
echo-%  : ; @echo -n $($*)
Echo-%  : ; @echo $($*)
ECHO-%  : ; @echo $* = $($*)
echo-Tab: ; @echo -n '    '

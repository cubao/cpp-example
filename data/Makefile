# this file keeps track of testing data used by this project.
# all files should be stored somewhere with overwrite protection.
# so these download links 100% guarantee exatly same files across time.

all:
	@echo use "make pull" to pull data
	@echo use "make clean" to clean data

.PHONY: all pull clean
clean:
	@find . -maxdepth 1 ! -name Makefile ! -name . ! -name .. -exec rm -rf {} +

pull: schema
schema:
	wget https://json-schema.org/draft/2020-12/schema

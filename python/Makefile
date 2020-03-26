.PHONY: all
all: build

.PHONY: build
build: ../common/mcs.h setup.py lwa_mcs/*.py Makefile
	python setup.py build $(PYBUILDFLAGS)

.PHONY: install
install: build
	python setup.py install $(PYINSTALLFLAGS)

.PHONY: clean
clean:
	python setup.py clean --all
	rm -rf lwa_mcs/config.py

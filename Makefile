# assumes the following directories already exist
# common  (MCS-wide header files; specifically mcs.h)
# ../sch  (executables copied to here - Scheduler)
# ../exec (executables copied to here - Executive)
# ../tp   (executables copied to here - Task Processor)

all: sch exec tp python

.PHONY: sch
sch:
	$(MAKE) -C sch

.PHONY: exec
exec:
	$(MAKE) -C sofa/src
	$(MAKE) -C exec

.PHONY: tp
tp:
	$(MAKE) -C tp

.PHONY: python
python:
	$(MAKE) -C python

.PHONY: install
install:
	$(MAKE) -C sch install
	$(MAKE) -C exec install
	$(MAKE) -C tp install
	$(MAKE) -C python install

.PHONY: install-services
install-services:
	$(MAKE) -C services install

.PHONY: clean
clean:
	$(MAKE) -C sofa/src clean
	$(MAKE) -C sch clean
	$(MAKE) -C exec clean
	$(MAKE) -C tp clean
	$(MAKE) -C python clean

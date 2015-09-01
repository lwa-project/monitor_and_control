# assumes the following directories already exist
# common  (MCS-wide header files; specifically mcs.h)
# ../sch  (executables copied to here - Scheduler)
# ../exec (executables copied to here - Executive)
# ../tp   (executables copied to here - Task Processor)

all: sch exec tp

.PHONY: sch
sch:
	$(MAKE) -C sch

.PHONY: exec
exec:
	$(MAKE) -C exec

.PHONY: tp
tp:
	$(MAKE) -C tp

.PHONY: install
install:
	$(MAKE) -C sch install
	$(MAKE) -C exec install
	$(MAKE) -C tp install

.PHONY: clean
clean:
	$(MAKE) -C sch clean
	$(MAKE) -C exec clean
	$(MAKE) -C tp clean
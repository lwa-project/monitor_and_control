.PHONY: install
install:
	install mcs-sch.service /etc/systemd/system/
	install mcs-exec.service /etc/systemd/system/
	systemctl daemon-reload
	systemctl enable mcs-sch mcs-exec

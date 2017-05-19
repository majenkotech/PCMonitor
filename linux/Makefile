BIN=fan
OBJ=fan.o

$(BIN): $(OBJ)
	$(CXX) -o fan $< -lhidapi-libusb

clean:
	rm $(BIN) $(OBJ)

install: fan
	install fan.service /lib/systemd/system/
	install -m 755 fan /usr/sbin/
	@echo ""
	@echo "Installed. Enable service with:"
	@echo "$ sudo systemctl daemon-reload"
	@echo "$ sudo systemctl enable fan"
	@echo "Start service with:"
	@echo "$ sudo systemctl start fan"

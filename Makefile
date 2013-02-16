all:
	make -C firmware
	make -C hostapp

clean:
	make -C firmware clean
	make -C hostapp clean

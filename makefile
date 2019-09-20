# file		makefile
# date		2008/11/12
# author	kkamagui 
# brief		OS �̹����� �����ϱ� ���� make ����

# �⺻������ ���带 ������ ���
all: BootLoader Kernel32 Disk.img

# ��Ʈ �δ� ���带 ���� ��Ʈ �δ� ���͸����� make ����
BootLoader:
	@echo 
	@echo ============== Build Boot Loader ===============
	@echo 
	
	make -C 00.BootLoader
	
	@echo 
	@echo =============== Build Complete ===============
	@echo 
	
# ���� OS �̹��� ���带 ���� ��ȣ ��� Ŀ�� ���͸����� make ����
Kernel32:
	@echo 
	@echo ============== Build 32bit Kernel ===============
	@echo 
	
	make -C 01.Kernel32
	
	@echo 
	@echo =============== Build Complete ===============
	@echo 

	
# OS �̹��� ����
Disk.img: BootLoader Kernel32
	@echo 
	@echo =========== Disk Image Build Start ===========
	@echo 
	
	cat 00.BootLoader/BootLoader.bin 00.BootLoader/BootLoader2.bin 01.Kernel32/VirtualOS.bin  > Disk.img
	
	@echo 
	@echo ============= All Build Complete =============
	@echo 

# Run
run:
	
	qemu-system-x86_64 -L . -fda Disk.img -m 64 -rtc base=localtime -M pc
	
# �ҽ� ������ ������ ������ ���� ����	
clean:
	make -C 00.BootLoader clean
	make -C 01.Kernel32 clean
	rm -f Disk.img
	
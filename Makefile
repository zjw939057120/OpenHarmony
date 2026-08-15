CONFIG_MK = config.mk
-include ${CONFIG_MK}

all:
	@if [ "${TARGET}" == "" -o "${TARGET_DIR}" == "" -o "${OUT_ELF}" == "" -o "${TARGET_HEX}" == "" -o "${TARGET_BIN}" == "" ]; then echo "** please make <xxx_config> first **" && exit 1; fi
	@make clean \
	&& hb build -f --gn-flags="--export-compile-commands" \
	&& mkdir -p ${TARGET_DIR} \
	&& arm-none-eabi-size ${OUT_ELF} \
	&& arm-none-eabi-objdump -D ${OUT_ELF} > ${OUT_DIS} \
	&& arm-none-eabi-objcopy -O ihex ${OUT_ELF} ${TARGET_HEX}  \
	&& echo "-- target hexfile: ${TARGET_HEX}" \
	&& arm-none-eabi-objcopy -O binary -S ${OUT_ELF} ${TARGET_BIN} \
	&& echo "-- target binfile: ${TARGET_BIN}" \
	&& make install \
	|| echo "** failed **"

# mkconfig: <TARGET>
define mkconfig
	@echo "Configuring for target: <${1}> ..."
	@echo "TARGET=${1}" > ${CONFIG_MK}
	@echo "OUT_DIR=out/${1}/${1}" >> ${CONFIG_MK}
	@echo "OUT_ELF=out/${1}/${1}/OHOS_Image" >> ${CONFIG_MK} 
	@echo "OUT_DIS=out/${1}/${1}/OHOS_Image.dis" >> ${CONFIG_MK} 
	@echo "TARGET_DIR=targets/${1}" >> ${CONFIG_MK}
	@echo "TARGET_HEX=targets/${1}/${1}.hex" >> ${CONFIG_MK}
	@echo "TARGET_BIN=targets/${1}/${1}.bin" >> ${CONFIG_MK}
	@hb set -p ${1}
	@if [ "$$(readlink compile_commands.json)" != "out/${1}/${1}/compile_commands.json" ]; then rm -f ./compile_commands.json && ln -s out/${1}/${1}/compile_commands.json ./compile_commands.json; fi
endef

gd32f470vi_config:
	$(call mkconfig,${@:_config=})

gd32f470zi_config:
	$(call mkconfig,${@:_config=})

menuconfig:
	@make -C ./kernel/liteos_m/ menuconfig

install:
	cp -f ${TARGET_HEX} ${TARGET_BIN} /mnt/Downloads/

clean:
	rm -fr out/${TARGET}

distclean:
	rm -fr out/* compile_commands.json 

git_reset:
	git reset

git_add:
	git add README.md
	git add LICENSE
	git add Makefile
	git add config.mk
	git add vendor/my_company/
	git add device/soc/gd/
	git add device/board/my_company/
	git add targets/

test:
	echo ${TARGET} ${TARGET_HEX}
	
.PHONY: clean distclean install menuconfig git_reset git_add test gd32f470vi_config gd32f470zi_config


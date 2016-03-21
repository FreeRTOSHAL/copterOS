doxygen_dirs = html latex man
doxygen_dirs_abs = $(call objectify, $(doxygen_dirs)) 
clean-dirs += $(doxygen_dirs)

docu: $(obj)/html FORCE

quiet_cmd_doxygen = DOXYGEN	$<
      cmd_doxygen = doxygen $<

$(doxygen_dirs_abs): $(obj)/Doxyfile FORCE
	$(call if_changed,doxygen)
.PHONY: FORCE docu

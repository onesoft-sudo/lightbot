SUBDIRS = src
CONFIG_FILE_CONTENTS = '{\n"prefix": "!",\n"channel": ""\n}'
SUGGESTION_FILE_CONTENTS = "'{}'"

install-exec-hook:
	test -e $(DESTDIR)$(sysconfdir)/lightbot || mkdir -p $(DESTDIR)$(sysconfdir)/lightbot -m 755
	echo -e $(CONFIG_FILE_CONTENTS) > $(DESTDIR)$(sysconfdir)/lightbot/config.json && echo -e "{}" > $(DESTDIR)$(sysconfdir)/lightbot/suggestions.json
